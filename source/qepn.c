/**
* \file
* \brief QEP-nano implementation.
* \ingroup qep
* \cond
******************************************************************************
* Product: QEP-nano
* Last updated for version 5.3.0
* Last updated on  2014-04-14
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Web:   www.state-machine.com
* Email: info@state-machine.com
******************************************************************************
* \endcond
*/
#include "qpn_port.h" /* QP-nano port */

#ifndef qassert_h
    #include "qassert.h" /* QP assertions */
#endif /* qassert_h */

Q_DEFINE_THIS_MODULE("qepn")

/****************************************************************************/
/****************************************************************************/
#ifndef Q_NMSM

/*! Internal QEP macro to increment the given action table \a act_ */
/**
* \description
* Incrementing a pointer violates the MISRA-C 2004 Rule 17.4(req),
* pointer arithmetic other than array indexing. Encapsulating this violation
* in a macro allows to selectively suppress this specific deviation.
*/
#define QEP_ACT_PTR_INC_(act_) (++(act_))

/*! Helper function to exit the current state up to the transition source */
static void QMsm_exitToTranSource_(QMsm * const me, QMState const *s,
                                   QMState const * const ts);

/*! Helper functionn to exectue a transition-action table. */
static QState QMsm_execTatbl_(QMsm * const me, QMTranActTable const *tatbl);

#ifndef Q_NMSM_HIST
/*! Helper function to execute a transition to history */
static QState QMsm_enterHistory_(QMsm * const me, QMState const * const hist);
#endif /* Q_NMSM_HIST */

/*! Maximum depth of entry levels in a MSM for transition to history. */
#define QMSM_MAX_ENTRY_DEPTH_  ((int_fast8_t)4)

/****************************************************************************/
/**
* \description
* Performs the first step of QMsm initialization by assigning the initial
* pseudostate to the currently active state of the state machine.
*
* \arguments
* \arg[in,out] \c me       pointer (see \ref derivation)
* \arg[in]     \c initial  the top-most initial transition for the MSM.
*
* \note Must be called only by the "constructors" of the derived state
* machines.
*
* \note Must be called only ONCE before QMsm_init_().
*/
void QMsm_ctor(QMsm * const me, QStateHandler initial) {
    static QMsmVtbl const vtbl = { /* QMsm virtual table */
        &QMsm_init_,
        &QMsm_dispatch_
    };
    me->vptr = &vtbl; /* hook the vptr to QMsm virtual table */
    me->state.obj = (void *)0;
    me->temp.fun  = initial;
}

/****************************************************************************/
/**
* \description
* Executes the top-most initial transition in a MSM.
*
* \arguments
* \arg[in,out] \c me pointer (see \ref derivation)
*
* \note Must be called only ONCE after the QMsm_ctor().
*/
void QMsm_init_(QMsm * const me) {
    QState r;

    /** \pre the virtual pointer must be initialized, the top-most initial
    * transition must be initialized, and the initial transition must not
    * be taken yet.
    */
    Q_REQUIRE_ID(100, (me->vptr != (QMsmVtbl const *)0)
                      && (me->temp.fun != Q_STATE_CAST(0))
                      && (me->state.obj == (void *)0));

    r = (*me->temp.fun)(me); /* execute the top-most initial transition */

    /* the top-most initial transition must be taken */
    Q_ASSERT_ID(110, r == (QState)Q_RET_TRAN_INIT);

    /* drill down into the state hierarchy with initial transitions... */
    do {
        r = QMsm_execTatbl_(me, me->temp.tatbl);
    } while (r >= (QState)Q_RET_TRAN_INIT);
}

/****************************************************************************/
/**
* \description
* Dispatches an event for processing to a meta state machine (MSM).
* The processing of an event represents one run-to-completion (RTC) step.
*
* \arguments
* \arg[in,out] \c me pointer (see \ref derivation)
*
* \note
* This function should be called only via the virtual table (see
* QMSM_DISPATCH()) and should NOT be called directly in the applications.
*/
void QMsm_dispatch_(QMsm * const me) {
    QMState const *s = me->state.obj; /* store the current state */
    QMState const *t;
    QState r = (QState)Q_RET_SUPER;

    /** \pre current state must be initialized */
    Q_REQUIRE_ID(200, s != (QMState const *)0);

    /* scan the state hierarchy up to the top state... */
    for (t = s; t != (QMState const *)0; t = t->superstate) {
        r = (*t->stateHandler)(me); /* call state handler function */

        if (r >= (QState)Q_RET_HANDLED) {
            break; /* done scanning the state hierarchy */
        }
    }

    /* any kind of transition taken? */
    if (r >= (QState)Q_RET_TRAN) {
        do {
            /* save the transition-action table before it gets clobbered */
            QMTranActTable const *tatbl = me->temp.tatbl;

            /* was a regular state transition segment taken? */
            if (r == (QState)Q_RET_TRAN) {
                QMsm_exitToTranSource_(me, s, t);
                r = QMsm_execTatbl_(me, tatbl);
            }
            /* was an initial transition segment taken? */
            else if (r == (QState)Q_RET_TRAN_INIT) {
                r = QMsm_execTatbl_(me, tatbl);
            }
#ifndef Q_NMSM_HIST
            /* was a transition segment to history taken? */
            else if (r == (QState)Q_RET_TRAN_HIST) {
                QMState const *hist = me->state.obj; /* save history */
                me->state.obj = s; /* restore the original state */
                QMsm_exitToTranSource_(me, s, t);
                (void)QMsm_execTatbl_(me, tatbl);
                r = QMsm_enterHistory_(me, hist);
            }
#endif /* Q_NMSM_HIST */
            else {
                /* no other return value should be produced */
                Q_ERROR_ID(210);
            }
            s = me->state.obj;
            t = s;

        } while (r >= (QState)Q_RET_TRAN);
    }
}

/****************************************************************************/
/**
* \description
* Helper function to execute transition sequence in a transition-action table.
*
* \arguments
* \arg[in,out] \c me    pointer (see \ref derivation)
* \arg[in]     \c tatbl pointer to the transition-action table
*
* \returns status of the last action from the transition-action table.
*
* \note
* This function is for internal use inside the QEP and should NOT be called
* directly in the applications.
*/
static QState QMsm_execTatbl_(QMsm * const me, QMTranActTable const *tatbl) {
    QActionHandler const *a;
    QState r = (QState)Q_RET_NULL;

    /** \pre the transition-action table pointer must not be NULL */
    Q_REQUIRE_ID(300, tatbl != (QMTranActTable const *)0);

    for (a = &tatbl->act[0]; *a != Q_ACTION_CAST(0); QEP_ACT_PTR_INC_(a)) {
        r = (*(*a))(me); /* call the action through the 'a' pointer */
    }
    if (r >= (QState)Q_RET_TRAN_INIT) {
        me->state.obj = me->temp.tatbl->target; /* the tran. target */
    }
    else {
        me->state.obj = tatbl->target; /* the tran. target */
    }

    return r;
}

/****************************************************************************/
/**
* \description
* Static helper function to exit the current state configuration to the
* source of the transition, which is a hierarchical state machine might
* be a superstate of the current state.
*
* \arguments
* \arg[in,out] \c me   pointer (see \ref derivation)
* \arg[in]     \c s    pointer to the current state
* \arg[in]     \c ts   pointer to the transition source
*/
static void QMsm_exitToTranSource_(QMsm * const me, QMState const *s,
                                   QMState const * const ts)
{
    /* exit states from the current state to the tran. source state */
    while (s != ts) {
        /* exit action provided in state 's'? */
        if (s->exitAction != Q_ACTION_CAST(0)) {
            QState r = (*s->exitAction)(me); /* execute the exit action */

            /*  is it a regular exit? */
            if (r == (QState)Q_RET_EXIT) {
                s = s->superstate; /* advance to the superstate */
            }
            else {
                Q_ERROR_ID(410);
            }
        }
        else {
            s = s->superstate; /* advance to the superstate */
        }
    }
}

#ifndef Q_NMSM_HIST
/****************************************************************************/
/**
* \description
* Static helper function to execute the segment of transition to history
* after entering the composite state and
*
* \arguments
* \arg[in,out] \c me   pointer (see \ref derivation)
* \arg[in]     \c hist pointer to the history substate
*
* \returns Q_RET_INITIAL, if an initial transition has been executed in the
* last entered state or 0 if no initial transition was taken.
*
*/
static QState QMsm_enterHistory_(QMsm * const me,
                                 QMState const * const hist)
{
    QMState const *s;
    QMState const *ts = me->state.obj; /* transition source */
    QMState const *entry[QMSM_MAX_ENTRY_DEPTH_];
    QState r;
    uint_fast8_t i = (uint_fast8_t)0;  /* transition entry path index */

    for (s = hist; s != ts; s = s->superstate) {
        Q_ASSERT_ID(510, s != (QMState const *)0);
        if (s->entryAction != (QActionHandler)0) {
            entry[i] = s;
            ++i;
            Q_ASSERT_ID(520, i <= (uint_fast8_t)Q_DIM(entry));
        }
    }

    /* retrace the entry path in reverse (desired) order... */
    while (i > (uint_fast8_t)0) {
        --i;
        r = (*entry[i]->entryAction)(me); /* run entry action in entry[i] */
    }

    me->state.obj = hist; /* set current state to the transition target */

    /* initial tran. present? */
    if (hist->initAction != (QActionHandler)0) {
        r = (*hist->initAction)(me); /* execute the transition action */
    }
    else {
        r = (QState)Q_RET_NULL;
    }
    return r;
}
#endif /* Q_NMSM_HIST */
#endif /* Q_NMSM */


/****************************************************************************/
/****************************************************************************/
#ifndef Q_NHSM

/*! empty signal for internal use only */
#define QEP_EMPTY_SIG_        ((QSignal)0)

/*! maximum depth of state nesting in HSMs (including the top level),
* must be >= 2
*/
#define QHSM_MAX_NEST_DEPTH_  ((int_fast8_t)5)

/*! helper function to execute a transition chain in HSM */
static int_fast8_t QHsm_tran_(QHsm * const me,
                              QStateHandler path[QHSM_MAX_NEST_DEPTH_]);

/****************************************************************************/
/**
* \description
* Performs the first step of HSM initialization by assigning the initial
* pseudostate to the currently active state of the state machine.
*
* \arguments
* \arg[in,out] \c me      pointer (see \ref derivation)
* \arg[in]     \c initial pointer to the top-most initial state-handler
*                         function in the derived state machine
*
* \note Must be called only by the constructors of the derived state
* machines.
*
* \note Must be called only ONCE before QMSM_INIT().
*
* \note
* QHsm inherits QMsm, so by the \ref derivation convention it should call the
* constructor of the superclass, i.e., QMsm_ctor(). However, this would pull
* in the QMsmVtbl, which in turn will pull in the code for QMsm_init_() and
* QMsm_dispatch_() implemetations. To avoid this code size penalty, in case
* ::QMsm is not used in a given project, the QHsm_ctor() performs direct
* intitialization of the Vtbl, which avoids pulling in the code for QMsm.
*
* \usage
* The following example illustrates how to invoke QHsm_ctor() in the
* "constructor" of a derived state machine:
* \include qepn_qhsm_ctor.c
*/
void QHsm_ctor(QHsm * const me, QStateHandler initial) {
    static QMsmVtbl const vtbl = { /* QHsm virtual table */
        &QHsm_init_,
        &QHsm_dispatch_
    };
    /* do not call the QMsm_ctor() here */
    me->vptr = &vtbl;
    me->state.fun = Q_STATE_CAST(&QHsm_top);
    me->temp.fun  = initial;
}

/****************************************************************************/
/**
* \description
* QHsm_top() is the ultimate root of state hierarchy in all HSMs derived
* from ::QHsm.
*
* \arguments
* \arg[in] \c me pointer (see \ref derivation)
*
* \returns Always returns #Q_RET_IGNORED, which means that the top state
*          ignores all events.
*
* \note The argument \c me to this state handler is not used. It is provided
* for conformance with the state-handler function signature ::QStateHandler.
*/
QState QHsm_top(void const * const me) {
    (void)me;  /* suppress the "unused argument" compiler warning */
    return Q_IGNORED(); /* the top state ignores all events */
}

/****************************************************************************/
/**
* \description
* Executes the top-most initial transition in a HSM.
*
* \arguments
* \arg[in,out] \c me pointer (see \ref derivation)
*
* \note Must be called only ONCE after the QHsm_ctor().
*/
void QHsm_init_(QHsm * const me) {
    QStateHandler t = me->state.fun;
    QState r;

    /** \pre the virtual pointer must be initialized, the top-most initial
    * transition must be initialized, and the initial transition must not
    * be taken yet.
    */
    Q_REQUIRE_ID(600, (me->vptr != (QMsmVtbl const *)0)
                      && (me->temp.fun != Q_STATE_CAST(0))
                      && (t == Q_STATE_CAST(&QHsm_top)));

    r = (*me->temp.fun)(me); /* execute the top-most initial transition */

    /* the top-most initial transition must be taken */
    Q_ASSERT_ID(610, r == (QState)Q_RET_TRAN);

    /* drill down into the state hierarchy with initial transitions... */
    do {
        QStateHandler path[QHSM_MAX_NEST_DEPTH_];
        int_fast8_t ip = (int_fast8_t)0; /* transition entry path index */

        path[0] = me->temp.fun;
        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
        (void)(*me->temp.fun)(me);
        while (me->temp.fun != t) {
            ++ip;
            Q_ASSERT_ID(620, ip < (int_fast8_t)Q_DIM(path));
            path[ip] = me->temp.fun;
            (void)(*me->temp.fun)(me);
        }
        me->temp.fun = path[0];

        /* retrace the entry path in reverse (desired) order... */
        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        do {
            (void)(*path[ip])(me); /* enter path[ip] */
            --ip;
        } while (ip >= (int_fast8_t)0);

        t = path[0]; /* current state becomes the new source */

        Q_SIG(me) = (QSignal)Q_INIT_SIG;
        r = (*t)(me);
    } while (r == (QState)Q_RET_TRAN);

    me->state.fun = t; /* change the current active state */
    me->temp.fun  = t; /* mark the configuration as stable */
}

/****************************************************************************/
/**
* \description
* Dispatches an event for processing to a hierarchical state machine (HSM).
* The processing of an event represents one run-to-completion (RTC) step.
*
* \arguments
* \arg[in,out] \c me pointer (see \ref derivation)
*
* \note
* This function should be called only via the virtual table (see
* QMSM_DISPATCH()) and should NOT be called directly in the applications.
*/
void QHsm_dispatch_(QHsm * const me) {
    QStateHandler t = me->state.fun;
    QStateHandler s;
    QState r;
    int_fast8_t iq; /* helper transition entry path index */

    /** \pre the state configuration must be stable */
    Q_REQUIRE_ID(700, t == me->temp.fun);

    /* process the event hierarchically... */
    do {
        s = me->temp.fun;
        r = (*s)(me); /* invoke state handler s */

        /* unhandled due to a guard? */
        if (r == (QState)Q_RET_UNHANDLED) {
            iq = (int_fast8_t)Q_SIG(me); /* save the original signal */
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_; /* find the superstate */
            r = (*s)(me); /* invoke state handler s */
            Q_SIG(me) = (QSignal)iq; /* restore the original signal */
        }
    } while (r == (QState)Q_RET_SUPER);

    /* transition taken? */
    if (r >= (QState)Q_RET_TRAN) {
        QStateHandler path[QHSM_MAX_NEST_DEPTH_]; /* transition entry path */
        int_fast8_t ip; /* transition entry path index */

        path[0] = me->temp.fun; /* save the target of the transition */
        path[1] = t;
        path[2] = s;

        /* exit current state to transition source s... */
        for (; t != s; t = me->temp.fun) {
            Q_SIG(me) = (QSignal)Q_EXIT_SIG; /* find superstate of t */

            /* take the exit action and check if it was handled? */
            if ((*t)(me) == (QState)Q_RET_HANDLED) {
                Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                (void)(*t)(me); /* find superstate of t */
            }
        }

        ip = QHsm_tran_(me, path); /* take the state transition */

        /* retrace the entry path in reverse (desired) order... */
        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        for (; ip >= (int_fast8_t)0; --ip) {
            (void)(*path[ip])(me); /* enter path[ip] */
        }
        t = path[0];      /* stick the target into register */
        me->temp.fun = t; /* update the current state */

        /* drill into the target hierarchy... */
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
        while ((*t)(me) == (QState)Q_RET_TRAN) {
            ip = (int_fast8_t)0;

            path[0] = me->temp.fun;
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*me->temp.fun)(me); /* find the superstate */
            while (me->temp.fun != t) {
                ++ip;
                path[ip] = me->temp.fun;
                (void)(*me->temp.fun)(me); /* find the superstate */
            }
            me->temp.fun = path[0];

            /* entry path must not overflow */
            Q_ASSERT_ID(710, ip < QHSM_MAX_NEST_DEPTH_);

            /* retrace the entry path in reverse (correct) order... */
            Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
            do {
                (void)(*path[ip])(me); /* enter path[ip] */
                --ip;
            } while (ip >= (int_fast8_t)0);

            t = path[0];
            Q_SIG(me) = (QSignal)Q_INIT_SIG;
        }
    }

    me->state.fun = t; /* change the current active state */
    me->temp.fun  = t; /* mark the configuration as stable */
}

/****************************************************************************/
/**
* \description
* Static helper function to execute transition sequence in a hierarchical
* state machine (HSM).
*
* \arguments
* \arg[in,out] \c me   pointer (see \ref derivation)
* \arg[in,out] \c path array of pointers to state-handler functions
*                      to execute the entry actions
* \returns the depth of the entry path stored in the \c path argument.
*/
static int_fast8_t QHsm_tran_(QHsm * const me,
                              QStateHandler path[QHSM_MAX_NEST_DEPTH_])
{
    int_fast8_t ip = (int_fast8_t)(-1); /* transition entry path index */
    int_fast8_t iq; /* helper transition entry path index */
    QStateHandler t = path[0];
    QStateHandler s = path[2];
    QState r;

    /* (a) check source==target (transition to self) */
    if (s == t) {
        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
        (void)(*s)(me);      /* exit the source */
        ip = (int_fast8_t)0; /* enter the target */
    }
    else {
        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
        (void)(*t)(me); /* find superstate of target */
        t = me->temp.fun;

        /* (b) check source==target->super */
        if (s == t) {
            ip = (int_fast8_t)0; /* enter the target */
        }
        else {
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*s)(me); /* find superstate of source */

            /* (c) check source->super==target->super */
            if (me->temp.fun == t) {
                Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                (void)(*s)(me);     /* exit the source */
                ip = (int_fast8_t)0; /* enter the target */
            }
            else {
                /* (d) check source->super==target */
                if (me->temp.fun == path[0]) {
                    Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                    (void)(*s)(me); /* exit the source */
                }
                else {
                    /* (e) check rest of source==target->super->super..
                    * and store the entry path along the way
                    */
                    iq = (int_fast8_t)0; /* indicate that LCA not found */
                    ip = (int_fast8_t)1; /* enter target and its superstate */
                    path[1] = t; /* save the superstate of target */
                    t = me->temp.fun; /* save source->super */

                    /* find target->super->super... */
                    Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                    r = (*path[1])(me);
                    while (r == (QState)Q_RET_SUPER) {
                        ++ip;
                        path[ip] = me->temp.fun; /* store the entry path */
                        if (me->temp.fun == s) { /* is it the source? */
                            iq = (int_fast8_t)1; /* indicate that LCA found */

                            /* entry path must not overflow */
                            Q_ASSERT_ID(810, ip < QHSM_MAX_NEST_DEPTH_);
                            --ip; /* do not enter the source */
                            r = (QState)Q_RET_HANDLED; /* terminate loop */
                        }
                        /* it is not the source, keep going up */
                        else {
                            r = (*me->temp.fun)(me); /* superstate of t */
                        }
                    }

                    /* the LCA not found yet? */
                    if (iq == (int_fast8_t)0) {

                        /* entry path must not overflow */
                        Q_ASSERT_ID(820, ip < QHSM_MAX_NEST_DEPTH_);

                        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                        (void)(*s)(me); /* exit the source */

                        /* (f) check the rest of source->super
                        *                  == target->super->super...
                        */
                        iq = ip;
                        r = (QState)Q_RET_IGNORED; /* LCA NOT found */
                        do {
                            s = path[iq];
                            /* is this the LCA? */
                            if (t == s) {
                                r = (QState)Q_RET_HANDLED; /* LCA found */

                                /* do not enter LCA */
                                ip = (int_fast8_t)(iq - (int_fast8_t)1);
                                iq = (int_fast8_t)(-1); /* terminate loop */
                            }
                            else {
                                --iq; /* try lower superstate of target */
                            }
                        } while (iq >= (int_fast8_t)0);

                        /* LCAnot found? */
                        if (r != (QState)Q_RET_HANDLED) {
                            /* (g) check each source->super->...
                            * for each target->super...
                            */
                            r = (QState)Q_RET_IGNORED; /* keep looping */
                            do {
                                /* exit t unhandled? */
                                Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                                if ((*t)(me) == (QState)Q_RET_HANDLED) {
                                    Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                                    (void)(*t)(me); /* find super of t */
                                }
                                t = me->temp.fun; /*  set to super of t */
                                iq = ip;
                                do {
                                    s = path[iq];
                                    /* is this LCA? */
                                    if (t == s) {
                                        /* do not enter LCA */
                                        ip = (int_fast8_t)(iq-(int_fast8_t)1);
                                        /* cause breaking out of inner loop */
                                        iq = (int_fast8_t)(-1);
                                        r = (QState)Q_RET_HANDLED; /* break */
                                    }
                                    else {
                                        --iq;
                                    }
                                } while (iq >= (int_fast8_t)0);
                            } while (r != (QState)Q_RET_HANDLED);
                        }
                    }
                }
            }
        }
    }
    return ip;
}
#endif /* Q_NHSM */


/****************************************************************************/
/****************************************************************************/
#ifndef Q_NFSM

/****************************************************************************/
/**
* \description
* Performs the first step of FSM initialization by assigning the initial
* pseudostate to the currently active state of the state machine.
*
* \arguments
* \arg[in,out] \c me      pointer (see \ref derivation)
* \arg[in]     \c initial pointer to the top-most initial state-handler
*                         function in the derived state machine
*
* \note Must be called only by the constructors of the derived state
* machines.
*
* \note Must be called only ONCE before QMSM_INIT().
*
* \note
* QFsm inherits QMsm, so by the \ref derivation convention it should call the
* constructor of the superclass, i.e., QMsm_ctor(). However, this would pull
* in the QMsmVtbl, which in turn will pull in the code for QMsm_init_() and
* QMsm_dispatch_() implemetations. To avoid this code size penalty, in case
* ::QMsm is not used in a given project, the QFsm_ctor() performs direct
* intitialization of the Vtbl, which avoids pulling in the code for QMsm.
*
* \usage
* The following example illustrates how to invoke QFsm_ctor() in the
* "constructor" of a derived state machine:
* \include qepn_qfsm_ctor.c
*/
void QFsm_ctor(QFsm * const me, QStateHandler initial) {
    static QMsmVtbl const vtbl = { /* QFsm virtual table */
        &QFsm_init_,
        &QFsm_dispatch_
    };
    /* do not call the QMsm_ctor() here, see the note */
    me->vptr = &vtbl; /* hook the vptr to QFsm virtual table */
    me->state.fun = Q_STATE_CAST(0);
    me->temp.fun  = initial;
}

/****************************************************************************/
/**
* \description
* Executes the top-most initial transition in a FSM.
*
* \arguments
* \arg[in,out] \c me pointer (see \ref derivation)
* \arg[in]     \c e  pointer to the initialization event (might be NULL)
*
* \note Must be called only __once__ after the QFsm_ctor() and before
* QFsm_dispatch_().
*/
void QFsm_init_(QFsm * const me) {

    /** \pre the virtual pointer must be initialized, the top-most initial
    * transition must be initialized, and the initial transition must not
    * be taken yet.
    */
    Q_REQUIRE_ID(800, (me->vptr != (QMsmVtbl const *)0)
                      && (me->temp.fun != Q_STATE_CAST(0))
                      && (me->state.fun == Q_STATE_CAST(0)));

    /* execute the top-most initial transition, which must be taken */
    Q_ALLEGE_ID(810, (*me->temp.fun)(me) == (QState)Q_RET_TRAN);

    Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    (void)(*me->temp.fun)(me);    /* enter the target */
    me->state.fun = me->temp.fun; /* change the new active state */
}

/****************************************************************************/
/**
* \description
* Dispatches an event for processing to a non-hierarchical ("flat") state
* machine (FSM). The processing of an event represents one run-to-completion
* (RTC) step.
*
* \arguments
* \arg[in,out] \c me pointer (see \ref derivation)
*/
void QFsm_dispatch_(QFsm *const me) {

    /* tran. taken? */
    if ((*me->state.fun)(me) == (QState)Q_RET_TRAN) {
        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
        (void)(*me->state.fun)(me);   /* exit the source */

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        (void)(*me->temp.fun)(me);    /* enter the target */
        me->state.fun = me->temp.fun; /* record the new active state */
    }
}
#endif /* Q_NFSM */

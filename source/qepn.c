/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 29, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
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
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"                                       /* QP-nano port */

Q_DEFINE_THIS_MODULE("qepn")

/**
* \file
* \ingroup qepn qfn
* QEP-nano implementation.
*/

/****************************************************************************/
#ifndef Q_NMSM

/** \brief Internal QEP macro to increment the given action table \a act_
*
* \note Incrementing a pointer violates the MISRA-C 2004 Rule 17.4(req),
* pointer arithmetic other than array indexing. Encapsulating this violation
* in a macro allows to selectively suppress this specific deviation.
*/
#define QEP_ACT_PTR_INC_(act_) (++(act_))

/*..........................................................................*/
QActionHandler const QMsm_emptyAction_[1] = {
    Q_ACTION_CAST(0)
};

/*..........................................................................*/
void QMsm_ctor(QMsm * const me, QStateHandler initial) {
    static QMsmVtbl const vtbl = {                    /* QMsm virtual table */
        &QMsm_init_,
        &QMsm_dispatch_
    };
    me->vptr = &vtbl;                /* hook the vptr to QMsm virtual table */
    me->state.obj = (void *)0;
    me->temp.fun  = initial;
}
/*..........................................................................*/
void QMsm_init_(QMsm * const me) {
    QState r;

    Q_REQUIRE((me->vptr != (QMsmVtbl const *)0)    /* ctor must be executed */
              && (me->temp.fun != Q_STATE_CAST(0)) /* ctor must be executed */
              && (me->state.obj == (void *)0));  /* initial tran. NOT taken */

    r = (*me->temp.fun)(me);     /* execute the top-most initial transition */
    Q_ASSERT(r == (QState)Q_RET_INITIAL);    /* initial tran. must be taken */

    while (r == (QState)Q_RET_INITIAL) {
        QActionHandler const *a;

        r = (QState)0U;                      /* invalidate the return value */
        for (a = me->temp.act; *a != Q_ACTION_CAST(0); QEP_ACT_PTR_INC_(a)) {
            r = (*(*a))(me);                          /* execute the action */
        }
    }
}
/*..........................................................................*/
void QMsm_dispatch_(QMsm * const me) {
    QMState const *s = me->state.obj;            /* store the current state */
    QMState const *t;
    QState r = (QState)Q_RET_HANDLED;

    Q_REQUIRE(s != (QMState const *)0);              /* must be initialized */


    for (t = s; t != (QMState const *)0; t = t->parent) {
        r = (*t->stateHandler)(me);
        if (r != (QState)Q_RET_SUPER) {
            if (r == (QState)Q_RET_UNHANDLED) {/* unhandled due to a guard? */
                /* empty */
            }
            else {
                break;              /* event handled--break out of the loop */
            }
        }
    }

    if (r == (QState)Q_RET_TRAN) {                     /* transition taken? */
        QActionHandler const *a = me->temp.act;

         /* exit states starting from the current state to the source state */
        for (; s != t; s = s->parent) {
            if (s->exitAction != Q_ACTION_CAST(0)) {/* exit action defined? */
                (void)(*s->exitAction)(me);      /* execute the exit action */
            }
        }
        /*  at this point s == t and both hold the source of the transition */

        for (; *a != Q_ACTION_CAST(0); QEP_ACT_PTR_INC_(a)) {
            r = (*(*a))(me);                          /* execute the action */
        }

        while (r == (QState)Q_RET_INITIAL) {
            r = (QState)0U;                  /* invalidate the return value */
            for (a = me->temp.act;
                 *a != Q_ACTION_CAST(0);
                 QEP_ACT_PTR_INC_(a))
            {
                r = (*(*a))(me);                       /* execute the action */
            }
        }
    }
}
#endif                                                            /* Q_NMSM */


/****************************************************************************/
#ifndef Q_NHSM

/** empty signal for internal use only */
#define QEP_EMPTY_SIG_        ((QSignal)0)

/** maximum depth of state nesting (including the top level), must be >= 2 */
#define QEP_MAX_NEST_DEPTH_   ((uint8_t)5)

/*..........................................................................*/
void QHsm_ctor(QHsm * const me, QStateHandler initial) {
    static QMsmVtbl const vtbl = {                    /* QHsm virtual table */
        &QHsm_init_,
        &QHsm_dispatch_
    };

    me->vptr = &vtbl;                /* hook the vptr to QHsm virtual table */
    me->state.fun = Q_STATE_CAST(&QHsm_top);
    me->temp.fun  = initial;
}
/*..........................................................................*/
QState QHsm_top(void const * const me) {
    (void)me;            /* suppress the "unused argument" compiler warning */
    return Q_IGNORED();                 /* the top state ignores all events */
}
/*..........................................................................*/
void QHsm_init_(QHsm * const me) {
    QStateHandler t = me->state.fun;

    Q_REQUIRE((me->temp.fun != Q_STATE_CAST(0))    /* ctor must be executed */
              && (t == Q_STATE_CAST(&QHsm_top))); /*initial tran. NOT taken */

    Q_ALLEGE((*me->temp.fun)(me) == (QState)Q_RET_TRAN);/* init tran. taken */

    do {                              /* drill into the target hierarchy... */
        QStateHandler path[QEP_MAX_NEST_DEPTH_];
        int8_t ip = (int8_t)0;

        path[0] = me->temp.fun;
        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
        (void)(*me->temp.fun)(me);
        while (me->temp.fun != t) {
            ++ip;
            path[ip] = me->temp.fun;
            (void)(*me->temp.fun)(me);
        }
        me->temp.fun = path[0];
                                            /* entry path must not overflow */
        Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        do {        /* retrace the entry path in reverse (correct) order... */
            (void)(*path[ip])(me);                        /* enter path[ip] */
            --ip;
        } while (ip >= (int8_t)0);

        t = path[0];
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
    } while ((*t)(me) == (QState)Q_RET_TRAN);/* initial transition handled? */

    me->state.fun = t;                   /* change the current active state */
    me->temp.fun  = t;                  /* mark the configuration as stable */
}
/*..........................................................................*/
static int8_t QHsm_tran_(QHsm * const me,
                         QStateHandler path[QEP_MAX_NEST_DEPTH_]);
/*..........................................................................*/
void QHsm_dispatch_(QHsm * const me) {
    QStateHandler t = me->state.fun;
    QStateHandler s;
    QState r;
    int8_t iq;                        /* helper transition entry path index */

    Q_REQUIRE(t == me->temp.fun); /* the state configuration must be stable */

    do {                             /* process the event hierarchically... */
        s = me->temp.fun;
        r = (*s)(me);                             /* invoke state handler s */

        if (r == (QState)Q_RET_UNHANDLED) {    /* unhandled due to a guard? */
            iq = (int8_t)Q_SIG(me);             /* save the original signal */
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;     /* find the superstate */
            r = (*s)(me);                         /* invoke state handler s */
            Q_SIG(me) = (QSignal)iq;         /* restore the original signal */
        }
    } while (r == (QState)Q_RET_SUPER);

    if (r == (QState)Q_RET_TRAN) {                     /* transition taken? */
        QStateHandler path[QEP_MAX_NEST_DEPTH_];   /* transition entry path */
        int8_t ip;                           /* transition entry path index */

        path[0] = me->temp.fun;        /* save the target of the transition */
        path[1] = t;
        path[2] = s;

        while (t != s) {    /* exit current state to transition source s... */
            Q_SIG(me) = (QSignal)Q_EXIT_SIG;        /* find superstate of t */
            if ((*t)(me) == (QState)Q_RET_HANDLED) {/* exit action handled? */
                Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                (void)(*t)(me);                     /* find superstate of t */
            }
            t = me->temp.fun;              /* me->temp holds the superstate */
        }

        ip = QHsm_tran_(me, path);             /* take the state transition */

                    /* retrace the entry path in reverse (desired) order... */
        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        for (; ip >= (int8_t)0; --ip) {
            (void)(*path[ip])(me);                        /* enter path[ip] */
        }
        t = path[0];                      /* stick the target into register */
        me->temp.fun = t;                       /* update the current state */

                                      /* drill into the target hierarchy... */
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
        while ((*t)(me) == (QState)Q_RET_TRAN) {
            ip = (int8_t)0;

            path[0] = me->temp.fun;
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*me->temp.fun)(me);               /* find the superstate */
            while (me->temp.fun != t) {
                ++ip;
                path[ip] = me->temp.fun;
                (void)(*me->temp.fun)(me);           /* find the superstate */
            }
            me->temp.fun = path[0];
                                            /* entry path must not overflow */
            Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

            Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
            do {    /* retrace the entry path in reverse (correct) order... */
                (void)(*path[ip])(me);                    /* enter path[ip] */
                --ip;
            } while (ip >= (int8_t)0);

            t = path[0];
            Q_SIG(me) = (QSignal)Q_INIT_SIG;
        }
    }

    me->state.fun = t;                   /* change the current active state */
    me->temp.fun  = t;                  /* mark the configuration as stable */
}
/*..........................................................................*/
static int8_t QHsm_tran_(QHsm * const me,
                         QStateHandler path[QEP_MAX_NEST_DEPTH_])
{
    int8_t ip = (int8_t)(-1);                /* transition entry path index */
    int8_t iq;                        /* helper transition entry path index */
    QStateHandler t = path[0];
    QStateHandler s = path[2];
    QState r;

    if (s == t) {          /* (a) check source==target (transition to self) */
        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
        (void)(*s)(me);                                  /* exit the source */
        ip = (int8_t)0;                                 /* enter the target */
    }
    else {
        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
        (void)(*t)(me);                        /* find superstate of target */
        t = me->temp.fun;
        if (s == t) {                    /* (b) check source==target->super */
            ip = (int8_t)0;                             /* enter the target */
        }
        else {
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*s)(me);                    /* find superstate of source */

                                  /* (c) check source->super==target->super */
            if (me->temp.fun == t) {
                Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                (void)(*s)(me);                          /* exit the source */
                ip = (int8_t)0;                         /* enter the target */
            }
            else {
                                         /* (d) check source->super==target */
                if (me->temp.fun == path[0]) {
                    Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                    (void)(*s)(me);                      /* exit the source */
                }
                else { /* (e) check rest of source==target->super->super..
                        * and store the entry path along the way
                        */
                    iq = (int8_t)0;          /* indicate that LCA not found */
                    ip = (int8_t)1;      /* enter target and its superstate */
                    path[1] = t;           /* save the superstate of target */
                    t = me->temp.fun;                 /* save source->super */

                    Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                    r = (*path[1])(me);        /* find target->super->super */
                    while (r == (QState)Q_RET_SUPER) {
                        ++ip;
                        path[ip] = me->temp.fun;    /* store the entry path */
                        if (me->temp.fun == s) {       /* is it the source? */
                            iq = (int8_t)1;      /* indicate that LCA found */
                                            /* entry path must not overflow */
                            Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
                            --ip;                /* do not enter the source */
                            r = (QState)Q_RET_HANDLED;    /* terminate loop */
                        }
                        else {       /* it is not the source, keep going up */
                            r = (*me->temp.fun)(me);     /* superstate of t */
                        }
                    }
                    if (iq == (int8_t)0) {        /* the LCA not found yet? */

                                            /* entry path must not overflow */
                        Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

                        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                        (void)(*s)(me);                  /* exit the source */

                        /* (f) check the rest of source->super
                        *                  == target->super->super...
                        */
                        iq = ip;
                        r = (QState)Q_RET_IGNORED;         /* LCA NOT found */
                        do {
                            s = path[iq];
                            if (t == s) {               /* is this the LCA? */
                                r = (QState)Q_RET_HANDLED;     /* LCA found */
                                                        /* do not enter LCA */
                                ip = (int8_t)(iq - (int8_t)1);
                                iq = (int8_t)(-1);    /* terminate the loop */
                            }
                            else {
                                --iq;     /* try lower superstate of target */
                            }
                        } while (iq >= (int8_t)0);

                        if (r != (QState)Q_RET_HANDLED) {  /* LCAnot found? */
                            /* (g) check each source->super->...
                             * for each target->super...
                             */
                            r = (QState)Q_RET_IGNORED;      /* keep looping */
                            do {
                                                       /* exit t unhandled? */
                                Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                                if ((*t)(me) == (QState)Q_RET_HANDLED) {
                                    Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                                    (void)(*t)(me);      /* find super of t */
                                }
                                t = me->temp.fun;     /*  set to super of t */
                                iq = ip;
                                do {
                                    s = path[iq];
                                    if (t == s) {           /* is this LCA? */
                                                        /* do not enter LCA */
                                        ip = (int8_t)(iq - (int8_t)1);
                                        iq = (int8_t)(-1);   /* break inner */
                                        r = (QState)Q_RET_HANDLED; /* break */
                                    }
                                    else {
                                        --iq;
                                    }
                                } while (iq >= (int8_t)0);
                            } while (r != (QState)Q_RET_HANDLED);
                        }
                    }
                }
            }
        }
    }
    return ip;
}
#endif                                                            /* Q_NHSM */


/****************************************************************************/
#ifndef Q_NFSM
/*..........................................................................*/
void QFsm_ctor(QFsm * const me, QStateHandler initial) {
    static QMsmVtbl const vtbl = {                    /* QFsm virtual table */
        &QFsm_init_,
        &QFsm_dispatch_
    };

    me->vptr = &vtbl;                /* hook the vptr to QFsm virtual table */
    me->state.fun = Q_STATE_CAST(0);
    me->temp.fun  = initial;
}
/*..........................................................................*/
void QFsm_init_(QFsm * const me) {
    Q_REQUIRE((me->temp.fun != Q_STATE_CAST(0))    /* ctor must be executed */
              && (me->state.fun == Q_STATE_CAST(0)));  /* initial NOT taken */

                                 /* execute the top-most initial transition */
    Q_ALLEGE((*me->temp.fun)(me) == (QState)Q_RET_TRAN);     /* tran. taken */

    Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    (void)(*me->temp.fun)(me);                          /* enter the target */
    me->state.fun = me->temp.fun;            /* change the new active state */
}
/*..........................................................................*/
void QFsm_dispatch_(QFsm *const me) {
    if ((*me->state.fun)(me) == (QState)Q_RET_TRAN) {       /* tran. taken? */
        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
        (void)(*me->state.fun)(me);                      /* exit the source */

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        (void)(*me->temp.fun)(me);                      /* enter the target */
        me->state.fun = me->temp.fun;        /* record the new active state */
    }
}
#endif                                                            /* Q_NFSM */

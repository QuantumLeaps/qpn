/*****************************************************************************
* Product: QEP-nano implemenation
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"                                       /* QP-nano port */

#ifndef Q_NHSM
Q_DEFINE_THIS_MODULE("qepn")
#endif

/**
* \file
* \ingroup qepn qfn
* QEP-nano implementation.
*/

/** empty signal for internal use only */
#define QEP_EMPTY_SIG_        0

/** maximum depth of state nesting (including the top level), must be >= 2 */
#define QEP_MAX_NEST_DEPTH_   5

/*..........................................................................*/
/*lint -e970 -e971 */      /* ignore MISRA rules 13 and 14 in this function */
char const Q_ROM * Q_ROM_VAR QP_getVersion(void) {
    static char const Q_ROM Q_ROM_VAR version[] = "4.4.00";
    return version;
}

#ifndef Q_NFSM
/*..........................................................................*/
void QFsm_init(QFsm *me) {
    (void)(*me->state)(me);      /* execute the top-most initial transition */

    Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    (void)(*me->state)(me);                             /* enter the target */
}
/*..........................................................................*/
#ifndef QK_PREEMPTIVE
void QFsm_dispatch(QFsm *me) {
#else
void QFsm_dispatch(QFsm *me) Q_REENTRANT {
#endif
    QStateHandler s = me->state;

    if ((*s)(me) == Q_RET_TRAN) {                      /* transition taken? */
        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
        (void)(*s)(me);                                  /* exit the source */

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        (void)(*me->state)(me);                         /* enter the target */
    }
}
#endif                                                            /* Q_NFSM */

#ifndef Q_NHSM
/*..........................................................................*/
QState QHsm_top(QHsm *me) {
    (void)me;             /* supress the "unused argument" compiler warning */
    return Q_IGNORED();                 /* the top state ignores all events */
}
/*..........................................................................*/
void QHsm_init(QHsm *me) {
    QStateHandler t;

    Q_ALLEGE((*me->state)(me) == Q_RET_TRAN);/* initial tran. must be taken */

    t = (QStateHandler)&QHsm_top;         /* an HSM starts in the top state */
    do {                              /* drill into the target hierarchy... */
        QStateHandler path[QEP_MAX_NEST_DEPTH_];
        int8_t ip = (int8_t)0;

        path[0] = me->state;
        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
        (void)(*me->state)(me);
        while (me->state != t) {
            path[++ip] = me->state;
            (void)(*me->state)(me);
        }
        me->state = path[0];
                                            /* entry path must not overflow */
        Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        do {        /* retrace the entry path in reverse (correct) order... */
            (void)(*path[ip])(me);                        /* enter path[ip] */
        } while ((--ip) >= (int8_t)0);

        t = path[0];
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
    } while ((*t)(me) == Q_RET_TRAN);        /* initial transition handled? */
    me->state = t;
}
/*..........................................................................*/
#ifndef QK_PREEMPTIVE
void QHsm_dispatch(QHsm *me) {
#else
void QHsm_dispatch(QHsm *me) Q_REENTRANT {
#endif
    QStateHandler path[QEP_MAX_NEST_DEPTH_];
    QStateHandler s;
    QStateHandler t;
    QState r;

    t = me->state;                                /* save the current state */

    do {                             /* process the event hierarchically... */
        s = me->state;
        r = (*s)(me);                             /* invoke state handler s */
    } while (r == Q_RET_SUPER);

    if (r == Q_RET_TRAN) {                             /* transition taken? */
        int8_t ip = (int8_t)(-1);            /* transition entry path index */
        int8_t iq;                    /* helper transition entry path index */

        path[0] = me->state;           /* save the target of the transition */
        path[1] = t;

        while (t != s) {    /* exit current state to transition source s... */
            Q_SIG(me) = (QSignal)Q_EXIT_SIG;        /* find superstate of t */
            if ((*t)(me) == Q_RET_HANDLED) {        /* exit action handled? */
                Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                (void)(*t)(me);                     /* find superstate of t */
            }
            t = me->state;                /* me->state holds the superstate */
        }

        t = path[0];                            /* target of the transition */

        if (s == t) {      /* (a) check source==target (transition to self) */
            Q_SIG(me) = (QSignal)Q_EXIT_SIG;
            (void)(*s)(me);                              /* exit the source */
            ip = (int8_t)0;                             /* enter the target */
        }
        else {
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*t)(me);                    /* find superstate of target */
            t = me->state;
            if (s == t) {                /* (b) check source==target->super */
                ip = (int8_t)0;                         /* enter the target */
            }
            else {
                Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                (void)(*s)(me);                /* find superstate of source */

                                  /* (c) check source->super==target->super */
                if (me->state == t) {
                    Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                    (void)(*s)(me);                      /* exit the source */
                    ip = (int8_t)0;                     /* enter the target */
                }
                else {
                                         /* (d) check source->super==target */
                    if (me->state == path[0]) {
                        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                        (void)(*s)(me);                  /* exit the source */
                    }
                    else { /* (e) check rest of source==target->super->super..
                            * and store the entry path along the way
                            */
                        iq = (int8_t)0;      /* indicate that LCA not found */
                        ip = (int8_t)1;  /* enter target and its superstate */
                        path[1] = t;       /* save the superstate of target */
                        t = me->state;                /* save source->super */

                        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                        r = (*path[1])(me);    /* find target->super->super */
                        while (r == Q_RET_SUPER) {
                            path[++ip] = me->state; /* store the entry path */
                            if (me->state == s) {      /* is it the source? */
                                iq = (int8_t)1;  /* indicate that LCA found */
                                            /* entry path must not overflow */
                                Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
                                --ip;            /* do not enter the source */
                                r = Q_RET_HANDLED;    /* terminate the loop */
                            }
                            else {   /* it is not the source, keep going up */
                                r = (*me->state)(me);    /* superstate of t */
                            }
                        }
                        if (iq == (int8_t)0) {    /* the LCA not found yet? */

                                            /* entry path must not overflow */
                            Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

                            Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                            (void)(*s)(me);              /* exit the source */

                                /* (f) check the rest of source->super
                                 *                  == target->super->super...
                                 */
                            iq = ip;
                            r = Q_RET_IGNORED;    /* indicate LCA NOT found */
                            do {
                                s = path[iq];
                                if (t == s) {           /* is this the LCA? */
                                    r = Q_RET_HANDLED;/* indicate LCA found */
                                    ip = (int8_t)(iq - 1);/*do not enter LCA*/
                                    iq = (int8_t)(-1);/* terminate the loop */
                                }
                                else {
                                    --iq; /* try lower superstate of target */
                                }
                            } while (iq >= (int8_t)0);

                            if (r != Q_RET_HANDLED) { /* LCA not found yet? */
                                    /* (g) check each source->super->...
                                     * for each target->super...
                                     */
                                r = Q_RET_IGNORED;          /* keep looping */
                                do {
                                                       /* exit t unhandled? */
                                    Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                                    if ((*t)(me) == Q_RET_HANDLED) {
                                        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                                        (void)(*t)(me);  /* find super of t */
                                    }
                                    t = me->state;    /*  set to super of t */
                                    iq = ip;
                                    do {
                                        s = path[iq];
                                        if (t == s) {       /* is this LCA? */
                                                        /* do not enter LCA */
                                            ip = (int8_t)(iq - 1);
                                            iq = (int8_t)(-1);/*break inner */
                                            r = Q_RET_HANDLED;/*break outer */
                                        }
                                        else {
                                            --iq;
                                        }
                                    } while (iq >= (int8_t)0);
                                } while (r != Q_RET_HANDLED);
                            }
                        }
                    }
                }
            }
        }
                    /* retrace the entry path in reverse (desired) order... */
        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        for (; ip >= (int8_t)0; --ip) {
            (void)(*path[ip])(me);                        /* enter path[ip] */
        }
        t = path[0];                      /* stick the target into register */
        me->state = t;                          /* update the current state */

                                      /* drill into the target hierarchy... */
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
        while ((*t)(me) == Q_RET_TRAN) {
            ip = (int8_t)0;

            path[0] = me->state;
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*me->state)(me);                  /* find the superstate */
            while (me->state != t) {
                path[++ip] = me->state;
                (void)(*me->state)(me);              /* find the superstate */
            }
            me->state = path[0];
                                            /* entry path must not overflow */
            Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

            Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
            do {    /* retrace the entry path in reverse (correct) order... */
                (void)(*path[ip])(me);                    /* enter path[ip] */
            } while ((--ip) >= (int8_t)0);

            t = path[0];
            Q_SIG(me) = (QSignal)Q_INIT_SIG;
        }
    }
    me->state = t;            /* set new state or restore the current state */
}

#endif                                                            /* Q_NHSM */

/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 4.5.04
* Date of the Last Update:  Feb 02, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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

/** empty signal for internal use only */
#define QEP_EMPTY_SIG_        ((QSignal)0)

/** maximum depth of state nesting (including the top level), must be >= 2 */
#define QEP_MAX_NEST_DEPTH_   ((uint8_t)5)

/*..........................................................................*/
char_t const Q_ROM * Q_ROM_VAR QP_getVersion(void) {
    static char_t const Q_ROM Q_ROM_VAR version[] = {
        (char_t)((uint8_t)((QP_VERSION >> 12) & 0xFU) + (uint8_t)'0'),
        (char_t)'.',
        (char_t)((uint8_t)((QP_VERSION >>  8) & 0xFU) + (uint8_t)'0'),
        (char_t)'.',
        (char_t)((uint8_t)((QP_VERSION >>  4) & 0xFU) + (uint8_t)'0'),
        (char_t)((uint8_t)(QP_VERSION         & 0xFU) + (uint8_t)'0'),
        (char_t)'\0'
    };
    return version;
}

#ifndef Q_NFSM
/*..........................................................................*/
void QFsm_init(QFsm * const me) {
    Q_REQUIRE((me->temp != Q_STATE_CAST(0))        /* ctor must be executed */
              && (me->state == Q_STATE_CAST(0))); /*initial tran. NOT taken */

                                 /* execute the top-most initial transition */
    Q_ALLEGE((*me->temp)(me) == Q_RET_TRAN);    /* transition must be taken */

    Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    (void)(*me->temp)(me);                              /* enter the target */
    me->state = me->temp;                    /* change the new active state */
}
/*..........................................................................*/
#ifndef QK_PREEMPTIVE
void QFsm_dispatch(QFsm *const me) {
#else
void QFsm_dispatch(QFsm *const me) Q_REENTRANT {
#endif

    if ((*me->state)(me) == Q_RET_TRAN) {              /* transition taken? */
        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
        (void)(*me->state)(me);                          /* exit the source */

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        (void)(*me->temp)(me);                          /* enter the target */
        me->state = me->temp;                /* record the new active state */
    }
}
#endif                                                            /* Q_NFSM */

#ifndef Q_NHSM
/*..........................................................................*/
QState QHsm_top(void const * const me) {
    (void)me;            /* suppress the "unused argument" compiler warning */
    return Q_IGNORED();                 /* the top state ignores all events */
}
/*..........................................................................*/
void QHsm_init(QHsm * const me) {
    QStateHandler t = me->state;

    Q_REQUIRE((me->temp != Q_STATE_CAST(0))        /* ctor must be executed */
              && (t == Q_STATE_CAST(&QHsm_top))); /*initial tran. NOT taken */

    Q_ALLEGE((*me->temp)(me) == Q_RET_TRAN); /* initial tran. must be taken */

    do {                              /* drill into the target hierarchy... */
        QStateHandler path[QEP_MAX_NEST_DEPTH_];
        int8_t ip = (int8_t)0;

        path[0] = me->temp;
        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
        (void)(*me->temp)(me);
        while (me->temp != t) {
            ++ip;
            path[ip] = me->temp;
            (void)(*me->temp)(me);
        }
        me->temp = path[0];
                                            /* entry path must not overflow */
        Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);

        Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
        do {        /* retrace the entry path in reverse (correct) order... */
            (void)(*path[ip])(me);                        /* enter path[ip] */
            --ip;
        } while (ip >= (int8_t)0);

        t = path[0];
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
    } while ((*t)(me) == Q_RET_TRAN);        /* initial transition handled? */

    me->state = t;                       /* change the current active state */
    me->temp  = t;                      /* mark the configuration as stable */
}
/*..........................................................................*/
#ifndef QK_PREEMPTIVE
void QHsm_dispatch(QHsm * const me) {
#else
void QHsm_dispatch(QHsm * const me) Q_REENTRANT {
#endif
    QStateHandler t = me->state;
    QStateHandler s;
    QState r;
    int8_t iq;                        /* helper transition entry path index */

    Q_REQUIRE(t == me->temp);     /* the state configuration must be stable */

    do {                             /* process the event hierarchically... */
        s = me->temp;
        r = (*s)(me);                             /* invoke state handler s */

        if (r == Q_RET_UNHANDLED) {            /* unhandled due to a guard? */
            iq = (int8_t)Q_SIG(me);             /* save the original signal */
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;     /* find the superstate */
            r = (*s)(me);                         /* invoke state handler s */
            Q_SIG(me) = (QSignal)iq;         /* restore the original signal */
        }
    } while (r == Q_RET_SUPER);

    if (r == Q_RET_TRAN) {                             /* transition taken? */
        QStateHandler path[QEP_MAX_NEST_DEPTH_];
        int8_t ip = (int8_t)(-1);            /* transition entry path index */

        path[0] = me->temp;            /* save the target of the transition */
        path[1] = t;

        while (t != s) {    /* exit current state to transition source s... */
            Q_SIG(me) = (QSignal)Q_EXIT_SIG;        /* find superstate of t */
            if ((*t)(me) == Q_RET_HANDLED) {        /* exit action handled? */
                Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                (void)(*t)(me);                     /* find superstate of t */
            }
            t = me->temp;                  /* me->temp holds the superstate */
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
            t = me->temp;
            if (s == t) {                /* (b) check source==target->super */
                ip = (int8_t)0;                         /* enter the target */
            }
            else {
                Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                (void)(*s)(me);                /* find superstate of source */

                                  /* (c) check source->super==target->super */
                if (me->temp == t) {
                    Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                    (void)(*s)(me);                      /* exit the source */
                    ip = (int8_t)0;                     /* enter the target */
                }
                else {
                                         /* (d) check source->super==target */
                    if (me->temp == path[0]) {
                        Q_SIG(me) = (QSignal)Q_EXIT_SIG;
                        (void)(*s)(me);                  /* exit the source */
                    }
                    else { /* (e) check rest of source==target->super->super..
                            * and store the entry path along the way
                            */
                        iq = (int8_t)0;      /* indicate that LCA not found */
                        ip = (int8_t)1;  /* enter target and its superstate */
                        path[1] = t;       /* save the superstate of target */
                        t = me->temp;                 /* save source->super */

                        Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
                        r = (*path[1])(me);    /* find target->super->super */
                        while (r == Q_RET_SUPER) {
                            ++ip;
                            path[ip] = me->temp;    /* store the entry path */
                            if (me->temp == s) {       /* is it the source? */
                                iq = (int8_t)1;  /* indicate that LCA found */
                                            /* entry path must not overflow */
                                Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
                                --ip;            /* do not enter the source */
                                r = Q_RET_HANDLED;    /* terminate the loop */
                            }
                            else {   /* it is not the source, keep going up */
                                r = (*me->temp)(me);     /* superstate of t */
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
                                                        /* do not enter LCA */
                                    ip = (int8_t)(iq - (int8_t)1);
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
                                    t = me->temp;     /*  set to super of t */
                                    iq = ip;
                                    do {
                                        s = path[iq];
                                        if (t == s) {       /* is this LCA? */
                                                        /* do not enter LCA */
                                            ip = (int8_t)(iq - (int8_t)1);
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
        me->temp = t;                           /* update the current state */

                                      /* drill into the target hierarchy... */
        Q_SIG(me) = (QSignal)Q_INIT_SIG;
        while ((*t)(me) == Q_RET_TRAN) {
            ip = (int8_t)0;

            path[0] = me->temp;
            Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
            (void)(*me->temp)(me);                   /* find the superstate */
            while (me->temp != t) {
                ++ip;
                path[ip] = me->temp;
                (void)(*me->temp)(me);               /* find the superstate */
            }
            me->temp = path[0];
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

    me->state = t;                       /* change the current active state */
    me->temp  = t;                      /* mark the configuration as stable */
}

#endif                                                            /* Q_NHSM */

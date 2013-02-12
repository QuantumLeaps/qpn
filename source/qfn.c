/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 4.5.04
* Date of the Last Update:  Feb 04, 2013
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

Q_DEFINE_THIS_MODULE("qfn")

/**
* \file
* \ingroup qfn
* QF-nano implementation.
*/

/* Global-scope objects ----------------------------------------------------*/
uint8_t volatile QF_readySet_;                      /* ready-set of QF-nano */

#ifdef Q_TIMERSET
uint8_t volatile QF_timerSet_;                      /* timer-set of QF-nano */
#endif

#ifdef QF_LOG2LKUP
uint8_t const Q_ROM Q_ROM_VAR QF_log2Lkup[256] = {
    (uint8_t)0, (uint8_t)1, (uint8_t)2, (uint8_t)2,
    (uint8_t)3, (uint8_t)3, (uint8_t)3, (uint8_t)3,
    (uint8_t)4, (uint8_t)4, (uint8_t)4, (uint8_t)4,
    (uint8_t)4, (uint8_t)4, (uint8_t)4, (uint8_t)4,
    (uint8_t)5, (uint8_t)5, (uint8_t)5, (uint8_t)5,
    (uint8_t)5, (uint8_t)5, (uint8_t)5, (uint8_t)5,
    (uint8_t)5, (uint8_t)5, (uint8_t)5, (uint8_t)5,
    (uint8_t)5, (uint8_t)5, (uint8_t)5, (uint8_t)5,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)6, (uint8_t)6, (uint8_t)6, (uint8_t)6,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)7, (uint8_t)7, (uint8_t)7, (uint8_t)7,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8,
    (uint8_t)8, (uint8_t)8, (uint8_t)8, (uint8_t)8
};
#endif                                                           /* QF_LOG2 */


/* local objects -----------------------------------------------------------*/
static uint8_t const Q_ROM Q_ROM_VAR l_pow2Lkup[] = {
    (uint8_t)0x00,
    (uint8_t)0x01, (uint8_t)0x02, (uint8_t)0x04, (uint8_t)0x08,
    (uint8_t)0x10, (uint8_t)0x20, (uint8_t)0x40, (uint8_t)0x80
};

#if (defined Q_TIMERSET) || (!(defined QK_PREEMPTIVE))
static uint8_t const Q_ROM Q_ROM_VAR l_invPow2Lkup[] = {
    (uint8_t)0xFF,
    (uint8_t)0xFE, (uint8_t)0xFD, (uint8_t)0xFB, (uint8_t)0xF7,
    (uint8_t)0xEF, (uint8_t)0xDF, (uint8_t)0xBF, (uint8_t)0x7F
};
#endif                  /* (defined Q_TIMERSET) && !(defined QK_PREEMPTIVE) */

/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
void QActive_post(QActive * const me, QSignal sig, QParam par)
#else
void QActive_post(QActive * const me, QSignal sig)
#endif
{
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];

            /* the queue must be able to accept the event (cannot overflow) */
    Q_ASSERT(me->nUsed < Q_ROM_BYTE(acb->end));

    QF_INT_DISABLE();
                                /* insert event into the ring buffer (FIFO) */
    QF_ROM_QUEUE_AT_(acb, me->head).sig = sig;
#if (Q_PARAM_SIZE != 0)
    QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
    if (me->head == (uint8_t)0) {
        me->head = Q_ROM_BYTE(acb->end);                   /* wrap the head */
    }
    --me->head;
    ++me->nUsed;
    if (me->nUsed == (uint8_t)1) {              /* is this the first event? */
        QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);    /* set the bit */
#ifdef QK_PREEMPTIVE
        sig = QK_schedPrio_();          /* reuse 'sig' to hold the priority */
        if (sig != (QSignal)0) {
            QK_sched_(sig);             /* check for synchronous preemption */
        }
#endif
    }
    QF_INT_ENABLE();
}
/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
void QActive_postISR(QActive * const me, QSignal sig, QParam par)
#else
void QActive_postISR(QActive * const me, QSignal sig)
#endif
{
#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_STAT_TYPE stat;
#endif
#endif
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];

            /* the queue must be able to accept the event (cannot overflow) */
    Q_ASSERT(me->nUsed < Q_ROM_BYTE(acb->end));

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_DISABLE(stat);
#else
    QF_INT_DISABLE();
#endif
#endif
                                /* insert event into the ring buffer (FIFO) */
    QF_ROM_QUEUE_AT_(acb, me->head).sig = sig;
#if (Q_PARAM_SIZE != 0)
    QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
    if (me->head == (uint8_t)0) {
        me->head = Q_ROM_BYTE(acb->end);                   /* wrap the head */
    }
    --me->head;
    ++me->nUsed;
    if (me->nUsed == (uint8_t)1) {              /* is this the first event? */
        QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);    /* set the bit */
    }

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_RESTORE(stat);
#else
    QF_INT_ENABLE();
#endif
#endif
}

/*--------------------------------------------------------------------------*/
#if (QF_TIMEEVT_CTR_SIZE != 0)

/*..........................................................................*/
void QF_tickISR(void) {
    uint8_t p = (uint8_t)QF_MAX_ACTIVE;
    do {
        QActive *a = QF_ROM_ACTIVE_GET_(p);
        if (a->tickCtr != (QTimeEvtCtr)0) {
            --a->tickCtr;
            if (a->tickCtr == (QTimeEvtCtr)0) {
#ifdef Q_TIMERSET
                QF_timerSet_ &= Q_ROM_BYTE(l_invPow2Lkup[p]);  /* clear bit */
#endif

#if (Q_PARAM_SIZE != 0)
                QActive_postISR(a, (QSignal)Q_TIMEOUT_SIG, (QParam)0);
#else
                QActive_postISR(a, (QSignal)Q_TIMEOUT_SIG);
#endif
            }
        }
        --p;
    } while (p != (uint8_t)0);
}
/*..........................................................................*/
void QActive_arm(QActive * const me, QTimeEvtCtr const ticks) {
    QF_INT_DISABLE();
    me->tickCtr = ticks;
#ifdef Q_TIMERSET
    QF_timerSet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);        /* set the bit */
#endif
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QActive_disarm(QActive * const me) {
    QF_INT_DISABLE();
    me->tickCtr = (QTimeEvtCtr)0;
#ifdef Q_TIMERSET
    QF_timerSet_ &= Q_ROM_BYTE(l_invPow2Lkup[me->prio]);       /* clear bit */
#endif
    QF_INT_ENABLE();
}
#endif                                    /* #if (QF_TIMEEVT_CTR_SIZE != 0) */

/*--------------------------------------------------------------------------*/
#ifndef QK_PREEMPTIVE

int16_t QF_run(void) {
    uint8_t p;
    QActive *a;
                         /* set priorities all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        Q_ASSERT(a != (QActive *)0);    /* QF_active[p] must be initialized */
        a->prio = p;               /* set the priority of the active object */
    }
         /* trigger initial transitions in all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
#ifndef QF_FSM_ACTIVE
        QHsm_init(&a->super);         /* take the initial transition in HSM */
#else
        QFsm_init(&a->super);         /* take the initial transition in FSM */
#endif
    }

    QF_onStartup();                              /* invoke startup callback */

    for (;;) {                      /* the event loop of the vanilla kernel */
        QF_INT_DISABLE();
        if (QF_readySet_ != (uint8_t)0) {
            QActiveCB const Q_ROM *acb;

            p = QF_LOG2(QF_readySet_);
            acb = &QF_active[p];
            a = QF_ROM_ACTIVE_GET_(p);
            Q_ASSERT(a->nUsed > (uint8_t)0);/*some events must be available */
            --a->nUsed;
            if (a->nUsed == (uint8_t)0) {          /* queue becoming empty? */
                QF_readySet_ &= Q_ROM_BYTE(l_invPow2Lkup[p]);  /* clear bit */
            }
            Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
            if (a->tail == (uint8_t)0) {                    /* wrap around? */
                a->tail = Q_ROM_BYTE(acb->end);
            }
            --a->tail;
            QF_INT_ENABLE();

#ifndef QF_FSM_ACTIVE
            QHsm_dispatch(&a->super);                    /* dispatch to HSM */
#else
            QFsm_dispatch(&a->super);                    /* dispatch to FSM */
#endif
        }
        else {
            QF_onIdle();                                      /* see NOTE01 */
        }
    }
    return (int16_t)0; /* this unreachable return is to make compiler happy */
}

#endif                                             /* #ifndef QK_PREEMPTIVE */

/*****************************************************************************
* NOTE01:
* QF_onIdle() must be called with interrupts disabled, because the
* determination of the idle condition (no events in the queues) can be
* changed by any interrupt. The QF_onIdle() MUST enable interrupts internally,
* ideally atomically with putting the CPU into a low-power mode.
*/

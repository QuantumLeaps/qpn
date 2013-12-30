/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 03, 2013
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

Q_DEFINE_THIS_MODULE("qfn")

/**
* \file
* \ingroup qfn
* QF-nano implementation.
*/

/* Global-scope objects ----------------------------------------------------*/
uint8_t volatile QF_readySet_;                      /* ready-set of QF-nano */

#ifdef Q_TIMERSET
uint8_t volatile QF_timerSetX_[QF_MAX_TICK_RATE];   /* timer-set of QF-nano */
#endif

#ifndef QF_LOG2
uint8_t const Q_ROM QF_log2Lkup[16] = {
    (uint8_t)0, (uint8_t)1, (uint8_t)2, (uint8_t)2,
    (uint8_t)3, (uint8_t)3, (uint8_t)3, (uint8_t)3,
    (uint8_t)4, (uint8_t)4, (uint8_t)4, (uint8_t)4,
    (uint8_t)4, (uint8_t)4, (uint8_t)4, (uint8_t)4
};
#endif                                                           /* QF_LOG2 */


/* local objects -----------------------------------------------------------*/
static uint8_t const Q_ROM l_pow2Lkup[] = {
    (uint8_t)0x00,
    (uint8_t)0x01, (uint8_t)0x02, (uint8_t)0x04, (uint8_t)0x08,
    (uint8_t)0x10, (uint8_t)0x20, (uint8_t)0x40, (uint8_t)0x80
};

#if (defined Q_TIMERSET) || (!(defined QK_PREEMPTIVE))
static uint8_t const Q_ROM l_invPow2Lkup[] = {
    (uint8_t)0xFF,
    (uint8_t)0xFE, (uint8_t)0xFD, (uint8_t)0xFB, (uint8_t)0xF7,
    (uint8_t)0xEF, (uint8_t)0xDF, (uint8_t)0xBF, (uint8_t)0x7F
};
#endif                  /* (defined Q_TIMERSET) && !(defined QK_PREEMPTIVE) */

/*..........................................................................*/
#ifndef Q_NHSM
void QActive_ctor(QActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = {              /* QActive virtual table */
        { &QHsm_init_,
          &QHsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };
    QHsm_ctor(&me->super, initial);
    me->super.vptr = &vtbl.super; /* hook the vptr to QActive virtual table */
}
#endif                                                            /* Q_NHSM */

/*..........................................................................*/
#ifndef Q_NMSM

void QMActive_ctor(QMActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = {             /* QMActive virtual table */
        { &QMsm_init_,
          &QMsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };

    QMsm_ctor(&me->super, initial);/*call instead of QActive_ctor(), NOTE01 */
    me->super.vptr = &vtbl.super;/* hook the vptr to QMActive virtual table */
}

#endif                                                            /* Q_NMSM */

/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
uint8_t QActive_postX_(QActive * const me, uint8_t margin, enum_t const sig,
                       QParam const par)
#else
uint8_t QActive_postX_(QActive * const me, uint8_t margin, enum_t const sig)
#endif
{
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint8_t end = Q_ROM_BYTE(acb->end);

    QF_INT_DISABLE();

    if ((end - me->nUsed) > margin) {                  /* margin available? */
                                /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint8_t)0) {
            me->head = end;                                /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        if (me->nUsed == (uint8_t)1) {          /* is this the first event? */
#ifdef QK_PREEMPTIVE
            uint8_t p;
            QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);/* set the bit */
            p = QK_schedPrio_();                   /* find the new priority */
            if (p != (uint8_t)0) {
                QK_sched_(p);           /* check for synchronous preemption */
            }
#else
            QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);/* set the bit */
#endif
        }
        margin = (uint8_t)1;                          /* posting successful */
    }
    else {
        Q_ASSERT(margin != (uint8_t)0);      /* can tolerate dropping evts? */
        margin = (uint8_t)0;                              /* posting failed */
    }
    QF_INT_ENABLE();

    return margin;
}
/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
uint8_t QActive_postXISR_(QActive * const me, uint8_t margin,
                          enum_t const sig, QParam const par)
#else
uint8_t QActive_postXISR_(QActive * const me, uint8_t margin,
                          enum_t const sig)
#endif
{
#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_STAT_TYPE stat;
#endif
#endif
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint8_t end = Q_ROM_BYTE(acb->end);

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_DISABLE(stat);
#else
    QF_INT_DISABLE();
#endif
#endif

    if ((end - me->nUsed) > margin) {                  /* margin available? */
                                /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint8_t)0) {
            me->head = end;                                /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        if (me->nUsed == (uint8_t)1) {          /* is this the first event? */
            QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);/* set the bit */
        }
        margin = (uint8_t)1;                          /* posting successful */
    }
    else {
        Q_ASSERT(margin != (uint8_t)0);      /* can tolerate dropping evts? */
        margin = (uint8_t)0;                              /* posting failed */
    }

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_RESTORE(stat);
#else
    QF_INT_ENABLE();
#endif
#endif

    return margin;
}

/*--------------------------------------------------------------------------*/
#if (QF_TIMEEVT_CTR_SIZE != 0)

/*..........................................................................*/
void QF_tickXISR(uint8_t const tickRate) {
    uint8_t p = (uint8_t)QF_MAX_ACTIVE;
    do {
        QActive *a = QF_ROM_ACTIVE_GET_(p);
        if (a->tickCtr[tickRate] != (QTimeEvtCtr)0) {
            --a->tickCtr[tickRate];
            if (a->tickCtr[tickRate] == (QTimeEvtCtr)0) {
#ifdef Q_TIMERSET
                QF_timerSetX_[tickRate] &= Q_ROM_BYTE(l_invPow2Lkup[p]);
#endif

#if (Q_PARAM_SIZE != 0)
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG + (enum_t)tickRate,
                                 (QParam)0);
#else
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG + (enum_t)tickRate);
#endif
            }
        }
        --p;
    } while (p != (uint8_t)0);
}
/*..........................................................................*/
void QActive_armX(QActive * const me, uint8_t const tickRate,
                  QTimeEvtCtr const ticks)
{
    QF_INT_DISABLE();
    me->tickCtr[tickRate] = ticks;
#ifdef Q_TIMERSET
    QF_timerSetX_[tickRate] |= Q_ROM_BYTE(l_pow2Lkup[me->prio]); /* set bit */
#endif
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QActive_disarmX(QActive * const me, uint8_t const tickRate) {
    QF_INT_DISABLE();
    me->tickCtr[tickRate] = (QTimeEvtCtr)0;
#ifdef Q_TIMERSET
    QF_timerSetX_[tickRate] &= Q_ROM_BYTE(l_invPow2Lkup[me->prio]);/*clr bit*/
#endif
    QF_INT_ENABLE();
}
#endif                                    /* #if (QF_TIMEEVT_CTR_SIZE != 0) */

/*--------------------------------------------------------------------------*/
#ifndef QK_PREEMPTIVE

int_t QF_run(void) {
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
        QMSM_INIT(&a->super);      /* take the initial transition in the SM */
    }

    QF_onStartup();                              /* invoke startup callback */

    for (;;) {                      /* the event loop of the vanilla kernel */
        QF_INT_DISABLE();
        if (QF_readySet_ != (uint8_t)0) {
            QActiveCB const Q_ROM *acb;

#ifdef QF_LOG2
            p = QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
            if ((QF_readySet_ & (uint8_t)0xF0) != (uint8_t)0) {/*hi nibble? */
                p = (uint8_t)(Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                              + (uint8_t)4);
            }
            else                       /* hi nibble of QF_readySet_ is zero */
#endif
            {
                p = Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }
#endif
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

            QMSM_DISPATCH(&a->super);                 /* dispatch to the SM */
        }
        else {
            QF_onIdle();                                      /* see NOTE01 */
        }
    }
#ifdef __GNUC__                                            /* GNU compiler? */
    return (int_t)0;
#endif
}

#endif                                             /* #ifndef QK_PREEMPTIVE */

/*****************************************************************************
* NOTE01:
* QF_onIdle() must be called with interrupts disabled, because the
* determination of the idle condition (no events in the queues) can be
* changed by any interrupt. The QF_onIdle() MUST enable interrupts internally,
* ideally atomically with putting the CPU into a low-power mode.
*/


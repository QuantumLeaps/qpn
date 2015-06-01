/**
* @file
* @brief QF-nano implementation.
* @ingroup qfn
* @cond
******************************************************************************
* Last updated for version 5.4.0
* Last updated on  2015-05-24
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
* @endcond
*/
#include "qpn_conf.h" /* QP-nano configuration file (from the application) */
#include "qfn_port.h" /* QF-nano port from the port directory */
#include "qassert.h"  /* embedded systems-friendly assertions */

Q_DEFINE_THIS_MODULE("qfn")

/* Global-scope objects *****************************************************/
/**
* @description
* The QV-nano ready set keeps track of active objects that are ready to run.
* The ready set represents each active object as a bit, with the bits
* assigned according to priorities of the active objects. The bit is set
* if the corresponding active object is ready to run (i.e., has one or
* more events in its event queue) and zero if the event queue is empty.
* The QF-nano ready set is one byte-wide, which corresponds to 8 active
* objects maximum.
*/
uint_fast8_t volatile QF_readySet_;

#ifdef QF_TIMEEVT_USAGE
/**
* @description
* The QF-nano timer set keeps track of the armed time events. The timer set
* represents the timeout down-counter of each active object as a bit, with
* the bits assigned according to priorities of the active objects. The bit
* is set if the corresponding timeout down-counter is not zero (i.e., is
* counting down) and zero if the down-counter is zero. The QF-nano time event
* set is one byte-wide, which corresponds to 8 active objects maximum.@n
* @n
* The main use of the QF_timerSetX_ is to quickly determine that all time
* events are disarmed by testing (QF_timerSetX_[tickRate] == 0).
* If so, the CPU can go to longer sleep mode, in which the system clock
* tick ISR is turned off.
*
* @note The test (QF_timerSet_[tickRate] == 0) must be always performed
* inside a CRITICAL SECTION.
*/
uint_fast8_t volatile QF_timerSetX_[QF_MAX_TICK_RATE];
#endif

#ifndef QF_LOG2
uint8_t const Q_ROM QF_log2Lkup[16] = {
    (uint8_t)0, (uint8_t)1, (uint8_t)2, (uint8_t)2,
    (uint8_t)3, (uint8_t)3, (uint8_t)3, (uint8_t)3,
    (uint8_t)4, (uint8_t)4, (uint8_t)4, (uint8_t)4,
    (uint8_t)4, (uint8_t)4, (uint8_t)4, (uint8_t)4
};
#endif /* QF_LOG2 */

uint8_t const Q_ROM QF_invPow2Lkup[9] = {
    (uint8_t)0xFF,
    (uint8_t)0xFE, (uint8_t)0xFD, (uint8_t)0xFB, (uint8_t)0xF7,
    (uint8_t)0xEF, (uint8_t)0xDF, (uint8_t)0xBF, (uint8_t)0x7F
};

/* local objects ************************************************************/
static uint8_t const Q_ROM l_pow2Lkup[] = {
    (uint8_t)0x00,
    (uint8_t)0x01, (uint8_t)0x02, (uint8_t)0x04, (uint8_t)0x08,
    (uint8_t)0x10, (uint8_t)0x20, (uint8_t)0x40, (uint8_t)0x80
};

/****************************************************************************/
/****************************************************************************/
#ifndef Q_NMSM

/****************************************************************************/
/**
* @description
* Performs the first step of active object initialization by assigning
* the virtual pointer and calling the superclass constructor.
*
* @param[in,out] me       pointer (see @ref oop)
* @param[in]     initial  pointer to the event to be dispatched to the MSM
*
* @note  Must be called only ONCE before QMSM_INIT().
*
* @sa QMsm_ctor()
*
* @usage
* @include qfn_qmactive.c
*/
void QMActive_ctor(QMActive * const me, QStateHandler initial) {
    static QMActiveVtbl const vtbl = { /* QMActive virtual table */
        { &QMsm_init_,
          &QMsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };

    QMsm_ctor(&me->super, initial);
    me->super.vptr = &vtbl.super;/* hook the vptr to QMActive virtual table */
}

#endif /* Q_NMSM */

/****************************************************************************/
#ifndef Q_NHSM

void QActive_ctor(QActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = { /* QActive virtual table */
        { &QHsm_init_,
          &QHsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };

    /**
    * @note QActive inherits QMActive, so by the @ref oop convention
    * it should call the constructor of the superclass, i.e., QMActive_ctor().
    * However, this would pull in the QMActiveVtbl, which in turn will pull
    * in the code for QMsm_init_() and QMsm_dispatch_() implemetations,
    * which is expensive. To avoid this code size penalty, in case ::QMsm is
    * not used in a given project, the call to QHsm_ctor() avoids pulling
    * in the code for QMsm.
    */
    QHsm_ctor(&me->super, initial);
    me->super.vptr = &vtbl.super; /* hook the vptr to QActive virtual table */
}

#endif /* Q_NHSM */


/****************************************************************************/
/**
* @description
* Direct event posting is the simplest asynchronous communication method
* available in QF-nano.
*
* @attention
* This function should be called only via the macro QACTIVE_POST()
* or QACTIVE_POST_X(). This function should be only used in the
* __task__ context.
*
* @param[in,out] me     pointer (see @ref oop)
* @param[in]     margin number of required free slots in the queue
*                       after posting the event.
* @param[in]     sig    signal of the event to be posted
* @param[in]     par    parameter of the event to be posted
*
* @note The zero value of the 'margin' parameter is special and denotes
* situation when the post() operation is assumed to succeed (event delivery
* guarantee). An assertion fires, when the event cannot be delivered in
* this case.
*
* @usage
* @include qfn_postx.c
*/
#if (Q_PARAM_SIZE != 0)
bool QActive_postX_(QMActive * const me, uint_fast8_t margin,
                    enum_t const sig, QParam const par)
#else
bool QActive_postX_(QMActive * const me, uint_fast8_t margin,
                    enum_t const sig)
#endif
{
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint_fast8_t qlen = (uint_fast8_t)Q_ROM_BYTE(acb->qlen);

    QF_INT_DISABLE();

    /* margin available? */
    if ((qlen - me->nUsed) > margin) {

        /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = qlen; /* wrap the head */
        }
        --me->head;
        ++me->nUsed;

        /* is this the first event? */
        if (me->nUsed == (uint_fast8_t)1) {
#ifdef QK_PREEMPTIVE
            uint_fast8_t p;

            /* set the corresponding bit in the ready set */
            QF_readySet_ |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);

            p = QK_schedPrio_();  /* find the new priority */
            if (p != (uint_fast8_t)0) {
                QK_sched_(p); /* check for synchronous preemption */
            }
#else
            /* set the bit */
            QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);
#endif
        }
        margin = (uint_fast8_t)true; /* posting successful */
    }
    else {
        /* can tolerate dropping evts? */
        Q_ASSERT_ID(310, margin != (uint_fast8_t)0);

        margin = (uint_fast8_t)false; /* posting failed */
    }
    QF_INT_ENABLE();

    return (bool)margin;
}

/****************************************************************************/
/**
* @description
* Direct event posting is the simplest asynchronous communication method
* available in QF-nano.
*
* @attention
* This function should be called only via the macro QACTIVE_POST_ISR()
* or QACTIVE_POST_X_ISR(). This function should be only used in the
* __ISR__ context.
*
* @param[in,out] me     pointer (see @ref oop)
* @param[in]     margin number of required free slots in the queue
*                       after posting the event.
* @param[in]     sig    signal of the event to be posted
* @param[in]     par    parameter of the event to be posted
*
* @note The zero value of the 'margin' parameter is special and denotes
* situation when the post() operation is assumed to succeed (event delivery
* guarantee). An assertion fires, when the event cannot be delivered in
* this case.
*
* @usage
* @include qfn_postx.c
*/
#if (Q_PARAM_SIZE != 0)
bool QActive_postXISR_(QMActive * const me, uint_fast8_t margin,
                       enum_t const sig, QParam const par)
#else
bool QActive_postXISR_(QMActive * const me, uint_fast8_t margin,
                       enum_t const sig)
#endif
{
#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_STAT_TYPE stat;
#endif
#endif
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint_fast8_t qlen = (uint_fast8_t)Q_ROM_BYTE(acb->qlen);

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_DISABLE(stat);
#else
    QF_INT_DISABLE();
#endif
#endif

    /* margin available? */
    if ((qlen - me->nUsed) > margin) {

        /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = qlen; /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        /* is this the first event? */
        if (me->nUsed == (uint_fast8_t)1) {
            /* set the bit */
            QF_readySet_ |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
        }
        margin = (uint_fast8_t)true; /* posting successful */
    }
    else {
        /* can tolerate dropping evts? */
        Q_ASSERT_ID(410, margin != (uint_fast8_t)0);
        margin = (uint_fast8_t)false; /* posting failed */
    }

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_RESTORE(stat);
#else
    QF_INT_ENABLE();
#endif
#endif

    return (bool)margin;
}


/****************************************************************************/
/****************************************************************************/
#if (QF_TIMEEVT_CTR_SIZE != 0)

/****************************************************************************/
/**
* @description
* This function must be called periodically from a time-tick ISR or from
* an ISR so that QF-nano can manage the timeout events assigned to the given
* system clock tick rate.
*
* @param[in]  tickRate  system clock tick rate serviced in this call.
*
* @note Each system tick rate posts timeout events with a different signal
* as follows:@n
* tickRate==0  Q_TIMEOUT_SIG@n
* tickRate==1  Q_TIMEOUT1_SIG@n
* tickRate==2  Q_TIMEOUT2_SIG@n
* tickRate==3  Q_TIMEOUT3_SIG
*
* @note The calls to QF_tickXISR() with different tick rate parameter can
* preempt each other. For example, higher clock tick rates might be serviced
* from interrupts that can preempt lower-priority interrupts.
*/
void QF_tickXISR(uint_fast8_t const tickRate) {
    uint_fast8_t p = (uint_fast8_t)QF_MAX_ACTIVE;
    do {
        QMActive *a = QF_ROM_ACTIVE_GET_(p);
        QTimer *t = &a->tickCtr[tickRate];

        if (t->nTicks != (QTimeEvtCtr)0) {
            --t->nTicks;
            if (t->nTicks == (QTimeEvtCtr)0) {

#ifdef QF_TIMEEVT_PERIODIC
                if (t->interval != (QTimeEvtCtr)0) {
                    t->nTicks = t->interval; /* re-arm the periodic timer */
                }
#endif /* QF_TIMEEVT_PERIODIC */

#ifdef QF_TIMEEVT_USAGE
                QF_timerSetX_[tickRate] &=
                    (uint_fast8_t)Q_ROM_BYTE(QF_invPow2Lkup[p]);
#endif /* QF_TIMEEVT_USAGE */

#if (Q_PARAM_SIZE != 0)
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG + (enum_t)tickRate,
                                 (QParam)0);
#else
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG + (enum_t)tickRate);
#endif /* (Q_PARAM_SIZE != 0) */
            }
        }
        --p;
    } while (p != (uint_fast8_t)0);
}

/****************************************************************************/
/**
* @description
* Arms a time event to fire in a specified number of clock ticks at the
* specified tick rate. The timeout signal gets directly posted (using the
* FIFO policy) into the event queue of the active object calling this
* function.
*
* @param[in,out] me       pointer (see @ref oop)
* @param[in]     tickRate tick rate .
* @param[in]     nTicks   number of clock ticks (at the associated rate)
*                         to rearm the time event with.
*
* @note Each system tick rate posts timeout events with a different signal
* as follows:@n
* tickRate==0  Q_TIMEOUT_SIG@n
* tickRate==1  Q_TIMEOUT1_SIG@n
* tickRate==2  Q_TIMEOUT2_SIG@n
* tickRate==3  Q_TIMEOUT3_SIG
*
* @note After posting, a one-shot time event gets automatically disarmed.
*
* @note A time event can be disarmed at any time by calling the
* QActive_disarmX() function.
*
* @usage
* The following example shows how to arm a time event from a state
* machine of an active object:
* @include qfn_armx.c
*/
#ifdef QF_TIMEEVT_PERIODIC
void QActive_armX(QMActive * const me, uint_fast8_t const tickRate,
                  QTimeEvtCtr const nTicks, QTimeEvtCtr const interval)
#else
void QActive_armX(QActive * const me, uint_fast8_t const tickRate,
                  QTimeEvtCtr const nTicks)
#endif
{
    QF_INT_DISABLE();
    me->tickCtr[tickRate].nTicks = nTicks;
#ifdef QF_TIMEEVT_PERIODIC
    me->tickCtr[tickRate].interval = interval;
#endif /* QF_TIMEEVT_PERIODIC */

#ifdef QF_TIMEEVT_USAGE
    /* set a bit in QF_timerSetX_[] to rememer that the timer is running */
    QF_timerSetX_[tickRate] |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
#endif
    QF_INT_ENABLE();
}

/****************************************************************************/
/**
* @description
* The time event of the active object gets disarmed (stopped).
*
* @param[in,out] me       pointer (see @ref oop)
* @param[in]     tickRate tick rate
*
* @note You should __not__ assume that the timeout event will not
* arrive after you disarm the time event. The timeout event could be
* already in the event queue.
*/
void QActive_disarmX(QMActive * const me, uint_fast8_t const tickRate) {
    QF_INT_DISABLE();
    me->tickCtr[tickRate].nTicks = (QTimeEvtCtr)0;
#ifdef QF_TIMEEVT_PERIODIC
    me->tickCtr[tickRate].interval = (QTimeEvtCtr)0;
#endif /* QF_TIMEEVT_PERIODIC */

#ifdef QF_TIMEEVT_USAGE
    /* clear a bit in QF_timerSetX_[] to rememer that timer is not running */
    QF_timerSetX_[tickRate] &=
        (uint_fast8_t)Q_ROM_BYTE(QF_invPow2Lkup[me->prio]);
#endif
    QF_INT_ENABLE();
}
#endif /* #if (QF_TIMEEVT_CTR_SIZE != 0) */



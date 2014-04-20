/**
* \file
* \brief QF-nano implementation.
* \ingroup qfn
* \cond
******************************************************************************
* Product: QF-nano
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

Q_DEFINE_THIS_MODULE("qfn")

/* Global-scope objects *****************************************************/
/**
* \description
* The QF-nano ready set keeps track of active objects that are ready to run.
* The ready set represents each active object as a bit, with the bits
* assigned according to priorities of the active objects. The bit is set
* if the corresponding active object is ready to run (i.e., has one or
* more events in its event queue) and zero if the event queue is empty.
* The QF-nano ready set is one byte-wide, which corresponds to 8 active
* objects maximum.
*/
uint_fast8_t volatile QF_readySet_;

#ifdef Q_TIMERSET
/**
* \description
* The QF-nano timer set keeps track of the armed time event set. The
* timer set represents the timeout down-counter of each active object as
* a bit, with the bits assigned according to priorities of the active
* objects. The bit is set if the corresponding timeout down-counter is
* not zero (i.e., is counting down) and zero if the down-counter is zero.
* The QF-nano time event set is one byte-wide, which corresponds to 8
* active objects maximum.\n
* \n
* The main use of the QF_timerSetX_ is to quickly determine that all time
* events are disarmed by testing (QF_timerSetX_[tickRate] == 0).
* If so, the CPU can go to longer sleep mode, in which the system clock
* tick ISR is turned off.
*
* \note The test (QF_timerSet_[tickRate] == 0) must be always performed
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


/* local objects ************************************************************/
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
#endif /* (defined Q_TIMERSET) && !(defined QK_PREEMPTIVE) */


/****************************************************************************/
/****************************************************************************/
#ifndef Q_NHSM
void QActive_ctor(QActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = { /* QActive virtual table */
        { &QHsm_init_,
          &QHsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };
    QHsm_ctor(&me->super, initial);
    me->super.vptr = &vtbl.super; /* hook the vptr to QActive virtual table */
}
#endif /* Q_NHSM */


/****************************************************************************/
/****************************************************************************/
#ifndef Q_NMSM

/****************************************************************************/
/**
* \description
* Performs the first step of active object initialization by assigning
* the virtual pointer and calling the superclass constructor.
*
* \arguments
* \arg[in,out] \c me       pointer (see \ref derivation)
* \arg[in]     \c initial  pointer to the event to be dispatched to the MSM
*
* \note  Must be called only ONCE before QMSM_INIT().
*
* \sa QHsm_ctor() and QFsm_ctor()
*/
void QMActive_ctor(QMActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = { /* QMActive virtual table */
        { &QMsm_init_,
          &QMsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };

    /**
    * \note QMActive inherits QActive, so by the \ref derivation convention
    * it should call the constructor of the superclass, i.e., QActive_ctor().
    * However, this would pull in the QActiveVtbl, which in turn will pull
    * in the code for QHsm_init_() and QHsm_dispatch_() implemetations,
    * which is expensive. To avoid this code size penalty, in case QHsm is
    * not used in a given project, the call to QMsm_ctor() avoids pulling
    * in the code for QHsm.
    */
    QMsm_ctor(&me->super, initial);

    me->super.vptr = &vtbl.super;/* hook the vptr to QMActive virtual table */
}

#endif /* Q_NMSM */

/****************************************************************************/
/**
* \description
* Direct event posting is the simplest asynchronous communication method
* available in QF-nano.
*
* \attention
* This function should be called only via the macro QACTIVE_POST()
* or QACTIVE_POST_X(). This function should be only used in the
* __task__ context.
*
* \arguments
* \arg[in,out] \c me     pointer (see \ref derivation)
* \arg[in]     \c margin number of required free slots in the queue
*                        after posting the event.
* \arg[in]     \c sig    signal of the event to be posted
* \arg[in]     \c par    parameter of the event to be posted
*
* \note The zero value of the 'margin' argument is special and denotes
* situation when the post() operation is assumed to succeed (event delivery
* guarantee). An assertion fires, when the event cannot be delivered in
* this case.
*
* \usage
* \include qfn_post.c
*/
#if (Q_PARAM_SIZE != 0)
bool QActive_postX_(QActive * const me, uint_fast8_t margin, enum_t const sig,
                    QParam const par)
#else
bool QActive_postX_(QActive * const me, uint_fast8_t margin, enum_t const sig)
#endif
{
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint_fast8_t end = (uint_fast8_t)Q_ROM_BYTE(acb->end);

    QF_INT_DISABLE();

    /* margin available? */
    if ((end - me->nUsed) > margin) {

        /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = end; /* wrap the head */
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
* \description
* Direct event posting is the simplest asynchronous communication method
* available in QF-nano.
*
* \attention
* This function should be called only via the macro QACTIVE_POST_ISR()
* or QACTIVE_POST_X_ISR(). This function should be only used in the
* __ISR__ context.
*
* \arguments
* \arg[in,out] \c me     pointer (see \ref derivation)
* \arg[in]     \c margin number of required free slots in the queue
*                        after posting the event.
* \arg[in]     \c sig    signal of the event to be posted
* \arg[in]     \c par    parameter of the event to be posted
*
* \note The zero value of the 'margin' argument is special and denotes
* situation when the post() operation is assumed to succeed (event delivery
* guarantee). An assertion fires, when the event cannot be delivered in
* this case.
*
* \usage
* \include qfn_post.c
*/
#if (Q_PARAM_SIZE != 0)
bool QActive_postXISR_(QActive * const me, uint_fast8_t margin,
                       enum_t const sig, QParam const par)
#else
bool QActive_postXISR_(QActive * const me, uint_fast8_t margin,
                       enum_t const sig)
#endif
{
#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_STAT_TYPE stat;
#endif
#endif
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint_fast8_t end = (uint_fast8_t)Q_ROM_BYTE(acb->end);

#ifdef QF_ISR_NEST
#ifdef QF_ISR_STAT_TYPE
    QF_ISR_DISABLE(stat);
#else
    QF_INT_DISABLE();
#endif
#endif

    /* margin available? */
    if ((end - me->nUsed) > margin) {

        /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = end; /* wrap the head */
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
* \description
* This function must be called periodically from a time-tick ISR or from
* an ISR so that QF-nano can manage the timeout events assigned to the given
* system clock tick rate.
*
* \arguments
* \arg[in]  \c tickRate  system clock tick rate serviced in this call.
*
* \note Each system tick rate posts timeout events with a different signal
* as follows:\n
* tickRate==0  Q_TIMEOUT_SIG\n
* tickRate==1  Q_TIMEOUT1_SIG\n
* tickRate==2  Q_TIMEOUT2_SIG\n
* tickRate==3  Q_TIMEOUT3_SIG
*
* \note The calls to QF_tickXISR() with different tick rate argument can
* preempt each other. For example, higher clock tick rates might be serviced
* from interrupts that can preempt lower-priority interrupts.
*/
void QF_tickXISR(uint_fast8_t const tickRate) {
    uint_fast8_t p = (uint_fast8_t)QF_MAX_ACTIVE;
    do {
        QActive *a = QF_ROM_ACTIVE_GET_(p);
        if (a->tickCtr[tickRate] != (QTimeEvtCtr)0) {
            --a->tickCtr[tickRate];
            if (a->tickCtr[tickRate] == (QTimeEvtCtr)0) {
#ifdef Q_TIMERSET
                QF_timerSetX_[tickRate] &=
                    (uint_fast8_t)Q_ROM_BYTE(l_invPow2Lkup[p]);
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
    } while (p != (uint_fast8_t)0);
}

/****************************************************************************/
/**
* \description
* Arms a time event to fire in a specified number of clock ticks at the
* specified tick rate. The timeout signal gets directly posted (using the
* FIFO policy) into the event queue of the active object calling this
* function.
*
* \arguments
* \arg[in,out] \c me     pointer (see \ref derivation)
* \arg[in]     \c tickRate tick rate .
* \arg[in]     \c nTicks number of clock ticks (at the associated rate)
*                        to rearm the time event with.
*
* \note Each system tick rate posts timeout events with a different signal
* as follows:\n
* tickRate==0  Q_TIMEOUT_SIG\n
* tickRate==1  Q_TIMEOUT1_SIG\n
* tickRate==2  Q_TIMEOUT2_SIG\n
* tickRate==3  Q_TIMEOUT3_SIG
*
* \note After posting, a one-shot time event gets automatically disarmed.
*
* \note A time event can be disarmed at any time by calling the
* QActive_disarmX() function.
*
* \usage
* The following example shows how to arm a time event from a state
* machine of an active object:
* \include qfn_arm.c
*/
void QActive_armX(QActive * const me, uint_fast8_t const tickRate,
                  QTimeEvtCtr const ticks)
{
    QF_INT_DISABLE();
    me->tickCtr[tickRate] = ticks;
#ifdef Q_TIMERSET
    /* set a bit in QF_timerSetX_[] to rememer that the timer is running */
    QF_timerSetX_[tickRate] |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
#endif
    QF_INT_ENABLE();
}

/****************************************************************************/
/**
* \description
* The time event of the active object gets disarmed (stopped).
*
* \arguments
* \arg[in,out] \c me     pointer (see \ref derivation)
* \arg[in]     \c tickRate tick rate .
*
* \note You should __not__ assume that the timeout event will not
* arrive after you disarm the time event. The timeout event could be
* already in the event queue.
*/
void QActive_disarmX(QActive * const me, uint_fast8_t const tickRate) {
    QF_INT_DISABLE();
    me->tickCtr[tickRate] = (QTimeEvtCtr)0;
#ifdef Q_TIMERSET
    /* clear a bit in QF_timerSetX_[] to rememer that timer is not running */
    QF_timerSetX_[tickRate] &=
        (uint_fast8_t)Q_ROM_BYTE(l_invPow2Lkup[me->prio]);
#endif
    QF_INT_ENABLE();
}
#endif /* #if (QF_TIMEEVT_CTR_SIZE != 0) */


/****************************************************************************/
/****************************************************************************/
#ifndef QK_PREEMPTIVE

/****************************************************************************/
/**
* \description
* QF_run() is typically called from your startup code after you initialize
* the QF and start at least one active object with QActive_start().
* This implementation of QF_run() is for the cooperative Vanilla kernel.
*
* \returns QF_run() typically does not return in embedded applications.
* However, when QP runs on top of an operating system,  QF_run() might
* return and in this case the return represents the error code (0 for
* success). Typically the value returned from QF_run() is subsequently
* passed on as return from main().
*/
int_t QF_run(void) {
    uint_fast8_t p;
    QActive *a;

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(810, a != (QActive *)0);

        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QMSM_INIT(&a->super); /* take the initial transition in the SM */
    }

    QF_onStartup(); /* invoke startup callback */

    /* the event loop of the vanilla kernel... */
    for (;;) {
        QF_INT_DISABLE();
        if (QF_readySet_ != (uint_fast8_t)0) {
            QActiveCB const Q_ROM *acb;

#ifdef QF_LOG2
            p = QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
            /* hi nibble non-zero? */
            if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
                p = (uint_fast8_t)(Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                              + (uint_fast8_t)4);
            }
            /* hi nibble of QF_readySet_ is zero */
            else
#endif
            {
                p = Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }
#endif
            acb = &QF_active[p];
            a = QF_ROM_ACTIVE_GET_(p);

            /* some unsuded events must be available */
            Q_ASSERT_ID(820, a->nUsed > (uint_fast8_t)0);

            --a->nUsed;
            /* queue becoming empty? */
            if (a->nUsed == (uint_fast8_t)0) {
                QF_readySet_ &= Q_ROM_BYTE(l_invPow2Lkup[p]); /* clear bit */
            }
            Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
            if (a->tail == (uint_fast8_t)0) { /* wrap around? */
                a->tail = Q_ROM_BYTE(acb->end);
            }
            --a->tail;
            QF_INT_ENABLE();

            QMSM_DISPATCH(&a->super); /* dispatch to the SM */
        }
        else {
            /* QF_onIdle() must be called with interrupts DISABLED because
            * the determination of the idle condition (no events in the
            * queues) can change at any time by an interrupt posting events
            * to a queue. QF_onIdle() MUST enable interrupts internally,
            * perhaps at the same time as putting the CPU into a power-saving
            * mode.
            */
            QF_onIdle();
        }
    }
#ifdef __GNUC__  /* GNU compiler? */
    return (int_t)0;
#endif
}

#endif /* #ifndef QK_PREEMPTIVE */

/*****************************************************************************
* Product: QF-nano emulation for Win32 with cooperative QV kernel
* Last Updated for Version: 5.6.4
* Date of the Last Update:  2016-05-04
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
* http://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/

#include "qpn.h" /* QP-nano */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>  /* Win32 API */

#ifdef QK_PREEMPTIVE
    #error "This QP-nano port does not support QK_PREEMPTIVE configuration"
#endif

Q_DEFINE_THIS_MODULE("qfn_win32")

/* Global objects ==========================================================*/
uint_fast8_t volatile QF_readySet_; /* ready-set of QF-nano */
uint_fast8_t QF_maxActive_; /* # active objects that QF-nano must manage */

#ifdef QF_TIMEEVT_USAGE
uint_fast8_t volatile QF_timerSetX_[QF_MAX_TICK_RATE]; /* timer-set */
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

/* Local objects ===========================================================*/
static uint8_t const Q_ROM l_pow2Lkup[] = {
    (uint8_t)0x00,
    (uint8_t)0x01, (uint8_t)0x02, (uint8_t)0x04, (uint8_t)0x08,
    (uint8_t)0x10, (uint8_t)0x20, (uint8_t)0x40, (uint8_t)0x80
};

static CRITICAL_SECTION l_win32CritSect; /* for QP-nano critical sections */
static HANDLE  l_win32Event;     /* Win32 event to signal events */
static DWORD   l_tickMsec = 10U; /* clock tick in msec (for Sleep()) */
static bool    l_isRunning;      /* flag indicating when QF is running */

/* "fudged" event queues for AOs, see NOTE2 */
#define QF_FUDGED_QUEUE_LEN  0xFFU
static QEvt l_fudgedQueue[8][QF_FUDGED_QUEUE_LEN];
#define QF_FUDGED_QUEUE_AT_(ao_, i_) (l_fudgedQueue[(ao_)->prio - 1U][(i_)])

static DWORD WINAPI ticker_thread(LPVOID arg);

/****************************************************************************/
/****************************************************************************/
#ifndef Q_NMSM

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
    QHsm_ctor(&me->super, initial);
    me->super.vptr = &vtbl.super; /* hook the vptr to QActive virtual table */
}

#endif /* Q_NHSM */


/****************************************************************************/
#if (Q_PARAM_SIZE != 0)
bool QActive_postX_(QMActive * const me, uint_fast8_t margin,
                    enum_t const sig, QParam const par)
#else
bool QActive_postX_(QMActive * const me, uint_fast8_t margin,
                    enum_t const sig)
#endif
{
    QF_INT_DISABLE();

    /* margin available? */
    if (((uint_fast8_t)QF_FUDGED_QUEUE_LEN - me->nUsed) > margin) {

        /* insert event into the ring buffer (FIFO) */
        QF_FUDGED_QUEUE_AT_(me, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_FUDGED_QUEUE_AT_(me, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = (uint_fast8_t)QF_FUDGED_QUEUE_LEN; /* wrap the head */
        }
        --me->head;
        ++me->nUsed;

        /* is this the first event? */
        if (me->nUsed == (uint_fast8_t)1) {
            /* set the corresponding bit in the ready set */
            QF_readySet_ |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
            SetEvent(l_win32Event);
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
#if (Q_PARAM_SIZE != 0)
bool QActive_postXISR_(QMActive * const me, uint_fast8_t margin,
                       enum_t const sig, QParam const par)
#else
bool QActive_postXISR_(QMActive * const me, uint_fast8_t margin,
                       enum_t const sig)
#endif
{
    /* margin available? */
    if (((uint_fast8_t)QF_FUDGED_QUEUE_LEN - me->nUsed) > margin) {

        /* insert event into the ring buffer (FIFO) */
        QF_FUDGED_QUEUE_AT_(me, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_FUDGED_QUEUE_AT_(me, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = (uint_fast8_t)QF_FUDGED_QUEUE_LEN; /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        /* is this the first event? */
        if (me->nUsed == (uint_fast8_t)1) {
            /* set the bit */
            QF_readySet_ |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
            SetEvent(l_win32Event);
        }
        margin = (uint_fast8_t)true; /* posting successful */
    }
    else {
        /* can tolerate dropping evts? */
        Q_ASSERT_ID(410, margin != (uint_fast8_t)0);
        margin = (uint_fast8_t)false; /* posting failed */
    }

    return (bool)margin;
}


/****************************************************************************/
/****************************************************************************/
#if (QF_TIMEEVT_CTR_SIZE != 0)

void QF_tickXISR(uint_fast8_t const tickRate) {
    uint_fast8_t p = QF_maxActive_;
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
#ifdef QF_TIMEEVT_PERIODIC
void QActive_armX(QMActive * const me, uint_fast8_t const tickRate,
                  QTimeEvtCtr const nTicks, QTimeEvtCtr const interval)
#else
void QActive_armX(QMActive * const me, uint_fast8_t const tickRate,
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

/* QF functions ============================================================*/
/****************************************************************************/
void QF_enterCriticalSection_(void) {
    EnterCriticalSection(&l_win32CritSect);
}
/****************************************************************************/
void QF_leaveCriticalSection_(void) {
    LeaveCriticalSection(&l_win32CritSect);
}
/****************************************************************************/
/**
* @description
* The function QF_init() initializes the number of active objects to be
* managed by the framework and clears the internal QF-nano variables as well
* as all registered active objects to zero, which is needed in case when
* the startup code does not clear the uninitialized data (in violation of
* the C Standard).
*
* @note
* The intended use of the function is to call as follows:
* QF_init(Q_DIM(QF_active));
*/
void QF_init(uint_fast8_t maxActive) {
    QMActive *a;
    uint_fast8_t p;
#if (defined(QF_TIMEEVT_USAGE) || (QF_TIMEEVT_CTR_SIZE != 0))
    uint_fast8_t n;
#endif /* QF_TIMEEVT_USAGE */

    /** @pre the number of active objects must be in range */
    Q_REQUIRE_ID(100, ((uint_fast8_t)1 < maxActive)
                      && (maxActive <= (uint_fast8_t)9));
    QF_maxActive_ = (uint_fast8_t)maxActive - (uint_fast8_t)1;

#ifdef QF_TIMEEVT_USAGE
    for (n = (uint_fast8_t)0; n < (uint_fast8_t)QF_MAX_TICK_RATE; ++n) {
        QF_timerSetX_[n] = (uint_fast8_t)0;
    }
#endif /* QF_TIMEEVT_USAGE */

    QF_readySet_ = (uint_fast8_t)0;

#ifdef QK_PREEMPTIVE
    QK_currPrio_ = (uint_fast8_t)8; /* QK-nano scheduler locked */

#ifdef QF_ISR_NEST
    QK_intNest_ = (uint_fast8_t)0;
#endif

#ifdef QK_MUTEX
    QK_lockPrio_ = (uint_fast8_t)0;
#endif

#endif /* #ifdef QK_PREEMPTIVE */

    /* clear all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(110, a != (QMActive *)0);

        a->head    = (uint_fast8_t)0;
        a->tail    = (uint_fast8_t)0;
        a->nUsed   = (uint_fast8_t)0;
#if (QF_TIMEEVT_CTR_SIZE != 0)
        for (n = (uint_fast8_t)0; n < (uint_fast8_t)QF_MAX_TICK_RATE; ++n) {
            a->tickCtr[n].nTicks   = (QTimeEvtCtr)0;
#ifdef QF_TIMEEVT_PERIODIC
            a->tickCtr[n].interval = (QTimeEvtCtr)0;
#endif /* def QF_TIMEEVT_PERIODIC */
        }
#endif /* (QF_TIMEEVT_CTR_SIZE != 0) */
    }
}
/****************************************************************************/
int_t QF_run(void) {
    uint_fast8_t p;
    QMActive *a;

    InitializeCriticalSection(&l_win32CritSect);
    l_win32Event = CreateEvent(NULL, FALSE, FALSE, NULL);

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(810, a != (QMActive *)0);

        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QMSM_INIT(&a->super); /* take the initial transition in the SM */
    }

    QF_onStartup(); /* invoke startup callback */

    l_isRunning = true;
    Q_ALLEGE_ID(810, CreateThread(NULL, 1024, &ticker_thread, 0, 0, NULL)
        != (HANDLE)0); /* ticker thread must be created */

    /* the event loop of the vanilla kernel... */
    while (l_isRunning) {
        QF_INT_DISABLE();
        if (QF_readySet_ != (uint_fast8_t)0) {
            QMActiveCB const Q_ROM *acb;

            /* hi nibble non-zero? */
            if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
                p = (uint_fast8_t)(
                      (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                      + (uint_fast8_t)4);
            }
            else { /* hi nibble of QF_readySet_ is zero */
                p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }

            acb = &QF_active[p];
            a = QF_ROM_ACTIVE_GET_(p);

            /* some unsuded events must be available */
            Q_ASSERT_ID(820, a->nUsed > (uint_fast8_t)0);

            --a->nUsed;
            /* queue becoming empty? */
            if (a->nUsed == (uint_fast8_t)0) {
                /* clear the bit corresponding to 'p' */
                QF_readySet_ &= (uint_fast8_t)Q_ROM_BYTE(QF_invPow2Lkup[p]);
            }
            Q_SIG(a) = QF_FUDGED_QUEUE_AT_(a, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_FUDGED_QUEUE_AT_(a, a->tail).par;
#endif
            if (a->tail == (uint_fast8_t)0) { /* wrap around? */
                a->tail = (uint_fast8_t)QF_FUDGED_QUEUE_LEN;
            }
            --a->tail;
            QF_INT_ENABLE();

            QMSM_DISPATCH(&a->super); /* dispatch to the SM */
        }
        else {
            QF_INT_ENABLE();

            /* yield the CPU until new event(s) arrive */
            WaitForSingleObject(l_win32Event, (DWORD)INFINITE);
        }
    }
    QF_onCleanup(); /* cleanup callback */
    //CloseHandle(l_win32Event);
    //DeleteCriticalSection(&l_win32CritSect);

    return (int_t)0; /* success */
}
/****************************************************************************/
void QF_stop(void) {
    l_isRunning = false;    /* cause exit from the event loop */
    SetEvent(l_win32Event); /* unblock the event-loop so it can terminate */
}
/****************************************************************************/
void QF_setTickRate(uint32_t ticksPerSec) {
    l_tickMsec = 1000UL / ticksPerSec;
}

/*..........................................................................*/
/* Win32 thread for the system time tick... */
static DWORD WINAPI ticker_thread(LPVOID par) {
    (void)par; /* unused parameter */
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    while (l_isRunning) {
        Sleep(l_tickMsec); /* wait for the tick interval */

        QF_INT_DISABLE();  /* make sure simulated ISR cannot be preempted */
        QF_onClockTickISR(); /* call back to the app, see NOTE1 */
        QF_INT_ENABLE();
    }
    return 0U; /* return success */
}

/*****************************************************************************
* NOTE1:
* The callback QF_onClockTickISR() is invoked with interupts disabled
* to emulate the ISR level. This means that only the ISR-level APIs are
* available inside the QF_onClockTickISR() callback.
*
*
* NOTE2:
* Windows is not a deterministic real-time system, which means that the
* system can occasionally and unexpectedly "choke and freeze" for a number
* of seconds. The designers of Windows have dealt with these sort of issues
* by massively oversizing the resources available to the applications. For
* example, the default Windows GUI message queues size is 10,000 entries,
* which can dynamically grow to an even larger number. Also the stacks of
* Win32 threads can dynamically grow to several megabytes.
*
* In contrast, the event queues, event pools, and stack size inside the
* real-time embedded (RTE) systems can be (and must be) much smaller,
* because you typically can put an upper bound on the real-time behavior
* and the resulting delays.
*
* To be able to run the unmodified applications designed originally for
* RTE systems on Windows, and to reduce the odds of resource shortages in
* this case, the event queues of all Active Objects are "fudged" to the
* maximum dynamic range of uint_fast8_t data type, which is 0xFF
* (see QF_FUDGED_QUEUE_LEN).
*/



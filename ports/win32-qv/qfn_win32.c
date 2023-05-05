/**
* @file
* @brief QF-nano port to Win32 API (single-threaded, like the QV kernel)
* @ingroup ports
* @cond
******************************************************************************
* Last updated for version 6.8.0
* Last updated on  2020-03-17
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2020 Quantum Leaps, LLC. All rights reserved.
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
* along with this program. If not, see <www.gnu.org/licenses>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
******************************************************************************
* @endcond
*/
#include "qpn.h" /* QP-nano */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>  /* Win32 API */

#include <conio.h>    /* console input/output */

#ifdef qkn_h
    #error "This QP-nano port does not support QK-nano configuration"
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
    0U, 1U, 2U, 2U, 3U, 3U, 3U, 3U,
    4U, 4U, 4U, 4U, 4U, 4U, 4U, 4U
};
#endif /* QF_LOG2 */

/* Local objects ===========================================================*/
static CRITICAL_SECTION l_win32CritSect; /* for QP-nano critical sections */
static HANDLE l_win32Event;    /* Win32 event to signal events */
static DWORD l_tickMsec = 10U; /* clock tick in msec (argument for Sleep()) */
static int_t l_tickPrio = 50;  /* default priority of the "ticker" thread */
static bool  l_isRunning;      /* flag indicating when QF is running */

/* "fudged" event queues for AOs, see NOTE1 */
#define QF_FUDGED_QUEUE_LEN  0xFFU
static QEvt l_fudgedQueue[8][QF_FUDGED_QUEUE_LEN];
#define QF_FUDGED_QUEUE_AT_(ao_, i_) (l_fudgedQueue[(ao_)->prio - 1U][(i_)])

static DWORD WINAPI ticker_thread(LPVOID arg);

/****************************************************************************/
void QActive_ctor(QActive * const me, QStateHandler initial) {
    static QActiveVtable const vtable = { /* QActive virtual table */
        { &QHsm_init_,
          &QHsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };
    QHsm_ctor(&me->super, initial);
    me->super.vptr = &vtable.super; /* hook the vptr to QActive vtable */
}

/****************************************************************************/
#if (Q_PARAM_SIZE != 0)
bool QActive_postX_(QActive * const me, uint_fast8_t margin,
                    enum_t const sig, QParam const par)
#else
bool QActive_postX_(QActive * const me, uint_fast8_t margin,
                    enum_t const sig)
#endif
{
    QF_INT_DISABLE();
    bool status;

    if (margin == QF_NO_MARGIN) {
        if (QF_FUDGED_QUEUE_LEN > me->nUsed) {
            status = true; /* can post */
        }
        else {
            status = false; /* cannot post */
            Q_ERROR_ID(310); /* must be able to post the event */
        }
    }
    else if ((QF_FUDGED_QUEUE_LEN - me->nUsed) > margin) {
        status = true; /* can post */
    }
    else {
        status = false; /* cannot post */
    }

    if (status) { /* can post the event? */
        /* insert event into the ring buffer (FIFO) */
        QF_FUDGED_QUEUE_AT_(me, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_FUDGED_QUEUE_AT_(me, me->head).par = par;
#endif
        if (me->head == 0U) {
            me->head = QF_FUDGED_QUEUE_LEN; /* wrap the head */
        }
        --me->head;
        ++me->nUsed;

        /* is this the first event? */
        if (me->nUsed == 1U) {
            /* set the corresponding bit in the ready set */
            QF_readySet_ |= (uint_fast8_t)(1U << (me->prio - 1U));
            SetEvent(l_win32Event);
        }
    }
    QF_INT_ENABLE();

    return status;
}

/****************************************************************************/
#if (Q_PARAM_SIZE != 0)
bool QActive_postXISR_(QActive * const me, uint_fast8_t margin,
                       enum_t const sig, QParam const par)
#else
bool QActive_postXISR_(QActive * const me, uint_fast8_t margin,
                       enum_t const sig)
#endif
{
    bool status;

    if (margin == QF_NO_MARGIN) {
        if (QF_FUDGED_QUEUE_LEN > me->nUsed) {
            status = true; /* can post */
        }
        else {
            status = false; /* cannot post */
            Q_ERROR_ID(310); /* must be able to post the event */
        }
    }
    else if ((QF_FUDGED_QUEUE_LEN - me->nUsed) > margin) {
        status = true; /* can post */
    }
    else {
        status = false; /* cannot post */
    }

    if (status) { /* can post the event? */
        /* insert event into the ring buffer (FIFO) */
        QF_FUDGED_QUEUE_AT_(me, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_FUDGED_QUEUE_AT_(me, me->head).par = par;
#endif
        if (me->head == 0U) {
            me->head = QF_FUDGED_QUEUE_LEN; /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        /* is this the first event? */
        if (me->nUsed == 1U) {
            /* set the bit */
            QF_readySet_ |= (uint_fast8_t)(1U << (me->prio - 1U));
            SetEvent(l_win32Event);
        }
    }

    return status;
}

/****************************************************************************/
#if (QF_TIMEEVT_CTR_SIZE != 0)

void QF_tickXISR(uint_fast8_t const tickRate) {
    uint_fast8_t p = QF_maxActive_;
    do {
        QActive *a = QF_ROM_ACTIVE_GET_(p);
        QTimer *t = &a->tickCtr[tickRate];

        if (t->nTicks != 0U) {
            --t->nTicks;
            if (t->nTicks == 0U) {

#ifdef QF_TIMEEVT_PERIODIC
                if (t->interval != 0U) {
                    t->nTicks = t->interval; /* re-arm the periodic timer */
                }
#endif /* QF_TIMEEVT_PERIODIC */

#ifdef QF_TIMEEVT_USAGE
                QF_timerSetX_[tickRate] &= (uint_fast8_t)(~(1U << (p - 1U)));
#endif /* QF_TIMEEVT_USAGE */

#if (Q_PARAM_SIZE != 0)
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG + (enum_t)tickRate,
                                 0U);
#else
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG + (enum_t)tickRate);
#endif /* (Q_PARAM_SIZE != 0) */
            }
        }
        --p;
    } while (p != 0U);
}

/****************************************************************************/
#ifdef QF_TIMEEVT_PERIODIC
void QActive_armX(QActive * const me, uint_fast8_t const tickRate,
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
    QF_timerSetX_[tickRate] |=  (uint_fast8_t)(1U << (me->prio - 1U));
#endif
    QF_INT_ENABLE();
}

/****************************************************************************/
void QActive_disarmX(QActive * const me, uint_fast8_t const tickRate) {
    QF_INT_DISABLE();
    me->tickCtr[tickRate].nTicks = 0U;
#ifdef QF_TIMEEVT_PERIODIC
    me->tickCtr[tickRate].interval = 0U;
#endif /* QF_TIMEEVT_PERIODIC */

#ifdef QF_TIMEEVT_USAGE
    /* clear a bit in QF_timerSetX_[] to rememer that timer is not running */
    QF_timerSetX_[tickRate] &= (uint_fast8_t)(~(1U << (me->prio - 1U)));
#endif
    QF_INT_ENABLE();
}
#endif /* #if (QF_TIMEEVT_CTR_SIZE != 0) */

/* QF functions ============================================================*/
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
    QActive *a;
    uint_fast8_t p;
#if (defined(QF_TIMEEVT_USAGE) || (QF_TIMEEVT_CTR_SIZE != 0))
    uint_fast8_t n;
#endif /* QF_TIMEEVT_USAGE */

    /** @pre the number of active objects must be in range */
    Q_REQUIRE_ID(100, (1U < maxActive) && (maxActive <= 9U));
    QF_maxActive_ = maxActive - 1U;

#ifdef QF_TIMEEVT_USAGE
    for (n = 0U; n < QF_MAX_TICK_RATE; ++n) {
        QF_timerSetX_[n] = 0U;
    }
#endif /* QF_TIMEEVT_USAGE */

    QF_readySet_ = 0U;

#ifdef QK_PREEMPTIVE
    QK_currPrio_ = 8U; /* QK-nano scheduler locked */

#ifdef QF_ISR_NEST
    QK_intNest_ = 0U;
#endif

#ifdef QK_SCHED_LOCK
    QK_lockPrio_ = 0U;
#endif

#endif /* #ifdef QK_PREEMPTIVE */

    /* clear all registered active objects... */
    for (p = 1U; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(110, a != (QActive *)0);

        a->head  = 0U;
        a->tail  = 0U;
        a->nUsed = 0U;
#if (QF_TIMEEVT_CTR_SIZE != 0)
        for (n = 0U; n < QF_MAX_TICK_RATE; ++n) {
            a->tickCtr[n].nTicks   = 0U;
#ifdef QF_TIMEEVT_PERIODIC
            a->tickCtr[n].interval = 0U;
#endif /* def QF_TIMEEVT_PERIODIC */
        }
#endif /* (QF_TIMEEVT_CTR_SIZE != 0) */
    }
}
/****************************************************************************/
int_t QF_run(void) {
    uint_fast8_t p;
    QActive *a;

    InitializeCriticalSection(&l_win32CritSect);
    l_win32Event = CreateEvent(NULL, FALSE, FALSE, NULL);

    /* set priorities all registered active objects... */
    for (p = 1U; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(810, a != (QActive *)0);

        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = 1U; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QHSM_INIT(&a->super); /* take the initial transition in the HSM */
    }

    QF_onStartup(); /* invoke startup callback */

    l_isRunning = true; /* QF-nano is running */

    /* system clock tick configured? */
    if (l_tickMsec != 0U) {
        /* create the ticker thread... */
        HANDLE ticker = CreateThread(NULL, 1024, &ticker_thread, 0, 0, NULL);
        Q_ASSERT_ID(810, ticker != (HANDLE)0); /* thread must be created */
    }

    /* the event loop of the QV-nano kernel... */
    QF_INT_DISABLE();
    while (l_isRunning) {
        if (QF_readySet_ != 0U) {

            /* hi nibble non-zero? */
            if ((QF_readySet_ & 0xF0U) != 0U) {
                p = (uint_fast8_t)(
                      Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4]) + 4U);
            }
            else { /* hi nibble of QF_readySet_ is zero */
                p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }

            a = QF_ROM_ACTIVE_GET_(p);

            /* some unsuded events must be available */
            Q_ASSERT_ID(820, a->nUsed > 0U);

            --a->nUsed;
            Q_SIG(a) = QF_FUDGED_QUEUE_AT_(a, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_FUDGED_QUEUE_AT_(a, a->tail).par;
#endif
            if (a->tail == 0U) { /* wrap around? */
                a->tail = QF_FUDGED_QUEUE_LEN;
            }
            --a->tail;
            QF_INT_ENABLE();

            QHSM_DISPATCH(&a->super); /* dispatch to the HSM (RTC step) */

            QF_INT_DISABLE();
            /* empty queue? */
            if (a->nUsed == 0U) {
                /* clear the bit corresponding to 'p' */
                QF_readySet_ &= (uint_fast8_t)(~(1U << (p - 1U)));
            }
        }
        else {
            QF_INT_ENABLE();

            /* yield the CPU until new event(s) arrive */
            WaitForSingleObject(l_win32Event, (DWORD)INFINITE);

            QF_INT_DISABLE();
        }
    }
    QF_INT_ENABLE();
    QF_onCleanup(); /* cleanup callback */
    /*CloseHandle(l_win32Event);*/
    /*DeleteCriticalSection(&l_win32CritSect);*/

    return 0; /* success */
}
/****************************************************************************/
void QF_stop(void) {
    l_isRunning = false;    /* cause exit from the event loop */
    SetEvent(l_win32Event); /* unblock the event-loop so it can terminate */
}
/****************************************************************************/
void QF_setTickRate(uint32_t ticksPerSec, int_t tickPrio) {
    if (ticksPerSec != 0U) {
        l_tickMsec = 1000UL / ticksPerSec;
    }
    else {
        l_tickMsec = 0U; /* means NO system clock tick */
    }
    l_tickPrio = tickPrio;
}

/*..........................................................................*/
void QF_consoleSetup(void) {
}
/*..........................................................................*/
void QF_consoleCleanup(void) {
}
/*..........................................................................*/
int QF_consoleGetKey(void) {
    if (_kbhit()) { /* any key pressed? */
        return _getch();
    }
    return 0;
}
/*..........................................................................*/
int QF_consoleWaitForKey(void) {
    return _getch();
}

/****************************************************************************/
static DWORD WINAPI ticker_thread(LPVOID arg) { /* for CreateThread() */
    int threadPrio = THREAD_PRIORITY_NORMAL;

    /* set the ticker thread priority according to selection made in
    * QF_setTickRate()
    */
    if (l_tickPrio < 33) {
        threadPrio = THREAD_PRIORITY_BELOW_NORMAL;
    }
    else if (l_tickPrio > 66) {
        threadPrio = THREAD_PRIORITY_ABOVE_NORMAL;
    }

    SetThreadPriority(GetCurrentThread(), threadPrio);

    (void)arg; /* unused parameter */

    while (l_isRunning) {
        Sleep(l_tickMsec); /* wait for the tick interval */

        QF_INT_DISABLE();  /* make sure simulated ISR cannot be preempted */
        QF_onClockTickISR(); /* call back to the app, see NOTE2 */
        QF_INT_ENABLE();
    }
    return 0U; /* return success */
}

/*****************************************************************************
* NOTE1:
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
*
* NOTE2:
* The callback QF_onClockTickISR() is invoked with interupts disabled
* to emulate the ISR level. This means that only the ISR-level APIs are
* available inside the QF_onClockTickISR() callback.
*/


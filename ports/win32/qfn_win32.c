/*****************************************************************************
* Product: QF-nano emulation for Win32
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
*****************************************************************************/

#include "qpn_port.h" /* QP-nano port */
#ifndef qassert_h
    #include "qassert.h" /* QP assertions */
#endif /* qassert_h */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>  /* Win32 API */

#ifdef QK_PREEMPTIVE
    #error "This QP-nano port does not support QK_PREEMPTIVE configuration"
#endif

Q_DEFINE_THIS_MODULE("qfn_win32")

/* Global objects ----------------------------------------------------------*/
uint_fast8_t volatile QF_readySet_; /* ready-set of QF-nano */

#ifdef Q_TIMERSET
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

/* local objects -----------------------------------------------------------*/
static CRITICAL_SECTION l_win32CritSect; /* Win32 critical section */
static HANDLE l_win32Event;   /* Win32 event to signal when AOs are ready */
static DWORD l_tickMsec = 10; /* clock tick in msec (argument for Sleep()) */
static uint_fast8_t volatile l_running;

static uint8_t const Q_ROM l_pow2Lkup[] = {
    (uint8_t)0x00,
    (uint8_t)0x01, (uint8_t)0x02, (uint8_t)0x04, (uint8_t)0x08,
    (uint8_t)0x10, (uint8_t)0x20, (uint8_t)0x40, (uint8_t)0x80
};
static uint8_t const Q_ROM l_invPow2Lkup[] = {
    (uint8_t)0xFF,
    (uint8_t)0xFE, (uint8_t)0xFD, (uint8_t)0xFB, (uint8_t)0xF7,
    (uint8_t)0xEF, (uint8_t)0xDF, (uint8_t)0xBF, (uint8_t)0x7F
};

static DWORD WINAPI tickerThread(LPVOID par); /* ticker thread routine */

/*..........................................................................*/
void QF_enterCriticalSection(void) {
    EnterCriticalSection(&l_win32CritSect);
}
/*..........................................................................*/
void QF_leaveCriticalSection(void) {
    LeaveCriticalSection(&l_win32CritSect);
}

/*..........................................................................*/
#ifndef Q_NHSM
void QActive_ctor(QActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = { /* QActive virtual table */
        { &QHsm_init_,
          &QHsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };
    QHsm_ctor(&me->super, initial);
    me->super.vptr = &vtbl.super; /* hook the QActive's vtable */
}
#endif /* Q_NHSM */

/*..........................................................................*/
#ifndef Q_NMSM

void QMActive_ctor(QMActive * const me, QStateHandler initial) {
    static QActiveVtbl const vtbl = { /* QMActive virtual table */
        { &QMsm_init_,
          &QMsm_dispatch_ },
        &QActive_postX_,
        &QActive_postXISR_
    };

    QMsm_ctor(&me->super, initial);/*call instead of QActive_ctor(), NOTE01 */
    me->super.vptr = &vtbl.super; /* hook the QMActive's vtable */
}

#endif /* Q_NMSM */

/*..........................................................................*/
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
            me->head = (uint_fast8_t)Q_ROM_BYTE(acb->end); /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        if (me->nUsed == (uint_fast8_t)1) { /* is this the first event? */
            QF_readySet_ |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
            SetEvent(l_win32Event);
        }
        margin = (uint_fast8_t)true; /* posting successful */
    }
    else {
        /* margin!=0 means that the caller can tolerate dropping evts */
        Q_ASSERT(margin != (uint_fast8_t)0);

        margin = (uint_fast8_t)false; /* posting failed */
    }
    QF_INT_ENABLE();

    return (bool)margin;
}
/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
bool QActive_postXISR_(QActive * const me, uint_fast8_t margin,
                       enum_t const sig, QParam const par)
#else
bool QActive_postXISR_(QActive * const me, uint_fast8_t margin,
                       enum_t const sig)
#endif
{
    QActiveCB const Q_ROM *acb = &QF_active[me->prio];
    uint_fast8_t end = (uint_fast8_t)Q_ROM_BYTE(acb->end);

    /* margin available? */
    if ((end - me->nUsed) > margin) {
        /* insert event into the ring buffer (FIFO) */
        QF_ROM_QUEUE_AT_(acb, me->head).sig = (QSignal)sig;
#if (Q_PARAM_SIZE != 0)
        QF_ROM_QUEUE_AT_(acb, me->head).par = par;
#endif
        if (me->head == (uint_fast8_t)0) {
            me->head = (uint_fast8_t)Q_ROM_BYTE(acb->end); /* wrap the head */
        }
        --me->head;
        ++me->nUsed;
        if (me->nUsed == (uint_fast8_t)1) { /* is this the first event? */
            QF_readySet_ |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
            SetEvent(l_win32Event);
        }
        margin = (uint_fast8_t)true; /* posting successful */
    }
    else {
        Q_ASSERT(margin != (uint_fast8_t)0); /* can tolerate dropping evts? */
        margin = (uint_fast8_t)false; /* posting failed */
    }

    return (bool)margin;
}

/*--------------------------------------------------------------------------*/
#if (QF_TIMEEVT_CTR_SIZE != 0)

/*..........................................................................*/
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
                QACTIVE_POST_ISR(a, (enum_t)Q_TIMEOUT_SIG +(enum_t)tickRate));
#endif
            }
        }
        --p;
    } while (p != (uint_fast8_t)0);
}
/*..........................................................................*/
void QActive_armX(QActive * const me, uint_fast8_t const tickRate,
                  QTimeEvtCtr const ticks)
{
    QF_INT_DISABLE();
    me->tickCtr[tickRate] = ticks;
#ifdef Q_TIMERSET
    QF_timerSetX_[tickRate] |= (uint_fast8_t)Q_ROM_BYTE(l_pow2Lkup[me->prio]);
#endif
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QActive_disarmX(QActive * const me, uint_fast8_t const tickRate) {
    QF_INT_DISABLE();
    me->tickCtr[tickRate] = (QTimeEvtCtr)0;
#ifdef Q_TIMERSET
    QF_timerSetX_[tickRate] &=
        (uint_fast8_t)Q_ROM_BYTE(l_invPow2Lkup[me->prio]);
#endif
    QF_INT_ENABLE();
}
#endif /* #if (QF_TIMEEVT_CTR_SIZE != 0) */

/*--------------------------------------------------------------------------*/
void QF_stop(void) {
    l_running = (uint_fast8_t)0;
    SetEvent(l_win32Event); /* unblock the event-loop so it can terminate */
}
/*..........................................................................*/
int_t QF_run(void) {
    uint_fast8_t p;
    QActive *a;

    InitializeCriticalSection(&l_win32CritSect);
    l_win32Event = CreateEvent(NULL, FALSE, FALSE, NULL);

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        Q_ASSERT(a != (QActive *)0); /* QF_active[p] must be initialized */
        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QMSM_INIT(&a->super); /* take the initial transition in the SM */
    }

    QF_onStartup(); /* invoke startup callback */

    l_running = (uint_fast8_t)1;
    Q_ALLEGE(CreateThread(NULL, 1024, &tickerThread, 0, 0, NULL)
             != (HANDLE)0); /* ticker thread must be created */

    while (l_running) { /* the event loop of the vanilla kernel */
        QF_INT_DISABLE();
        if (QF_readySet_ != (uint_fast8_t)0) {
            QActiveCB const Q_ROM *acb;

#ifdef QF_LOG2
            p = QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
            /* upper nibble used? */
            if ((uint_fast8_t)(QF_readySet_ & (uint_fast8_t)0xF0)
                != (uint_fast8_t)0)
            {
                p = (uint_fast8_t)(
                    (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                     + (uint_fast8_t)4);
            }
            else /* upper nibble of QF_readySet_ is zero */
#endif
            {
                p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }
#endif
            acb = &QF_active[p];
            a = QF_ROM_ACTIVE_GET_(p);

            /* some unsuded events must be available */
            Q_ASSERT_ID(310, a->nUsed > (uint_fast8_t)0);

            --a->nUsed;
            if (a->nUsed == (uint_fast8_t)0) { /* queue becoming empty? */
                QF_readySet_ &= (uint_fast8_t)Q_ROM_BYTE(l_invPow2Lkup[p]);
            }
            Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
            if (a->tail == (uint_fast8_t)0) { /* wrap around? */
                a->tail = (uint_fast8_t)Q_ROM_BYTE(acb->end);
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
/*..........................................................................*/
void QF_setTickRate(uint32_t ticksPerSec) {
    l_tickMsec = 1000UL / ticksPerSec;
}

/*..........................................................................*/
static DWORD WINAPI tickerThread(LPVOID par) {/* Win32 thread for time tick */
    (void)par; /* unused parameter */
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    while (l_running) {
        Sleep(l_tickMsec); /* wait for the tick interval */

        QF_INT_DISABLE();  /* make sure simulated ISR cannot be preempted */
        QF_onClockTickISR(); /* call back to the app, see NOTE01 */
        QF_INT_ENABLE();
    }
    return 0U; /* return success */
}

/*****************************************************************************
* NOTE01:
* The callback QF_onClockTickISR() is invoked with interupts disabled
* to emulate the ISR level. This means that only the ISR-level APIs are
* available inside the QF_onClockTickISR() callback.
*/

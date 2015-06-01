/*****************************************************************************
* Product: QF-nano emulation for Win32 with cooperative QV kernel
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
*****************************************************************************/

#include "qpn.h" /* QP-nano */

#include <pthread.h>    /* POSIX-thread API */
#include <sys/select.h> /* for the select() API */

#ifdef QK_PREEMPTIVE
    #error "This QP-nano port does not support QK_PREEMPTIVE configuration"
#endif

Q_DEFINE_THIS_MODULE("qfn_posix")

/* Global objects ==========================================================*/
uint_fast8_t volatile QF_readySet_; /* ready-set of QF-nano */

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

/* mutex for QF critical section */
static pthread_mutex_t l_pThreadMutex_ = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t l_condVar; /* cond var to signal when AOs are ready */
static long int l_tickUsec = 10000UL; /* clock tick in usec (for tv_usec) */
static bool l_isRunning;  /* flag indicating when QF is running */

/* "fudged" event queues for AOs, see NOTE2 */
#define QF_FUDGED_QUEUE_LEN  0xFFU
static QEvt l_fudgedQueue[QF_MAX_ACTIVE][QF_FUDGED_QUEUE_LEN];
#define QF_FUDGED_QUEUE_AT_(ao_, i_) (l_fudgedQueue[(ao_)->prio - 1U][(i_)])

static void *tickerThread(void *par); /* the expected P-Thread signature */

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
            pthread_cond_signal(&l_condVar); /* unblock the event loop */
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
            pthread_cond_signal(&l_condVar); /* unblock the event loop */
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
void QActive_disarmX(QActive * const me, uint_fast8_t const tickRate) {
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
    pthread_mutex_lock(&l_pThreadMutex_);
}
/****************************************************************************/
void QF_leaveCriticalSection_(void) {
    pthread_mutex_unlock(&l_pThreadMutex_);
}
/****************************************************************************/
int_t QF_run(void) {
    uint_fast8_t p;
    QMActive *a;
    pthread_t thread;

    pthread_cond_init(&l_condVar, 0);

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(810, a != (QMActive *)0);

        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QMSM_INIT(&a->super); /* take the initial transition in the SM */
    }

    QF_onStartup(); /* invoke startup callback */

    l_isRunning = true;
    Q_ALLEGE_ID(810, pthread_create(&thread, (pthread_attr_t *)0,
         &tickerThread, (void *)0) == 0); /* ticker thread must be created */

    /* the event loop of the vanilla kernel... */
    while (l_isRunning) {
        QF_INT_DISABLE();
        if (QF_readySet_ != (uint_fast8_t)0) {
            QActiveCB const Q_ROM *acb;

#if (QF_MAX_ACTIVE > 4)
            /* hi nibble non-zero? */
            if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
                p = (uint_fast8_t)(
                      (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                      + (uint_fast8_t)4);
            }
            /* hi nibble of QF_readySet_ is zero */
            else
#endif
            {
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
            /* yield the CPU until new event(s) arrive */
            pthread_cond_wait(&l_condVar, &l_pThreadMutex_);

            QF_INT_ENABLE();
        }
    }
    QF_onCleanup(); /* cleanup callback */
    pthread_cond_destroy(&l_condVar); /* cleanup the condition variable */
    pthread_mutex_destroy(&l_pThreadMutex_);

    return (int_t)0; /* success */
}
/****************************************************************************/
void QF_stop(void) {
    l_isRunning = false;    /* cause exit from the event loop */

    /* unblock the event loop so it can terminate */
    pthread_cond_signal(&l_condVar);
}
/****************************************************************************/
void QF_setTickRate(uint32_t ticksPerSec) {
    l_tickUsec = 1000000UL / ticksPerSec;
}

/*..........................................................................*/
static void *tickerThread(void *par) { /* the expected P-Thread signature */
    struct timeval timeout = { 0U, 0U }; /* timeout for select() */
    (void)par; /* unused parameter */

    while (l_isRunning) {
        timeout.tv_usec = l_tickUsec; /* set the desired tick interval */
        select(0, 0, 0, 0, &timeout); /* sleep for the desired tick, NOTE1 */

        QF_INT_DISABLE();
        QF_onClockTickISR(); /* call back to the app, see NOTE02 */
        QF_INT_ENABLE();
    }
    return (void *)0; /* return success */
}

/* NOTES: ********************************************************************
*
* NOTE1:
* The select() system call seems to deliver the finest time granularity of
* 1 clock tick. The timeout value passed to select() is rounded up to the
* nearest tick (10 ms on desktop Linux). The timeout cannot be too short,
* because the system might choose to busy-wait for very short timeouts.
* An alternative, POSIX nanosleep() system call seems to deliver only 20ms
* granularity.
*
* Here the select() call is used not just as a fairly portable way to sleep
* with subsecond precision. The select() call is also used to detect any
* characters typed on the console.
*
* Also according to man pages, on Linux, the function select() modifies
* timeout to reflect the amount of time not slept; most other implementations
* do not do this. This causes problems both when Linux code which reads
* timeout is ported to other operating systems, and when code is ported to
* Linux that reuses a struct timeval for multiple selects in a loop without
* reinitializing it. Here the microsecond part of the structure is re-
* initialized before each select() call.
*
* NOTE2:
* POSIX is not necessariliy a deterministic real-time system, which means
* that the system can occasionally and unexpectedly "choke and freeze" for
* a number of seconds. The designers of POSIX have dealt with these sort
* of issues by massively oversizing the resources available to the
* applications. For example, the the stacks of POSIX-threads can dynamically
* grow to several megabytes.
*
* In contrast, the event queues, event pools, and stack size inside the
* real-time embedded (RTE) systems can be (and must be) much smaller,
* because you typically can put an upper bound on the real-time behavior
* and the resulting delays.
*
* To be able to run the unmodified applications designed originally for
* RTE systems on POSIX, and to reduce the odds of resource shortages in
* this case, the event queues of all Active Objects are "fudged" to the
* maximum dynamic range of uint_fast8_t data type, which is 0xFF
* (see QF_FUDGED_QUEUE_LEN).
*
* NOTE3:
* The callback QF_onClockTickISR() is invoked with interupts disabled
* to emulate the ISR level. This means that only the ISR-level APIs are
* available inside the QF_onClockTickISR() callback.
*/



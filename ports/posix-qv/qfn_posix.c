/**
* @file
* @brief QF-nano port to POSIX API with cooperative QV scheduler (posix-qv)
* @ingroup ports
* @cond
******************************************************************************
* Last updated for version 6.6.0
* Last updated on  2019-10-04
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
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
* <www.state-machine.com>
* <info@state-machine.com>
******************************************************************************
* @endcond
*/
#include "qpn.h" /* QP-nano */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#ifdef qkn_h
    #error "This QP-nano port does not support QK-nano configuration"
#endif

Q_DEFINE_THIS_MODULE("qfn_posix")

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

/* Local objects ===========================================================*/
/* mutex for QF critical section */
static pthread_mutex_t l_pThreadMutex_;
static pthread_cond_t l_condVar; /* cond var to signal when AOs are ready */
static struct termios l_tsav; /* structure with saved terminal attributes */
static bool l_isRunning;  /* flag indicating when QF is running */
static struct timespec l_tick;
static int_t l_tickPrio;
enum { NANOSLEEP_NSEC_PER_SEC = 1000000000 }; /* see NOTE1 */

/* "fudged" event queues for AOs, see NOTE2 */
#define QF_FUDGED_QUEUE_LEN  0xFFU
static QEvt l_fudgedQueue[8][QF_FUDGED_QUEUE_LEN];
#define QF_FUDGED_QUEUE_AT_(ao_, i_) (l_fudgedQueue[(ao_)->prio - 1U][(i_)])

static void *tickerThread(void *par); /* the expected P-Thread signature */
static void sigIntHandler(int dummy);

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

/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
bool QActive_postX_(QActive * const me, uint_fast8_t margin,
                    enum_t const sig, QParam const par)
#else
bool QActive_postX_(QActive * const me, uint_fast8_t margin,
                    enum_t const sig)
#endif
{
    QF_INT_DISABLE();

    if (margin == QF_NO_MARGIN) {
        if ((uint_fast8_t)QF_FUDGED_QUEUE_LEN > me->nUsed) {
            margin = (uint_fast8_t)true; /* can post */
        }
        else {
            margin = (uint_fast8_t)false; /* cannot post */
            Q_ERROR_ID(310); /* must be able to post the event */
        }
    }
    else if (((uint_fast8_t)QF_FUDGED_QUEUE_LEN - me->nUsed) > margin) {
        margin = (uint_fast8_t)true; /* can post */
    }
    else {
        margin = (uint_fast8_t)false; /* cannot post */
    }

    if (margin) { /* can post the event? */
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
            QF_readySet_ |= (uint_fast8_t)
                ((uint_fast8_t)1U << (me->prio - (uint_fast8_t)1));
            pthread_cond_signal(&l_condVar); /* unblock the event loop */
        }
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
    if (margin == QF_NO_MARGIN) {
        if ((uint_fast8_t)QF_FUDGED_QUEUE_LEN > me->nUsed) {
            margin = (uint_fast8_t)true; /* can post */
        }
        else {
            margin = (uint_fast8_t)false; /* cannot post */
            Q_ERROR_ID(310); /* must be able to post the event */
        }
    }
    else if (((uint_fast8_t)QF_FUDGED_QUEUE_LEN - me->nUsed) > margin) {
        margin = (uint_fast8_t)true; /* can post */
    }
    else {
        margin = (uint_fast8_t)false; /* cannot post */
    }

    if (margin) { /* can post the event? */
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
            QF_readySet_ |= (uint_fast8_t)
                ((uint_fast8_t)1 << (me->prio - (uint_fast8_t)1));
            pthread_cond_signal(&l_condVar); /* unblock the event loop */
        }
    }

    return (bool)margin;
}


/****************************************************************************/
#if (QF_TIMEEVT_CTR_SIZE != 0)

void QF_tickXISR(uint_fast8_t const tickRate) {
    uint_fast8_t p = QF_maxActive_;
    do {
        QActive *a = QF_ROM_ACTIVE_GET_(p);
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
                QF_timerSetX_[tickRate] &= (uint_fast8_t)
                    ~((uint_fast8_t)1 << (p - (uint_fast8_t)1));
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
/*..........................................................................*/
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
    QF_timerSetX_[tickRate] |=  (uint_fast8_t)
        ((uint_fast8_t)1 << (me->prio - (uint_fast8_t)1));
#endif
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QActive_disarmX(QActive * const me, uint_fast8_t const tickRate) {
    QF_INT_DISABLE();
    me->tickCtr[tickRate].nTicks = (QTimeEvtCtr)0;
#ifdef QF_TIMEEVT_PERIODIC
    me->tickCtr[tickRate].interval = (QTimeEvtCtr)0;
#endif /* QF_TIMEEVT_PERIODIC */

#ifdef QF_TIMEEVT_USAGE
    /* clear a bit in QF_timerSetX_[] to rememer that timer is not running */
    QF_timerSetX_[tickRate] &= (uint_fast8_t)
        ~((uint_fast8_t)1 << (me->prio - (uint_fast8_t)1));
#endif
    QF_INT_ENABLE();
}
#endif /* #if (QF_TIMEEVT_CTR_SIZE != 0) */

/* QF functions ============================================================*/
void QF_enterCriticalSection_(void) {
    pthread_mutex_lock(&l_pThreadMutex_);
}
/*..........................................................................*/
void QF_leaveCriticalSection_(void) {
    pthread_mutex_unlock(&l_pThreadMutex_);
}
/*..........................................................................*/
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
    struct sigaction sig_act;

#if (defined(QF_TIMEEVT_USAGE) || (QF_TIMEEVT_CTR_SIZE != 0))
    uint_fast8_t n;
#endif /* QF_TIMEEVT_USAGE */

    /** @pre the number of active objects must be in range */
    Q_REQUIRE_ID(100, ((uint_fast8_t)1 < maxActive)
                      && (maxActive <= (uint_fast8_t)9));
    QF_maxActive_ = (uint_fast8_t)maxActive - (uint_fast8_t)1;

    /* init the global mutex with the default non-recursive initializer */
    pthread_mutex_init(&l_pThreadMutex_, NULL);

    l_tick.tv_sec = 0;
    l_tick.tv_nsec = NANOSLEEP_NSEC_PER_SEC/100L; /* default clock tick */
    l_tickPrio = sched_get_priority_min(SCHED_FIFO); /* default tick prio */

    /* install the SIGINT (Ctrl-C) signal handler */
    sig_act.sa_handler = &sigIntHandler;
    sigaction(SIGINT, &sig_act, NULL);

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

#ifdef QK_SCHED_LOCK
    QK_lockPrio_ = (uint_fast8_t)0;
#endif

#endif /* #ifdef QK_PREEMPTIVE */

    /* clear all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(110, a != (QActive *)0);

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
/*..........................................................................*/
int_t QF_run(void) {
    uint_fast8_t p;
    QActive *a;
    pthread_t thread;
    struct sched_param sparam;

    pthread_cond_init(&l_condVar, 0);

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(810, a != (QActive *)0);

        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QHSM_INIT(&a->super); /* take the initial transition in the HSM */
    }

    QF_onStartup(); /* invoke startup callback */

    /* try to set the priority of the ticker thread, see NOTE01 */
    sparam.sched_priority = l_tickPrio;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sparam) == 0) {
        /* success, this application has sufficient privileges */
    }
    else {
        /* setting priority failed, probably due to insufficient privieges */
    }

    l_isRunning = true;

    /* system clock tick configured? */
    if ((l_tick.tv_sec != 0) || (l_tick.tv_nsec != 0)) {
        Q_ALLEGE_ID(810, pthread_create(&thread, (pthread_attr_t *)0,
             &tickerThread, (void *)0) == 0); /* ticker thread must be created */
    }

    /* the event loop of the QV-nano kernel... */
    QF_INT_DISABLE();
    while (l_isRunning) {
        if (QF_readySet_ != (uint_fast8_t)0) {

            /* hi nibble non-zero? */
            if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
                p = (uint_fast8_t)(
                      (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                      + (uint_fast8_t)4);
            }
            else { /* hi nibble of QF_readySet_ is zero */
                p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }

            a = QF_ROM_ACTIVE_GET_(p);

            /* some unsuded events must be available */
            Q_ASSERT_ID(820, a->nUsed > (uint_fast8_t)0);

            --a->nUsed;
            Q_SIG(a) = QF_FUDGED_QUEUE_AT_(a, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_FUDGED_QUEUE_AT_(a, a->tail).par;
#endif
            if (a->tail == (uint_fast8_t)0) { /* wrap around? */
                a->tail = (uint_fast8_t)QF_FUDGED_QUEUE_LEN;
            }
            --a->tail;
            QF_INT_ENABLE();

            QHSM_DISPATCH(&a->super); /* dispatch to the HSM (RTC step) */

            QF_INT_DISABLE();
            /* empty queue? */
            if (a->nUsed == (uint_fast8_t)0) {
                /* clear the bit corresponding to 'p' */
                QF_readySet_ &= (uint_fast8_t)
                    ~((uint_fast8_t)1 << (p - (uint_fast8_t)1));
            }
        }
        else {
            /* the QV kernel in embedded systems calls here the QV_onIdle()
            * callback. However, the POSIX-QV port does not do busy-waiting
            * for events. Instead, the POSIX-QV port efficiently waits until
            * QP events become available.
            */
            while (QF_readySet_ == (uint_fast8_t)0) {
                pthread_cond_wait(&l_condVar, &l_pThreadMutex_);
            }
        }
    }
    QF_INT_ENABLE();
    QF_onCleanup(); /* cleanup callback */
    pthread_cond_destroy(&l_condVar); /* cleanup the condition variable */
    pthread_mutex_destroy(&l_pThreadMutex_);

    return (int_t)0; /* success */
}
/*..........................................................................*/
void QF_stop(void) {
    l_isRunning = false;    /* cause exit from the event loop */

    /* unblock the event loop so it can terminate */
    QF_readySet_ = (uint_fast8_t)1;
    pthread_cond_signal(&l_condVar);
}

/****************************************************************************/
void QF_setTickRate(uint32_t ticksPerSec, int_t tickPrio) {
    if (ticksPerSec != (uint32_t)0) {
        l_tick.tv_nsec = NANOSLEEP_NSEC_PER_SEC / ticksPerSec;
    }
    else {
        l_tick.tv_nsec = 0; /* means NO system clock tick */
    }
    l_tickPrio = tickPrio;
}
/*..........................................................................*/
void QF_consoleSetup(void) {
    struct termios tio;   /* modified terminal attributes */

    tcgetattr(0, &l_tsav); /* save the current terminal attributes */
    tcgetattr(0, &tio);    /* obtain the current terminal attributes */
    tio.c_lflag &= ~(ICANON | ECHO); /* disable the canonical mode & echo */
    tcsetattr(0, TCSANOW, &tio);     /* set the new attributes */
}
/*..........................................................................*/
void QF_consoleCleanup(void) {
    tcsetattr(0, TCSANOW, &l_tsav); /* restore the saved attributes */
}
/*..........................................................................*/
int QF_consoleGetKey(void) {
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    if (byteswaiting > 0) {
        char ch;
        read(0, &ch, 1);
        return (int)ch;
    }
    return 0; /* no input at this time */
}
/*..........................................................................*/
int QF_consoleWaitForKey(void) {
    return getchar();
}


/*..........................................................................*/
static void *tickerThread(void *par) { /* the expected P-Thread signature */
    (void)par; /* unused parameter */

    while (l_isRunning) {
        nanosleep(&l_tick, NULL); /* sleep for the number of ticks, NOTE1 */

        QF_INT_DISABLE();
        QF_onClockTickISR(); /* call back to the app, see NOTE2 */
        QF_INT_ENABLE();
    }
    return (void *)0; /* return success */
}

/****************************************************************************/
static void sigIntHandler(int dummy) {
    (void)dummy; /* unused parameter */
    QF_onCleanup();
    exit(-1);
}

/* NOTES: ********************************************************************
*
* NOTE1:
* In some (older) Linux kernels, the POSIX nanosleep() system call might
* deliver only 2*actual-system-tick granularity. To compensate for this,
* you would need to reduce (by 2) the constant NANOSLEEP_NSEC_PER_SEC.
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


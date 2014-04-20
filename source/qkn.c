/**
* \file
* \brief QK-nano implementation.
* \ingroup qkn
* \cond
******************************************************************************
* Product: QK-nano
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

Q_DEFINE_THIS_MODULE("qkn")

#ifndef QK_PREEMPTIVE
    #error "The required header file qkn.h is not included in qpn_port.h"
#endif

/* Global-scope objects *****************************************************/

/* start with the QK scheduler locked */
uint_fast8_t volatile QK_currPrio_ = (uint_fast8_t)(QF_MAX_ACTIVE + 1);

#ifdef QF_ISR_NEST
uint_fast8_t volatile QK_intNest_; /* interrupt nesting for QK kernel */
#endif

#ifndef QK_NO_MUTEX
uint_fast8_t volatile QK_ceilingPrio_; /* ceiling priority of a mutex */
#endif

static void initialize(void); /* prototype required by MISRA */

/****************************************************************************/
/**
* \description
* Helper function to set the priorities of all the statically allocated
* active objects in the system followed by executing the top-most initial
* transtions in all active objects.
*
* \note
* The system initialization is specifically encapsulated in a function
* to reduce stack use, because the temporary stack variables needed
* for the initialization go out of scope for the rest of system execution.
*/
static void initialize(void) {
    uint_fast8_t p;
    QActive *a;

#ifdef QK_INIT /* initialization of the QK-nano kernel defined? */
    QK_INIT(); /* initialize the QK-nano kernel */
#endif

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(110, a != (QActive *)0);

        a->prio = p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QMSM_INIT(&a->super); /* take the initial transition in the SM */
    }

    QF_INT_DISABLE();
    QK_currPrio_ = (uint_fast8_t)0; /* the priority for the QK idle loop */
    p = QK_schedPrio_();
    if (p != (uint_fast8_t)0) {
        QK_sched_(p); /* process all events produced so far */
    }
    QF_INT_ENABLE();
}

/****************************************************************************/
/**
* \description
* QF_run() is typically called from your startup code after you initialize
* the QF and start at least one active object with QActive_start().
* This implementation of QF_run() is for the preemptive QK-nano kernel.
*
* \returns QF_run() typically does not return in embedded applications.
* However, when QP runs on top of an operating system,  QF_run() might
* return and in this case the return represents the error code (0 for
* success). Typically the value returned from QF_run() is subsequently
* passed on as return from main().
*/
int_t QF_run(void) {
    initialize();
    QF_onStartup();  /* invoke startup callback */

    /* the QK idle loop */
    for (;;) {
        QK_onIdle(); /* invoke the on-idle callback */
    }
#ifdef __GNUC__  /* GNU compiler? */
    return (int_t)0;
#endif
}

/****************************************************************************/
/**
* \description
* This function finds out the priority of the highest-priority active object
* that (1) has events to process, and (2) has priority that is above the
* current priority, and (3) has priority that is above the mutex ceiling,
* if mutex is configured in the port.
*
* \returns the 1-based priority of the the active object, or zero if
* no eligible active object is ready to run.
*
* \attention QK_schedPrio_() must be always called with interrupts
* __disabled__  and returns with interrupts __disabled__.
*/
uint_fast8_t QK_schedPrio_(void) {
    uint_fast8_t p; /* highest-priority active object ready to run */

    /* determine the priority of the highest-priority AO ready to run */
#ifdef QF_LOG2
    p = (uint_fast8_t)QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
    /* hi nibble used? */
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
#endif

    /* below the current preemption threshold? */
    if (p <= QK_currPrio_) {
        p = (uint_fast8_t)0;
    }
#ifndef QK_NO_MUTEX
    /* below the mutex ceiling? */
    else if (p <= QK_ceilingPrio_) {
        p = (uint_fast8_t)0;
    }
    else {
        /* empty */
    }
#endif
    return p;
}

/****************************************************************************/
/**
* \arguments
* \arg[in] \c p  priority of the next AO to schedule, typically obtained
*                from QK_schedPrio_().
*
* \attention QK_sched_() must be always called with interrupts
* __disabled__  and returns with interrupts __disabled__.
*
* \note The scheduler might enable interrupts internally, but always
* returns with interrupts __disabled__.
*/
void QK_sched_(uint_fast8_t p) {
    static uint8_t const Q_ROM invPow2Lkup[] = {
        (uint8_t)0xFF,
        (uint8_t)0xFE, (uint8_t)0xFD, (uint8_t)0xFB, (uint8_t)0xF7,
        (uint8_t)0xEF, (uint8_t)0xDF, (uint8_t)0xBF, (uint8_t)0x7F
    };
    uint_fast8_t pin = QK_currPrio_; /* save the initial priority */

    do {
        QActive *a;
        QActiveCB const Q_ROM *acb;

        QK_currPrio_ = p; /* new priority becomes the current priority */
        QF_INT_ENABLE();  /* it's safe to leave critical section */

        acb = &QF_active[p];
        a = QF_ROM_ACTIVE_GET_(p); /* map p to AO */

        QF_INT_DISABLE(); /* get ready to access the queue */

        /* some unused events must be available */
        Q_ASSERT_ID(410, a->nUsed > (uint_fast8_t)0);
        --a->nUsed;

        /* queue becoming empty? */
        if (a->nUsed == (uint_fast8_t)0) {
            /* clear the ready bit */
            QF_readySet_ &= (uint_fast8_t)Q_ROM_BYTE(invPow2Lkup[p]);
        }
        Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
        Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
        /* wrap around? */
        if (a->tail == (uint_fast8_t)0) {
            a->tail = (uint_fast8_t)Q_ROM_BYTE(acb->end);
        }
        --a->tail;
        QF_INT_ENABLE(); /* enable interrupts to launch a task */

        QMSM_DISPATCH(&a->super); /* dispatch to the SM (execute RTC step) */

        QF_INT_DISABLE();

        /* determine the highest-priority AO ready to run... */
#ifdef QF_LOG2
        p = QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
        /* hi nibble not zero? */
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
#endif

        /* below the current preemption threshold? */
        if (p <= pin) {
            p = (uint_fast8_t)0;
        }
#ifndef QK_NO_MUTEX
        /* below the mutex ceiling? */
        else if (p <= QK_ceilingPrio_) {
            p = (uint_fast8_t)0;
        }
        else {
            /* empty */
        }
#endif
    } while (p != (uint_fast8_t)0);

    QK_currPrio_ = pin; /* restore the initial priority */
}


/****************************************************************************/
/****************************************************************************/
#ifndef QK_NO_MUTEX

/****************************************************************************/
/**
* \description
* Lock the QK scheduler up to the given priority level.
*
* \arguments
* \arg[in] \c prioCeiling  priority ceiling to lock the mutex
*
* \returns the previous value of the mutex priority ceiling
*
* \note This function should be always paired with QK_mutexUnlock(). The
* code between QK_mutexLock() and QK_mutexUnlock() should be kept to the
* minimum.
*
* \usage
* \include qkn_mux.c
*/
QMutex QK_mutexLock(uint_fast8_t const prioCeiling) {
    uint_fast8_t mutex;
    QF_INT_DISABLE();
    mutex = QK_ceilingPrio_;  /* the original QK priority ceiling to return */
    if (QK_ceilingPrio_ < prioCeiling) {
        QK_ceilingPrio_ = prioCeiling;     /* raise the QK priority ceiling */
    }
    QF_INT_ENABLE();
    return mutex;
}

/****************************************************************************/
/**
* \description
* Unlock the QK scheduler up to the saved priority level.
*
* \arguments
* \arg[in] \c mutex  previous priority level to unlock the mutex
*
* \description
* \note This function should be always paired with QK_mutexLock(). The
* code between QK_mutexLock() and QK_mutexUnlock() should be kept to the
* minimum.
*
* \usage
* \include qkn_mux.c
*/
void QK_mutexUnlock(QMutex mutex) {
    QF_INT_DISABLE();
    if (QK_ceilingPrio_ > mutex) {
        QK_ceilingPrio_ = mutex;  /* restore the saved priority ceiling */
        mutex = QK_schedPrio_();  /* reuse 'mutex' to hold priority */
        if (mutex != (uint_fast8_t)0) {
            QK_sched_(mutex);
        }
    }
    QF_INT_ENABLE();
}

#endif /* #ifdef QK_MUTEX */


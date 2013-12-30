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

Q_DEFINE_THIS_MODULE("qkn")

/**
* \file
* \ingroup qkn
* QK-nano implementation.
*/

#ifndef QK_PREEMPTIVE
    #error "The required header file qkn.h is not included in qpn_port.h"
#endif

/* Global-scope objects ----------------------------------------------------*/
                                      /* start with the QK scheduler locked */
uint8_t volatile QK_currPrio_ = (uint8_t)(QF_MAX_ACTIVE + 1);

#ifdef QF_ISR_NEST
uint8_t volatile QK_intNest_;            /* interrupt nesting for QK kernel */
#endif

#ifndef QK_NO_MUTEX
uint8_t volatile QK_ceilingPrio_;            /* ceiling priority of a mutex */
#endif

static void initialize(void);                /* prototype required by MISRA */

/*..........................................................................*/
static void initialize(void) {
    uint8_t p;
    QActive *a;

#ifdef QK_INIT             /* initialization of the QK-nano kernel defined? */
    QK_INIT();                             /* initialize the QK-nano kernel */
#endif
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

    QF_INT_DISABLE();
    QK_currPrio_ = (uint8_t)0;     /* set the priority for the QK idle loop */
    p = QK_schedPrio_();
    if (p != (uint8_t)0) {
        QK_sched_(p);                 /* process all events produced so far */
    }
    QF_INT_ENABLE();
}
/*..........................................................................*/
int_t QF_run(void) {
    initialize();
    QF_onStartup();                              /* invoke startup callback */

    for (;;) {                                    /* enter the QK idle loop */
        QK_onIdle();                         /* invoke the on-idle callback */
    }
#ifdef __GNUC__                                            /* GNU compiler? */
    return (int_t)0;
#endif
}

/*..........................................................................*/
/* NOTE: QK schePrio_() is entered and exited with interrupts LOCKED */
uint8_t QK_schedPrio_(void) {
    uint8_t p;               /* highest-priority active object ready to run */

          /* determine the priority of the highest-priority AO ready to run */
#ifdef QF_LOG2
    p = QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
    if ((QF_readySet_ & (uint8_t)0xF0) != (uint8_t)0) {  /* hi nibble used? */
        p = (uint8_t)(Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                      + (uint8_t)4);
    }
    else                               /* hi nibble of QF_readySet_ is zero */
#endif
    {
        p = Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
    }
#endif

    if (p <= QK_currPrio_) {     /* below the current preemption threshold? */
        p = (uint8_t)0;
    }
#ifndef QK_NO_MUTEX
    else if (p <= QK_ceilingPrio_) {            /* below the mutex ceiling? */
        p = (uint8_t)0;
    }
    else {
        /* empty */
    }
#endif
    return p;
}
/*..........................................................................*/
/* NOTE: QK_sched_() is entered and exited with interrupts LOCKED */
void QK_sched_(uint8_t p) {
    static uint8_t const Q_ROM invPow2Lkup[] = {
        (uint8_t)0xFF,
        (uint8_t)0xFE, (uint8_t)0xFD, (uint8_t)0xFB, (uint8_t)0xF7,
        (uint8_t)0xEF, (uint8_t)0xDF, (uint8_t)0xBF, (uint8_t)0x7F
    };
    uint8_t pin = QK_currPrio_;                /* save the initial priority */

    do {
        QActive *a;
        QActiveCB const Q_ROM *acb;

        QK_currPrio_ = p;      /* new priority becomes the current priority */
        QF_INT_ENABLE();             /* it's safe to leave critical section */

        acb = &QF_active[p];
        a = QF_ROM_ACTIVE_GET_(p);              /* map p to AO */

        QF_INT_DISABLE();                  /* get ready to access the queue */
        Q_ASSERT(a->nUsed > (uint8_t)0);   /* some events must be available */
        --a->nUsed;
        if (a->nUsed == (uint8_t)0) {              /* queue becoming empty? */
            QF_readySet_ &= Q_ROM_BYTE(invPow2Lkup[p]);    /* clear the bit */
        }
        Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
        Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
        if (a->tail == (uint8_t)0) {                        /* wrap around? */
            a->tail = Q_ROM_BYTE(acb->end);
        }
        --a->tail;
        QF_INT_ENABLE();              /* enable interrupts to launch a task */

        QMSM_DISPATCH(&a->super);  /* dispatch to the SM (execute RTC step) */

        QF_INT_DISABLE();

                       /* determine the highest-priority AO ready to run... */
#ifdef QF_LOG2
        p = QF_LOG2(QF_readySet_);
#else

#if (QF_MAX_ACTIVE > 4)
        if ((QF_readySet_ & (uint8_t)0xF0) != (uint8_t)0) {   /* hi nibble? */
            p = (uint8_t)(Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                          + (uint8_t)4);
        }
        else                           /* hi nibble of QF_readySet_ is zero */
#endif
        {
            p = Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
        }
#endif

        if (p <= pin) {          /* below the current preemption threshold? */
            p = (uint8_t)0;
        }
#ifndef QK_NO_MUTEX
        else if (p <= QK_ceilingPrio_) {        /* below the mutex ceiling? */
            p = (uint8_t)0;
        }
        else {
            /* empty */
        }
#endif
    } while (p != (uint8_t)0);

    QK_currPrio_ = pin;                     /* restore the initial priority */
}

#ifndef QK_NO_MUTEX
/*..........................................................................*/
QMutex QK_mutexLock(uint8_t const prioCeiling) {
    uint8_t mutex;
    QF_INT_DISABLE();
    mutex = QK_ceilingPrio_;  /* the original QK priority ceiling to return */
    if (QK_ceilingPrio_ < prioCeiling) {
        QK_ceilingPrio_ = prioCeiling;     /* raise the QK priority ceiling */
    }
    QF_INT_ENABLE();
    return mutex;
}
/*..........................................................................*/
void QK_mutexUnlock(QMutex mutex) {
    QF_INT_DISABLE();
    if (QK_ceilingPrio_ > mutex) {
        QK_ceilingPrio_ = mutex;      /* restore the saved priority ceiling */
        mutex = QK_schedPrio_();          /* reuse 'mutex' to hold priority */
        if (mutex != (uint8_t)0) {
            QK_sched_(mutex);
        }
    }
    QF_INT_ENABLE();
}
#endif                                                   /* #ifdef QK_MUTEX */


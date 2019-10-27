/**
* @file
* @brief QK-nano preemptive kernel implementation.
* @ingroup qkn
* @cond
******************************************************************************
* Last updated for version 6.6.0
* Last updated on  2019-07-30
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
#include "qpn_conf.h" /* QP-nano configuration file (from the application) */
#include "qfn_port.h" /* QF-nano port from the port directory */
#include "qassert.h"  /* embedded systems-friendly assertions */

Q_DEFINE_THIS_MODULE("qkn")

/* protection against including this source file in a wrong project */
#ifndef QKN_H
    #error "Source file included in a project NOT based on the QK-nano kernel"
#endif /* QKN_H */

/* Public-scope objects *****************************************************/
QK_Attr QK_attr_; /* global attributes of the QK-nano kernel */

/* Local-scope objects ******************************************************/
static void initialize(void); /* prototype required by MISRA */

/****************************************************************************/
/**
* @description
* Helper function to set the priorities of all the statically allocated
* active objects in the system followed by executing the top-most initial
* transtions in all active objects.
*
* @note
* The system initialization is specifically encapsulated in a function
* to reduce stack use, because the temporary stack variables needed
* for the initialization go out of scope for the rest of system execution.
*/
static void initialize(void) {
    uint_fast8_t p;
    QActive *a;

#ifdef QF_MAX_ACTIVE /* deprecated constant provided? */
#if (QF_MAX_ACTIVE < 1) || (8 < QF_MAX_ACTIVE)
    #error "QF_MAX_ACTIVE not defined or out of range. Valid range is 1..8"
#endif
    QF_maxActive_ = (uint_fast8_t)QF_MAX_ACTIVE;
#else
    /** @pre the number of active objects must be initialized by calling:
    * QF_init(Q_DIM(QF_active));
    */
    Q_REQUIRE_ID(100, ((uint_fast8_t)1 <= QF_maxActive_)
                      && (QF_maxActive_ <= (uint_fast8_t)8));
#endif

    /* set priorities all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(110, a != (QActive *)0);

        a->prio = (uint8_t)p; /* set the priority of the active object */
    }

    /* trigger initial transitions in all registered active objects... */
    for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        QHSM_INIT(&a->super); /* take the initial transition in the SM */
    }

    /* process all events posted during initialization... */
    QF_INT_DISABLE();
    QK_attr_.actPrio = (uint8_t)0; /* prio of the QK-nano idle loop */
    if (QK_sched_() != (uint_fast8_t)0) {
        QK_activate_(); /* activate AOs to process all events posted so far */
    }
    QF_INT_ENABLE();
}

/****************************************************************************/
/**
* @description
* QF_run() is typically called from your startup code after you initialize
* the QF and start at least one active object with QActive_start().
* This implementation of QF_run() is for the preemptive QK-nano kernel.
*
* @returns
* In QK-nano QF_run() does not return.
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
/****************************************************************************/
#ifdef QK_SCHED_LOCK

/****************************************************************************/
/**
* @description
* This function locks the QK scheduler to the specified ceiling.
*
* @param[in]   ceiling    priority ceiling to which the QK scheduler
*                         needs to be locked
*
* @returns
* The previous QK Scheduler lock status, which is to be used to unlock
* the scheduler by restoring its previous lock status in QK_schedUnlock().
*
* @note
* QK_schedLock() must be always followed by the corresponding
* QK_schedUnlock().
*
* @sa QK_schedUnlock()
*
* @usage
* The following example shows how to lock and unlock the QK scheduler:
* @include qkn_lock.c
*/
QSchedStatus QK_schedLock(uint_fast8_t ceiling) {
    QSchedStatus stat;
    QF_INT_DISABLE();

    /* first store the previous lock prio */
    /* raising the lock prio? */
    if ((uint_fast8_t)QK_attr_.lockPrio < ceiling) {
        stat = (QSchedStatus)QK_attr_.lockPrio << 8;
        QK_attr_.lockPrio = (uint8_t)ceiling;

        /* add the previous lock holder priority */
        stat |= (QSchedStatus)QK_attr_.lockHolder;

        QK_attr_.lockHolder = QK_attr_.actPrio;
    }
    else {
       stat = (QSchedStatus)0xFF;
    }
    QF_INT_ENABLE();

    return stat; /* return the status to be saved in a stack variable */
}

/****************************************************************************/
/**
* @description
* This function unlocks the QK scheduler to the previous status.
*
* @param[in]   stat       previous QK Scheduler lock status returned from
*                         QK_schedLock()
* @note
* QK_schedUnlock() must always follow the corresponding QK_schedLock().
*
* @sa QK_schedLock()
*
* @usage
* The following example shows how to lock and unlock the QK scheduler:
* @include qkn_lock.c
*/
void QK_schedUnlock(QSchedStatus stat) {
    /* has the scheduler been actually locked by the last QK_schedLock()? */
    if (stat != (QSchedStatus)0xFF) {
        uint_fast8_t lockPrio = (uint_fast8_t)QK_attr_.lockPrio;
        uint_fast8_t prevPrio = (uint_fast8_t)(stat >> 8);

        QF_INT_DISABLE();

        /** @pre
        * The current lock priority must be greater than the previous
        */
        Q_REQUIRE_ID(700, lockPrio > prevPrio);
#ifdef Q_NASSERT
        (void)lockPrio; /* avoid compiler warning about unused variable */
#endif

        /* restore the previous lock priority and lock holder */
        QK_attr_.lockPrio   = (uint8_t)prevPrio;
        QK_attr_.lockHolder = (uint8_t)(stat & (QSchedStatus)0xFF);

        /* find the highest-prio thread ready to run */
        if (QK_sched_() != (uint_fast8_t)0) { /* priority found? */
            QK_activate_(); /* activate any unlocked basic threads */
        }

        QF_INT_ENABLE();
    }
}

#endif /* #ifdef QK_SCHED_LOCK */


/****************************************************************************/
/****************************************************************************/
/**
* @description
* The QK-nano scheduler finds out the priority of the highest-priority AO
* that (1) has events to process and (2) has priority that is above the
* current priority.
*
* @returns the 1-based priority of the the active object, or zero if
* no eligible active object is ready to run.
*
* @attention
* QK_sched_() must be always called with interrupts **disabled** and
* returns with interrupts **disabled**.
*/
uint_fast8_t QK_sched_(void) {
    uint_fast8_t p; /* for priority */

    /* find the highest-prio AO with non-empty event queue */
#ifdef QF_LOG2
    p = (uint_fast8_t)QF_LOG2(QF_readySet_);
#else
    /* hi nibble used? */
    if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
        p = (uint_fast8_t)(
                (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                + (uint_fast8_t)4);
    }
    else { /* hi nibble of QF_readySet_ is zero */
        p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
    }
#endif

    /* is the highest-prio below the active priority? */
    if (p <= (uint_fast8_t)QK_attr_.actPrio) {
        p = (uint_fast8_t)0; /* active object not eligible */
    }
#ifdef QK_SCHED_LOCK
    /* below the scheduler ceiling? */
    else if (p <= (uint_fast8_t)QK_attr_.lockPrio) {
        p = (uint_fast8_t)0; /* active object not eligible */
    }
#endif /* QK_SCHED_LOCK */
    else {
        Q_ASSERT_ID(610, p <= QF_maxActive_);
        QK_attr_.nextPrio = (uint8_t)p; /* next AO to run */
    }

    return p;
}

/****************************************************************************/
/**
* @description
* QK_activate_() activates ready-to run AOs that are above the initial
* active priority (QK_attr_.actPrio).
*
* @note
* The activator might enable interrupts internally, but always returns with
* interrupts **disabled**.
*/
void QK_activate_(void) {
    uint_fast8_t pin = (uint_fast8_t)QK_attr_.actPrio;  /* save active prio */
    uint_fast8_t p   = (uint_fast8_t)QK_attr_.nextPrio; /* next prio to run */

    /* QK Context switch callback enabled? */
#ifdef QK_ON_CONTEXT_SW
    uint_fast8_t pprev = pin;
#endif /* QK_ON_CONTEXT_SW */

    /* QK_attr_.nextPrio must be non-zero upon entry to QK_activate_() */
    Q_REQUIRE_ID(800, p != (uint_fast8_t)0);

    QK_attr_.nextPrio = (uint8_t)0; /* clear for the next time */

    /* loop until no more ready-to-run AOs of higher prio than the initial */
    do {
        QActive *a;
        QActiveCB const Q_ROM *acb;

        QK_attr_.actPrio = (uint8_t)p; /* this becomes the active priority */
        QF_INT_ENABLE();  /* it's safe to leave critical section */

        acb = &QF_active[p];
        a = QF_ROM_ACTIVE_GET_(p); /* map p to AO */

        QF_INT_DISABLE(); /* get ready to access the queue */

        /* some unused events must be available */
        Q_ASSERT_ID(810, a->nUsed > (uint8_t)0);
        --a->nUsed;

        Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
        Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
        /* wrap around? */
        if (a->tail == (uint8_t)0) {
            a->tail = Q_ROM_BYTE(acb->qlen);
        }
        --a->tail;

#ifdef QK_ON_CONTEXT_SW
        if (p != pprev) {  /* changing threads? */
            QK_onContextSw(pprev, p); /* context-switch callback */
            pprev = p; /* update previous priority */
        }
#endif /* QK_ON_CONTEXT_SW */

        QF_INT_ENABLE(); /* unconditionally enable interrupts */

        QHSM_DISPATCH(&a->super); /* dispatch to the SM (execute RTC step) */

        QF_INT_DISABLE();


        if (a->nUsed == (uint8_t)0) { /* empty queue? */
            /* clear the ready bit */
            QF_readySet_ &= (uint_fast8_t)
                ~((uint_fast8_t)1 << (p - (uint_fast8_t)1));
        }

        /* find new highest-prio AO ready to run... */
#ifdef QF_LOG2
        p = (uint_fast8_t)QF_LOG2(QF_readySet_);
#else
        /* hi nibble used? */
        if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
            p = (uint_fast8_t)(
                    (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                    + (uint_fast8_t)4);
        }
        else { /* hi nibble of QF_readySet_ is zero */
            p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
        }
#endif

        /* is the new priority below the initial preemption threshold? */
        if (p <= pin) {
            p = (uint_fast8_t)0; /* active object not eligible */
        }
#ifdef QK_SCHED_LOCK
        /* below the scheduler ceiling? */
        else if (p <= (uint_fast8_t)QK_attr_.lockPrio) {
            p = (uint_fast8_t)0; /* active object not eligible */
        }
        else {
            Q_ASSERT_ID(710, p <= QF_maxActive_);
        }
#endif /* QK_SCHED_LOCK */
    } while (p != (uint_fast8_t)0);

    QK_attr_.actPrio = (uint8_t)pin; /* restore the active priority */

#ifdef QK_ON_CONTEXT_SW
    QK_onContextSw(pprev, pin); /* context-switch callback */
#endif /* QK_ON_CONTEXT_SW */
}


/**
* @file
* @brief Public QK-nano interface.
* @ingroup qkn
* @cond
******************************************************************************
* Last updated for version 6.6.0
* Last updated on  2019-11-20
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
#ifndef QKN_H
#define QKN_H

/****************************************************************************/
/*! attributes of the QK kernel */
typedef struct {
    uint8_t volatile actPrio;     /*!< prio of the active AO */
    uint8_t volatile nextPrio;    /*!< prio of the next AO to execute */
    uint8_t volatile lockPrio;    /*!< lock prio (0 == no-lock) */
    uint8_t volatile lockHolder;  /*!< prio of the lock holder */
    uint8_t volatile intNest;     /*!< ISR nesting level */
} QK_Attr;

/*! global attributes of the QK kernel */
extern QK_Attr QK_attr_;

/****************************************************************************/
/*! QK-nano scheduler finds the highest-priority thread ready to run */
uint_fast8_t QK_sched_(void);

/*! QK activator activates the next active object. The activated AO preempts
* the currently executing AOs.
*/
void QK_activate_(void);

#ifndef QF_ISR_NEST
    #define QK_SCHEDULE_() do { \
        if (QK_sched_() != (uint_fast8_t)0) { \
            QK_activate_(); \
        } \
    } while(0)
#else
    /*! The macro to invoke the QK scheduler in the QK_ISR_EXIT() */
    #define QK_SCHEDULE_() \
        if (QK_attr_.intNest == (uint_fast8_t)0) { \
            if (QK_sched_() != (uint_fast8_t)0) { \
                QK_activate_(); \
            } \
        } else ((void)0)

#endif

#ifdef QK_ON_CONTEXT_SW

    /*! QK-nano context switch callback (customized in BSPs for QK-nano) */
    /**
    * @description
    * This callback function provides a mechanism to perform additional
    * custom operations when QK switches context from one thread to
    * another.
    *
    * @param[in] prev   priority of the previous thread (active object)
    *                   (prev==0 means that @p prev was the QK idle loop)
    * @param[in] next   priority of the next thread (active object)
    *                   (next==0) means that @p next is the QK idle loop)
    * @attention
    * QK_onContextSw() is invoked with interrupts **disabled** and must also
    * return with interrupts **disabled**.
    *
    * @note
    * This callback is enabled by defining the macro #QK_ON_CONTEXT_SW.
    *
    * @include qkn_oncontextsw.c
    */
    void QK_onContextSw(uint_fast8_t prev, uint_fast8_t next);

#endif /* QK_ON_CONTEXT_SW */

/*! QK idle callback (customized in BSPs for QK)
*
* QK_onIdle() is called continuously by the QK-nano idle loop. This callback
* gives the application an opportunity to enter a power-saving CPU mode,
* or perform some other idle processing.
*
* @note QK_onIdle() is invoked with interrupts enabled and must also
* return with interrupts enabled. This is in contrast to the callback
* QF_onIdle(), which is used by the non-preemptive QF-nano scheduler.
*/
void QK_onIdle(void);


#ifdef QK_SCHED_LOCK

    /*! QK-nano Scheduler locking */

    /*! The scheduler lock status */
    typedef uint_fast16_t QSchedStatus;

    /*! QK Scheduler lock */
    QSchedStatus QK_schedLock(uint_fast8_t ceiling);

    /*! QK Scheduler unlock */
    void QK_schedUnlock(QSchedStatus stat);

#endif /* QK_SCHED_LOCK */

#endif /* QKN_H */

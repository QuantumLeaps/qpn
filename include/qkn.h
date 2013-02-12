/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 4.5.03
* Date of the Last Update:  Jan 16, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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
#ifndef qkn_h
#define qkn_h

/**
* \file
* \ingroup qepn qfn qkn
* \brief Public QP-nano interface.
*
* This header file must be included in all modules that use QP-nano with
* preemptive kernel option. Typically, this header file is included
* indirectly through the header file qpn.h.
*/

/** \brief Preprocessor switch for configuring preemptive real-time kernel
* (QK-nano). The macro is automatically defined by including the qkn.h file
* in qpn_port.h.
*
* \note If defined, this macro eliminates the code for the non-preemptive
* scheduler provided in QF-nano. Instead, the fully preemptive QK-nano
* real-time kernel is used.
*/
#define QK_PREEMPTIVE   1

/** \brief QK-nano initialization.
*
* This function is optional and might be used in some QK-nano ports.
* When the function is defined, it must called exactly once before any
* other QK function, typically from the BSP initialization function.
*/
void QK_init(void);

/** \brief Find the highest-priority task ready to run
*
* \note QK_schedPrio_() must be always called with interrupts locked
* and returns with interrupts locked.
*/
uint8_t QK_schedPrio_(void) Q_REENTRANT;

/** \brief QK-nano scheduler
*
* \note QK_sched_() must be always called with interrupts locked.
* The scheduler might unlock the interrupts internally, but always
* returns with interrupts locked.
*/
void QK_sched_(uint8_t p) Q_REENTRANT;

extern uint8_t volatile QK_currPrio_;              /**< current QK priority */

#ifndef QF_ISR_NEST
    /** \brief The macro to invoke the QK scheduler in the QK_ISR_EXIT()
    */
    #define QK_SCHEDULE_() do { \
        uint8_t p = QK_schedPrio_(); \
        if (p != (uint8_t)0) { \
            QK_sched_(p); \
        } \
    } while(0)
#else
    #define QK_SCHEDULE_() \
        if (QK_intNest_ == (uint8_t)0) { \
            uint8_t p = QK_schedPrio_(); \
            if (p != (uint8_t)0) { \
                QK_sched_(p); \
            } \
        } else ((void)0)

    extern uint8_t volatile QK_intNest_;       /**< interrupt nesting level */

#endif


/** \brief QK idle callback (customized in BSPs for QK)
*
* QK_onIdle() is called continuously by the QK-nano idle loop. This callback
* gives the application an opportunity to enter a power-saving CPU mode,
* or perform some other idle processing.
*
* \note QK_onIdle() is invoked with interrupts unlocked and must also
* return with interrupts unlocked. This is in contrast to the callback
* QF_onIdle(), which is used by the non-preemptive QF-nano scheduler.
*/
void QK_onIdle(void);

#ifdef QK_MUTEX

    /** \brief QK Mutex type.
    *
    * QMutex represents the priority-ceiling mutex available in QK.
    * \sa QK_mutexLock()
    * \sa QK_mutexUnlock()
    */
    typedef uint8_t QMutex;

    /** \brief QK priority-ceiling mutex lock
    *
    * Lock the QK scheduler up to the priority level \a prioCeiling.
    *
    * \note This function should be always paired with QK_mutexUnlock().
    * The code between QK_mutexLock() and QK_mutexUnlock() should be kept
    * to the minimum.
    */
    QMutex QK_mutexLock(uint8_t const prioCeiling);

    /** \brief QK priority-ceiling mutex unlock
    *
    * \note This function should be always paired with QK_mutexLock().
    * The code between QK_mutexLock() and QK_mutexUnlock() should be kept
    * to the minimum.
    */
    void QK_mutexUnlock(QMutex mutex);

#endif                                                          /* QK_MUTEX */

#endif                                                             /* qkn_h */

/**
* @file
* @brief Public QK-nano interface.
* @ingroup qkn
* @cond
******************************************************************************
* Last updated for version 5.6.1
* Last updated on  2015-12-30
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
******************************************************************************
* @endcond
*/
#ifndef qkn_h
#define qkn_h

/*! Preprocessor switch for configuring preemptive real-time kernel
* (QK-nano). The macro is automatically defined by including the qkn.h file
* in qpn_port.h.
*/
/**
* @note If defined, this macro eliminates the code for the non-preemptive
* scheduler provided in QF-nano. Instead, the fully preemptive QK-nano
* real-time kernel is used.
*/
#define QK_PREEMPTIVE   1

/*! QK-nano initialization.
*
* This function is optional and might be used in some QK-nano ports.
* When the function is defined, it must called exactly once before any
* other QK function, typically from the BSP initialization function.
*/
void QK_init(void);

/*! Find the highest-priority task ready to run
*
* @note QK_schedPrio_() must be always called with interrupts disabled
* and returns with interrupts disabled.
*/
uint_fast8_t QK_schedPrio_(void);

/*! QK-nano scheduler
*
* @note QK_sched_() must be always called with interrupts disaled.
* The scheduler might unlock the interrupts internally, but always
* returns with interrupts disabled.
*/
void QK_sched_(uint_fast8_t p);

extern uint_fast8_t volatile QK_currPrio_;  /*!< current QK priority */

#ifndef QF_ISR_NEST
    #define QK_SCHEDULE_() do { \
        uint_fast8_t p = QK_schedPrio_(); \
        if (p != (uint_fast8_t)0) { \
            QK_sched_(p); \
        } \
    } while(0)
#else
    /*! The macro to invoke the QK scheduler in the QK_ISR_EXIT() */
    #define QK_SCHEDULE_() \
        if (QK_intNest_ == (uint_fast8_t)0) { \
            uint_fast8_t p = QK_schedPrio_(); \
            if (p != (uint_fast8_t)0) { \
                QK_sched_(p); \
            } \
        } else ((void)0)

    extern uint_fast8_t volatile QK_intNest_; /*!< interrupt nesting level */

#endif


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


#ifdef QK_MUTEX

    /*! QK Mutex type.
    *
    * QMutex represents the priority-ceiling mutex available in QK.
    * @sa QK_mutexLock()
    * @sa QK_mutexUnlock()
    */
    typedef uint_fast8_t QMutex;

    /*! QK priority-ceiling mutex lock
    *
    * Lock the QK scheduler up to the priority level @a prioCeiling.
    *
    * @note This function should be always paired with QK_mutexUnlock().
    * The code between QK_mutexLock() and QK_mutexUnlock() should be kept
    * to the minimum.
    */
    QMutex QK_mutexLock(uint_fast8_t const prioCeiling);

    /*! QK priority-ceiling mutex unlock
    *
    * @note This function should be always paired with QK_mutexLock().
    * The code between QK_mutexLock() and QK_mutexUnlock() should be kept
    * to the minimum.
    */
    void QK_mutexUnlock(QMutex mutex);

    /*! current ceiling priority of a mutex */
    extern uint_fast8_t volatile QK_ceilingPrio_;

#endif /* QK_MUTEX */

#endif /* qkn_h */

/**
* @file
* @brief Public QK-nano interface.
* @ingroup qkn
* @cond
******************************************************************************
* Last updated for version 5.7.2
* Last updated on  2016-09-30
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

/****************************************************************************/
/*! attributes of the QK kernel */
typedef struct {
    uint_fast8_t volatile actPrio;  /*!< prio of the active AO */
    uint_fast8_t volatile nextPrio; /*!< prio of the next AO to execute */
#ifdef QK_MUTEX
    uint_fast8_t volatile lockPrio;   /*!< lock prio (0 == no-lock) */
    uint_fast8_t volatile lockHolder; /*!< prio of the lock holder */
#endif /* QK_MUTEX */
#ifdef QF_ISR_NEST
    uint_fast8_t volatile intNest;    /*!< ISR nesting level */
#endif /* QF_ISR_NEST */
} QK_Attr;

/*! global attributes of the QK kernel */
extern QK_Attr QK_attr_;

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

#endif /* QK_MUTEX */

#endif /* qkn_h */

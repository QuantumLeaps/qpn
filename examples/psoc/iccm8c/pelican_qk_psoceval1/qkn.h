/*****************************************************************************
* Product: QK-nano public interface
* Last Updated for Version: 4.0.02
* Date of the Last Update:  Aug 11, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
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

/** \brief The QK scheduler
*
* \note The QK scheduler must be always called with interrupts locked.
* The scheduler might unlock the interrupts internally, but always returns
* with interrupts locked.
*/
void QK_schedule_(void) Q_REENTRANT;

/** \brief QK idle callback (customized in BSPs for QK)
*
* QK_onIdle() is called continously by the QK-nano idle loop. This callback
* gives the application an opportunity to enter a power-saving CPU mode,
* or perform some other idle processing.
*
* \note QK_onIdle() is invoked with interrupts unlocked and must also
* return with interrupts unlocked. This is in contrast to the callback
* QF_onIdle(), which is used by the non-preemptive QF-nano scheduler.
*/
void QK_onIdle(void);

extern uint8_t volatile QK_currPrio_;              /**< current QK priority */

#ifndef QF_ISR_NEST
    /** \brief The macro to invoke the QK scheduler in the QK_ISR_EXIT()
    */
    #define QK_SCHEDULE_() \
        if (QF_readySet_ != (uint8_t)0) { \
            QK_schedule_(); \
        } else ((void)0)
#else
    #define QK_SCHEDULE_() \
        if ((QF_readySet_ != (uint8_t)0) && (QK_intNest_ == (uint8_t)0)) { \
            QK_schedule_(); \
        } else ((void)0)

    extern uint8_t volatile QK_intNest_;       /**< interrupt nesting level */
#endif


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
    QMutex QK_mutexLock(uint8_t prioCeiling);

    /** \brief QK priority-ceiling mutex unlock
    *
    * \note This function should be always paired with QK_mutexLock().
    * The code between QK_mutexLock() and QK_mutexUnlock() should be kept
    * to the minimum.
    */
    void QK_mutexUnlock(QMutex mutex);

#endif                                                          /* QK_MUTEX */

#endif                                                             /* qkn_h */

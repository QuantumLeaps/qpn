/**
* @file
* @brief QP-nano public interface including backwards-compatibility layer
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
#ifndef QPN_H
#define QPN_H

/**
* @description
* This header file must be included directly or indirectly
* in all application modules (*.c files) that use QP-nano.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "qpn_conf.h" /* QP-nano configuration file (from the application) */
#include "qfn_port.h" /* QF-nano port from the port directory */
#include "qassert.h"  /* embedded systems-friendly assertions */


/****************************************************************************/
/* QP API compatibility layer */
#ifndef QP_API_VERSION

/*! Macro that specifies the backwards compatibility with the
* QP-nano API version.
*/
/**
* @description
* For example, QP_API_VERSION=450 will cause generating the compatibility
* layer with QP-nano version 4.5.0 and newer, but not older than 4.5.0.
* QP_API_VERSION=0 causes generation of the compatibility layer "from the
* begining of time", which is the maximum backwards compatibilty. This is
* the default.@n
* @n
* Conversely, QP_API_VERSION=9999 means that no compatibility layer should
* be generated. This setting is useful for checking if an application
* complies with the latest QP-nano API.
*/
#define QP_API_VERSION 0

#endif  /* QP_API_VERSION */

/****************************************************************************/
#if (QP_API_VERSION < 580)

/*! @deprecated QMActive Control Block; instead use: ::QActiveCB. */
typedef QActiveCB QMActiveCB;

/*! @deprecated QMActive; instead use: ::QActive. */
typedef QActive QMActive;

/*! @deprecated QMsm state machine; instead use: ::QHsm. */
typedef QHsm QMsm;

/*! @deprecated QMActive constructor; instead use: QActive_ctor() */
#define QMActive_ctor QActive_ctor

/*! @deprecated QMsm state machine constructor; instead use: QHsm_ctor() */
#define QMsm_ctor     QHsm_ctor

/*! @deprecated execute the top-most initial transition in QMsm;
* instead use QHSM_INIT()
*/
#define QMSM_INIT(me_)      QHSM_INIT((me_))

/*! @deprecated dispatch an event to QMsm; instead use QHSM_DISPATCH() */
#define QMSM_DISPATCH(me_)  QHSM_DISPATCH((me_))

/*! @deprecated macro for cooperativew "Vanilla" kernel;
* instead use: QV_onIdle() */
#define QF_onIdle   QV_onIdle

#ifdef Q_TIMERSET
    #define QF_TIMEEVT_USAGE
#endif /* Q_TIMERSET */

/****************************************************************************/
#if (QP_API_VERSION < 500)

/*! @deprecated macro for odd 8-bit CPUs. */
#define Q_ROM_VAR

/*! @deprecated macro for odd 8-bit CPUs. */
#define Q_REENTRANT

/*! @deprecated execute the top-most initial transition in QMsm;
* instead use QMSM_INIT()
*/
#define QMsm_init(me_)      QHSM_INIT((me_))

/*! @deprecated dispatch an event to QMsm; instead use QMSM_DISPATCH() */
#define QMsm_dispatch(me_)  QMSM_DISPATCH((me_))

#if (Q_PARAM_SIZE != 0)
    /*! Deprecated API defined for backwards-compatibility */
    #define QActive_post(me_, sig_, par_) \
        QACTIVE_POST((me_), (sig_), (par_))

    /*! Deprecated API defined for backwards-compatibility */
    #define QActive_postISR(me_, sig_, par_) \
        QACTIVE_POST_ISR((me_), (sig_), (par_))
#else
    #define QActive_post(me_, sig_)    QACTIVE_POST((me_), (sig_))
    #define QActive_postISR(me_, sig_) QACTIVE_POST_ISR((me_), (sig_))
#endif

#ifdef QF_TIMEEVT_PERIODIC
    /*! Arming a one-shot time event for the default tick rate (rate 0);
    * instead use QActive_armX()
    */
    #define QActive_arm(me_, ticks_) \
        (QActive_armX((me_), (uint_fast8_t)0, (ticks_), 0U))
#else
    #define QActive_arm(me_, ticks_) \
        (QActive_armX((me_), (uint_fast8_t)0, (ticks_)))
#endif /* QF_TIMEEVT_PERIODIC */

/*! Disarming a time event for the default tick rate (rate 0);
* instead use QActive_disarmX()
*/
#define QActive_disarm(me_)     (QActive_disarmX((me_), (uint_fast8_t)0))


/****************************************************************************/
#if (QP_API_VERSION < 450)

/*! @deprecated QP-nano event type; instead use QEvt */
typedef QEvt QEvent;

#ifdef QF_INT_LOCK
    #define QF_INT_DISABLE()  QF_INT_LOCK()
#endif

#ifdef QF_INT_UNLOCK
    #define QF_INT_ENABLE()   QF_INT_UNLOCK()
#endif

#ifdef QF_ISR_NEST
    #ifdef QF_ISR_KEY_TYPE
        #define QF_ISR_STAT_TYPE  QF_ISR_KEY_TYPE
    #endif
    #ifdef QF_ISR_LOCK
        #define QF_ISR_DISABLE(stat_)  QF_ISR_LOCK(stat_)
    #endif
    #ifdef QF_ISR_UNLOCK
        #define QF_ISR_RESTORE(stat_)  QF_ISR_UNLOCK(stat_)
    #endif
#endif /* QF_ISR_NEST */

#if (QF_TIMEEVT_CTR_SIZE != 0)
    /*! System clock tick processing for the default tick rate 0;
    * instead use QF_tickXISR()
    */
    #define QF_tickISR() (QF_tickXISR((uint_fast8_t)0))

    /*! Legacy API defined for backwards compatibility */
    #define QF_tick()    (QF_tickXISR((uint_fast8_t)0))
#endif

#endif /* QP_API_VERSION < 450 */
#endif /* QP_API_VERSION < 500 */
#endif /* QP_API_VERSION < 540 */
/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* QPN_H */

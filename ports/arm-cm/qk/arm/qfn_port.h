/**
* @file
* @brief QF-nano port to Cortex-M, preemptive QK-nano kernel, ARM-KEIL toolset
* @cond
******************************************************************************
* Last Updated for Version: 6.0.4
* Date of the Last Update:  2018-01-10
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2005-2017 Quantum Leaps, LLC. All rights reserved.
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
* https://state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef qfn_port_h
#define qfn_port_h

/* QF interrupt disable/enable and log2()... */
#if (__TARGET_ARCH_THUMB == 3) /* Cortex-M0/M0+/M1(v6-M, v6S-M)?, see NOTE1 */

    #define QF_INT_DISABLE()    __disable_irq()
    #define QF_INT_ENABLE()     __enable_irq()

    /* QF-aware ISR priority for CMSIS function NVIC_SetPriority(), NOTE1 */
    #define QF_AWARE_ISR_CMSIS_PRI 0

#else /* Cortex-M3/M4/M7, see NOTE2 */

    /* Cortex-M3/M4/M7 alternative interrupt disabling with PRIMASK */
    #define QF_PRIMASK_DISABLE() __disable_irq()
    #define QF_PRIMASK_ENABLE()  __enable_irq()

    /* Cortex-M3/M4/M7 interrupt disabling policy, see NOTE3 */
    #define QF_INT_DISABLE() do { \
        QF_PRIMASK_DISABLE(); \
        QF_set_BASEPRI(QF_BASEPRI); \
        QF_PRIMASK_ENABLE(); \
    } while (0)
    #define QF_INT_ENABLE()      QF_set_BASEPRI(0U)

    /* BASEPRI threshold for "QF-aware" interrupts, see NOTE2 */
    #define QF_BASEPRI           0x3F

    /* QF-aware ISR priority for CMSIS function NVIC_SetPriority(), NOTE4 */
    #define QF_AWARE_ISR_CMSIS_PRI (QF_BASEPRI >> (8 - __NVIC_PRIO_BITS))

    /* Cortex-M3/M4/M7 provide the CLZ instruction for fast LOG2 */
    #define QF_LOG2(n_) ((uint_fast8_t)(32U - __clz(n_)))

    /* inline function for getting the BASEPRI register */
    static __inline unsigned QF_get_BASEPRI(void) {
        register unsigned volatile __regBasePri __asm("basepri");
        return __regBasePri;
    }

    /* inline function for setting the BASEPRI register */
    static __inline void QF_set_BASEPRI(unsigned basePri) {
        register unsigned volatile __regBasePri __asm("basepri");
        __regBasePri = basePri;
    }

#endif

/* interrupt nesting policy for ISR level (ISRs can nest) */
#define QF_ISR_NEST

/* QK-nano initialization and ISR entry/exit */
#define QK_INIT() QK_init()
void QK_init(void);

#define QK_ISR_ENTRY() ((void)0)
#define QK_ISR_EXIT()  do { \
    QF_INT_DISABLE(); \
    if (QK_sched_() != (uint_fast8_t)0) { \
        (*Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (uint32_t)(1U << 28)); \
    } \
    QF_INT_ENABLE(); \
} while (0)

#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qkn.h"        /* QK-nano platform-independent public interface */

/*****************************************************************************
* NOTE1:
* On Cortex-M0/M0+/M1 (architecture v6-M, v6S-M), the interrupt disabling
* policy uses the PRIMASK register to disable interrupts globally. The
* QF_AWARE_ISR_CMSIS_PRI level is zero, meaning that all interrupts are
* "kernel-aware".
*
* NOTE2:
* On Cortex-M3/M4/M7, the interrupt disable/enable policy uses the BASEPRI
* register (which is not implemented in Cortex-M0/M0+/M1) to disable
* interrupts only with priority lower than the level specified by the
* QF_BASEPRI macro. The interrupts with priorities above QF_BASEPRI (i.e.,
* with numerical priority values lower than QF_BASEPRI) are not disabled in
* this method. These free-running interrupts are not allowed to call any QF
* services, because QF is not aware of these interrupts. Coversely, only
* "QF-aware" interrupts, with numerical values of priorities eqal to or
* higher than QF_BASEPRI, can call QF services.
*
* NOTE3:
* The selective disabling of "QF-aware" interrupts with the BASEPRI register
* has a problem on ARM Cortex-M7 core r0p1 (see ARM-EPM-064408, errata
* 837070). The workaround recommended by ARM is to surround MSR BASEPRI with
* the CPSID i/CPSIE i pair, which is implemented in the QF_INT_DISABLE()
* macro. This workaround works also for Cortex-M3/M4 cores.
*
* NOTE4:
* The QF_AWARE_ISR_CMSIS_PRI macro is useful as an offset for enumerating
* the QF-aware interrupt priority levels in the applications, whereas the
* numerical values of the QF-aware interrupts must be greater or equal to
* QF_AWARE_ISR_CMSIS_PRI. The enumerated values based on
* QF_AWARE_ISR_CMSIS_PRI can be passed directly to the CMSIS function
* NVIC_SetPriority(), which shifts them by (8 - __NVIC_PRIO_BITS) into the
* correct bit position, while __NVIC_PRIO_BITS is the CMSIS macro defining
* the number of implemented priority bits in the NVIC. Please note that
* the macro QF_AWARE_ISR_CMSIS_PRI is intended only for applications and
* is not used inside the QF port, which remains generic and not dependent
* on the number of implemented priority bits in the NVIC.
*/

#endif /* qfn_port_h */

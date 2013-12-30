/*****************************************************************************
* Product: QF-nano port to ARM Cortex-M, QK-nano kernel, GNU compiler
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 08, 2013
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
#ifndef qfn_port_h
#define qfn_port_h

/* QF interrupt disable/enable and log2()... */
#ifdef ARM_ARCH_V6M                       /* Cortex-M0/M0+/M1 ?, see NOTE02 */

    #define QF_INT_DISABLE()        __asm volatile ("cpsid i")
    #define QF_INT_ENABLE()         __asm volatile ("cpsie i")

    /* QF-aware ISR priority for CMSIS function NVIC_SetPriority(), NOTE2   */
    #define QF_AWARE_ISR_CMSIS_PRI  0

#else                                       /* Cortex-M3/M4/M4F, see NOTE03 */

    #define QF_SET_BASEPRI(val_)    __asm volatile (\
        "movs r0,%0 \n\t" \
        "msr  BASEPRI,r0" :: "I" (val_) : "cc", "r0")
    #define QF_INT_DISABLE()        QF_SET_BASEPRI(QF_BASEPRI)
    #define QF_INT_ENABLE()         QF_SET_BASEPRI(0U)

                    /* BASEPRI limit for QF-aware ISR priorities, see NOTE4 */
    #define QF_BASEPRI  (0xFF >> 2)

    /* QF-aware ISR priority for CMSIS function NVIC_SetPriority(), NOTE5   */
    #define QF_AWARE_ISR_CMSIS_PRI  (QF_BASEPRI >> (8 - __NVIC_PRIO_BITS))

              /* Cortex-M3/M4/M4F provide the CLZ instruction for fast LOG2 */
    #define QF_LOG2(n_) ((uint8_t)(32U - __builtin_clz(n_)))
#endif
                                /* interrupt disabling policy for ISR level */
#define QF_ISR_NEST

                               /* QK-nano initialization and ISR entry/exit */
#define QK_INIT()        QK_init()
#define QK_ISR_ENTRY()   ((void)0)
#define QK_ISR_EXIT()    do { \
    if (QK_schedPrio_() != (uint8_t)0) { \
        *Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (uint32_t)0x10000000U; \
    } \
} while (0)

#include <stdint.h>   /* GNU ARM provides C99-standard exact-width integers */
#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

/*****************************************************************************
* NOTE2:
* On Cortex-M0/M0+/M1 (architecture v6-M, v6S-M), the interrupt disabling
* policy uses the PRIMASK register to disable interrupts globally. The
* QF_AWARE_ISR_CMSIS_PRI level is zero, meaning that all interrupts are
* "kernel-aware".
*
* NOTE3:
* On Cortex-M3/M4/M4F, the interrupt disable/enable policy uses the BASEPRI
* register (which is not implemented in Cortex-M0/M0+/M1) to disable
* interrupts only with priority lower than the level specified by the
* QF_BASEPRI macro. The interrupts with priorities above QF_BASEPRI (i.e.,
* with numerical priority values lower than QF_BASEPRI) are not disabled in
* this method. These free-running interrupts are not allowed to call any QF
* services, because QF is not aware of these interrupts. Coversely, only
* "QF-aware" interrupts, with numerical values of priorities eqal to or
* higher than QF_BASEPRI, can call QF services.
*
* NOTE4:
* For Cortex-M3/M4/M4F, the macro QF_BASEPRI leaves the top 2 priority bits
* empty for QF-aware interrupts. This is the highest-possible priority
* (lowest possible numerical value) for the guaranteed 3 priority bits
* implemented in the NVIC.
*
* NOTE5:
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

#endif                                                        /* qfn_port_h */

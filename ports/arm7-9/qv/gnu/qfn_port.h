/**
* @file
* @brief QF-nano port ARM7-9, QV-nano kernel, GNU-ARM toolset
* @cond
******************************************************************************
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-04-08
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef QFN_PORT_H
#define QFN_PORT_H

/* fast unconditional interrupt disabling/enabling for ARM state, NOTE1 */
#define QF_INT_DISABLE()   \
    __asm volatile ("MSR cpsr_c,#(0x1F | 0x80)" ::: "cc")

#define QF_INT_ENABLE() \
    __asm volatile ("MSR cpsr_c,#(0x1F)" ::: "cc")


/* interrupt disabling policy for ISR level for ARM state */
#define QF_ISR_NEST

#define QF_ISR_STAT_TYPE    unsigned long

#define QF_ISR_DISABLE(stat_) do { \
    __asm volatile ("MRS %0,cpsr" : "=r" (stat_) :: "cc"); \
    QF_INT_DISABLE(); \
} while (0)

#define QF_ISR_RESTORE(stat_) __set_CPSR(stat_)

/* QV-nano initialization and ISR entry/exit */
void QV_irq(void);
void BSP_irq(void);
/* void BSP_fiq(void);  see NOTE1 */


#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qvn.h"        /* QV-nano platform-independent public interface */

void QF_reset(void);
void QF_undef(void);
void QF_swi(void);
void QF_pAbort(void);
void QF_dAbort(void);
void QF_reserved(void);
void QF_unused(void);

/*****************************************************************************
* NOTE1:
* The disabling/enabling of interrutps is only defined for the ARM state,
* which means that any code that disables/enables interrutps must be compiled
* to ARM. The policy is to disable only the IRQ and NOT to disable the FIQ,
* which means that FIQ is a "QF-unaware" from the kernel perspective. This means
* that FIQ has "zero latency", but it also means that FIQ *cannot* call any
* QP services. Specifically FIQ cannot post or publish events.
*/

#endif /* QFN_PORT_H */

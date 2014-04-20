/*****************************************************************************
* Product: QF-nano port to ARM Cortex-M, QK-nano, TI_ARM compiler
* Last updated for version 5.3.0
* Last updated on  2014-04-14
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
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
*****************************************************************************/
#ifndef qfn_port_h
#define qfn_port_h

/* interrupt locking policy for task level */
#define QF_INT_DISABLE()        asm(" CPSID I")
#define QF_INT_ENABLE()         asm(" CPSIE I")

/* interrupt disablgin policy for ISR level, see NOTE01 */
#define QF_ISR_NEST

/* QK-nano initialization and ISR entry/exit */
#define QK_INIT()        QK_init()
#define QK_ISR_ENTRY()   ((void)0)
#define QK_ISR_EXIT()    do { \
    if (QK_schedPrio_() != (uint8_t)0) { \
        *Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (uint32_t)0x10000000U; \
    } \
} while (0)

/* is the target M3 or M4? (M0/M0+/M1 don't support CLZ) */
#if (defined __TI_TMS470_V7M3__) || (defined __TI_TMS470_V7M4__)
    /* the intrinsic function _norm() generates the CLZ instruction */
    #define QF_LOG2(n_) ((uint8_t)(32U - _norm(n_)))
#endif

#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qkn.h"        /* QK-nano platform-independent public interface */
#include "qassert.h"    /* QP-nano assertions header file */

/*****************************************************************************
* NOTE01:
* Cortex-M does not lock interrupts upon the interrupt entry, so interrupts
* can nest. You can prevent interrupts from nesting by setting the priorities
* of all interrupts to the same level. In this case, you could undefine
* the QF_ISR_NEST macro. Either way, you don't need to change the macros
* QK_ISR_ENTRY()/QK_ISR_EXIT().
*
* NOTE02:
* The QK_ISR_EXIT() macro triggers the Cortex-M PendSV exception, which
* in turn causes QK_schedule_() to run. The PendSV exception is prioritized
* lower than all interrupts in the system, so the NVIC performs tail-chaining
* to PendSV only after all nested interrupts are handled. This means that
* in Cortex-M you don't need to increment and decrement the nesting level
* QK_intNest_, because it doesn't matter for the tail-chaining mechanism.
*/

#endif /* qfn_port_h */

/**
* @file
* @brief QF-nano port, PIC24/dsPIC, QK-nano kernel, XC16 compiler
* @ingroup ports
* @cond
******************************************************************************
* Last updated for version 6.5.1
* Last updated on  2019-06-10
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef QFN_PORT_H
#define QFN_PORT_H

/* QF-nano interrupt disable/enable, see NOTE01 */
#define QF_INT_DISABLE()        __builtin_disi(0x3FFFU)
#define QF_INT_ENABLE()         __builtin_disi(0x0000U)

/* ISR-level interrupt locking policy for PIC24/dsPIC, see NOTE02 */
#define QF_ISR_NEST

/* QK-nano interrupt entry/exit */
#define QK_ISR(psv_) \
void  __attribute__((__interrupt__(__preprologue__( \
    "push   RCOUNT      \n" \
    "push.d w0          \n" \
    "mov.w  [w15-8],w0  \n" \
    "lsr.w  w0,#13,w1   \n" \
    "mov.w  #1,w0       \n" \
    "sl     w0,w1,w0    \n" \
    "ior.b  _QK_IPL_set_\n" \
    "bra    .+6         ")) \
    , psv_))

#define QK_ISR_EXIT() do { \
    register uint16_t this_sr; \
    __asm__ volatile ( \
        "mov.w  SR,%0    \n" \
        "lsr    %0,#5,w0 \n" \
        "and.w  w0,#7,w0 \n" \
        "mov.w  #1,w1    \n" \
        "sl     w1,w0,w0 \n" \
        "ior.b  #1,w0    \n" \
        "com.b  w0,w0    \n" \
        "disi   #0x3FFF  \n" \
        "and.b  _QK_IPL_set_" : "=r"(this_sr) : : "w0", "w1"); \
    if (QK_IPL_set_ == 0) { \
        if (QK_sched_() != (uint_fast8_t)0) { \
            __asm__ volatile ("clr.b SR"); \
            QK_activate_(); \
            __asm__ volatile ("mov.w %0,SR" : : "r"(this_sr)); \
        } \
    } \
    __asm__ volatile ("disi #0x0000"); \
} while (0);

#include <stdint.h>      /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>     /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"        /* QEP-nano platform-independent public interface */
#include "qfn.h"         /* QF-nano  platform-independent public interface */
#include "qkn.h"         /* QK-nano  platform-independent public interface */

/* set of active interrupt priority levels (IPLs) */
extern uint8_t volatile QK_IPL_set_;

/*****************************************************************************
* NOTE01:
* The DISI (disable interrupts) instruction is used for very fast,
* unconditional locking and unlocking of interrupts. The DISI #0x3FFF
* instruction disables interrupts for 16383 instruction cycles, which is
* much longer than any critical section in QP-nano. The DISI #0 instruction
* is then used to unconditionally unlock the interrupts at the end of the
* critical section. The DISI instruction only disables interrupts with
* priority levels 1-6. Priority level 7 interrupts and all trap events still
* have the ability to interrupt the CPU when the DISI instruction is active.
* This means that from the perspective of QP-nano, the level 7 interrupts are
* treated as non-maskable interrupts (NMIs). Such non-maskable interrupts
* _cannot_ call any QP-nano services. In particular, they cannot post events.
*
* CAUTION: This QP-nano port assumes that interrupt nesting is _enabled_,
* which is the default in the PIC24/dsPIC processors. Interrupt nesting should
* never be disabled by setting the NSTDIS control bit (INTCON1<15>). If you
* don't want interrupts to nest, you can always prioritize them at the same
* level. For example, the default priority level for all interrupts is 4 out
* of reset. If you don't change this level for any interrupt the nesting of
* interrupt will not occur.
*
* NOTE02:
* The ISR-level interrupt policy allows interrupt nesting. The QF_ISR_KEY_TYPE
* is _not_ defined, which means that the ISRs will use the task-level
* interrupt locking policy. This is possible, because PIC24/dsPIC CPU does
* _not_ lock interrupts upon entry to the ISR, so the ISR does not constitute
* critical section and the simple critical section incapable of nesting is
* adequate.
*
* CAUTION: This _preemptive_ QK-nano port requires writing ISRs in assembly,
* as shown in the file isr.s. Unfortunately, the ISRs that the C30 compiler
* is capable of generating are not adequate for the QK-nano kernel.
*/

#endif /* QFN_PORT_H */


/*****************************************************************************
* QP-nano port, PIC24/dsPIC, MPLABX-XC16 compiler, Vanilla kernel
* Last Updated for Version: 4.5.04
* Date of the Last Update:  Feb 20, 2013
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
#ifndef qpn_port_h
#define qpn_port_h

#define Q_NFSM
#define Q_PARAM_SIZE            2
#define QF_TIMEEVT_CTR_SIZE     2


/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2U

                         /* task-level interrupt nesting policy, see NOTE01 */
#define QF_INT_DISABLE()        __builtin_disi(0x3FFFU)
#define QF_INT_ENABLE()         __builtin_disi(0x0000U)

          /* ISR-level interrupt locking policy for PIC24/dsPIC, see NOTE02 */
#define QF_ISR_NEST

                           /* fast log-base-2 with FBCL instruction, NOTE03 */
#define QF_LOG2(n_) ((uint8_t)(15 + __builtin_fbcl(n_)))

             /* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
#include <stdint.h>

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */


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
* interrupt locking policy. This is possible, because PIC245/dsPIC CPU does
* _not_ lock interrupts upon entry to the ISR, so the ISR does not constitute
* critical section and the simple critical section incapable of nesting is
* adequate.
*
* NOTE03:
* The FBCL instruction (Find First Bit Change Left) determines the exponent
* of a value by detecting the first bit change starting from the value’s sign
* bit and working towards the LSB. Since the PIC24/dsPIC’s barrel shifter
* uses negative values to specify a left shift, the FBCL instruction returns
* the negated exponent of a value. This value added to 15 gives the log-2.
*/

#endif                                                        /* qpn_port_h */


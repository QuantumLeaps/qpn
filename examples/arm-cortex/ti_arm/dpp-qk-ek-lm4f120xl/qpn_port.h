/*****************************************************************************
* Product: QP-nano port for DPP example, QK-nano, TI CCS compiler
* Last Updated for Version: 4.5.04
* Date of the Last Update:  Jan 27, 2013
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
#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           6

                         /* interrupt locking policy for task level, NOTE01 */
#define QF_INT_DISABLE()        asm(" CPSID I")
#define QF_INT_ENABLE()         asm(" CPSIE I")

                                  /* interrupt locking policy for ISR level */
#define QF_ISR_NEST
                                  /* QK-nano ISR entry and exit, see NOTE02 */
#define QK_ISR_ENTRY()   ((void)0)
#define QK_ISR_EXIT()    (*((uint32_t volatile *)0xE000ED04U) = 0x10000000U)

#include <stdint.h>       /* CCS provides C99-standard exact-width integers */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

/*****************************************************************************
* NOTE01:
* ARM Cortex-M does not disable interrupts upon the interrupt entry, so
* interrupts can nest by default. You can prevent interrupts from nesting by
* setting the priorities of all interrupts to the same level. In any case,
* you should not call any QP-nano services with interrupts disabled.
*/

#endif                                                        /* qpn_port_h */

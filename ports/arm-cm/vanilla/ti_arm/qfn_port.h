/*****************************************************************************
* Product: QF-nano port to ARM Cortex-M, Vanilla kernel, TI CCS compiler
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 11, 2013
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

                         /* interrupt locking policy for task level, NOTE01 */
#define QF_INT_DISABLE()        asm(" CPSID I")
#define QF_INT_ENABLE()         asm(" CPSIE I")

                                  /* interrupt locking policy for ISR level */
#define QF_ISR_NEST

#include <stdint.h>       /* CCS provides C99-standard exact-width integers */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */

/*****************************************************************************
* NOTE01:
* ARM Cortex-M does not disable interrupts upon the interrupt entry, so
* interrupts can nest by default. You can prevent interrupts from nesting by
* setting the priorities of all interrupts to the same level. In any case,
* you should not call any QP-nano services with interrupts disabled.
*/

#endif                                                        /* qfn_port_h */

/*****************************************************************************
* Product: PELICAN crossing example, Vainlla kernel, GNU compiler
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Nov 07, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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

#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     4

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2

                               /* interrupt locking policy for GNU compiler */
#define QF_INT_DISABLE()        __asm volatile ("cpsid i")
#define QF_INT_ENABLE()         __asm volatile ("cpsie i")

                      /* interrupt locking policy for ISR level, see NOTE01 */
#define QF_ISR_NEST

#include <stdint.h>  /* standard exact-width integer types supported by GNU */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */

/*****************************************************************************
* NOTE01:
* ARM Cortex does not lock interrupts upon the interrupt entry, so interrupts
* can nest by default. You can prevent interrupts from nesting by setting the
* priorities of all interrupts to the same level. In any case, you should not
* call any QP-nano services with interrupts locked.
*/

#endif                                                        /* qpn_port_h */

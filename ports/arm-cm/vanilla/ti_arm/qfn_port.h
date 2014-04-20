/*****************************************************************************
* Product: QF-nano port to ARM Cortex-M, Vanilla kernel, TI CCS compiler
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

/* interrupt disabling policy for task level, NOTE01 */
#define QF_INT_DISABLE()        asm(" CPSID I")
#define QF_INT_ENABLE()         asm(" CPSIE I")

/* interrupt locking policy for ISR level */
#define QF_ISR_NEST

#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qassert.h"    /* QP-nano assertions header file */

/*****************************************************************************
* NOTE01:
* ARM Cortex-M does not disable interrupts upon the interrupt entry, so
* interrupts can nest by default. You can prevent interrupts from nesting by
* setting the priorities of all interrupts to the same level. In any case,
* you should not call any QP-nano services with interrupts disabled.
*/

#endif /* qfn_port_h */

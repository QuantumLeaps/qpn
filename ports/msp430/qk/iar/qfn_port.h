/**
* @file
* @brief QF-nano port to MSP430, preemptive QK-nano kernel, IAR-430 toolset
* @cond
******************************************************************************
* Last Updated for Version: 6.8.0
* Date of the Last Update:  2020-03-31
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2020 Quantum Leaps, LLC. All rights reserved.
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
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
******************************************************************************
* @endcond
*/
#ifndef QFN_PORT_H
#define QFN_PORT_H

/* QF-nano interrupt disable/enable... */
#define QF_INT_DISABLE() __disable_interrupt()
#define QF_INT_ENABLE()  __enable_interrupt()

/* QF-nano interrupt disabling policy for interrupt level */
/*#define QF_ISR_NEST*/  /* nesting of ISRs not allowed */

/* QK-nano interrupt entry/exit */
#define QK_ISR_ENTRY()   ((void)0)
#define QK_ISR_EXIT()    do { \
    if (QK_sched_() != 0U) {  \
        QK_activate_();       \
    }                         \
} while (false)


#include <intrinsics.h>  /* IAR intrinsic functions */

#include <stdint.h>      /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>     /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"        /* QEP-nano platform-independent public interface */
#include "qfn.h"         /* QF-nano  platform-independent public interface */
#include "qkn.h"         /* QK-nano  platform-independent public interface */

#endif /* QFN_PORT_H */

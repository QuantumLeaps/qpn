/**
* @file
* @brief QF-nano port AVR ATmega, preemptive QK-nano kernel, GNU-AVR toolset
* @cond
******************************************************************************
* Last Updated for Version: 6.3.3
* Date of the Last Update:  2018-06-30
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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

/* GNU-AVR extended keyword '__flash' allocates const objects to ROM */
#define Q_ROM            __flash

/* QF-nano interrupt disable/enable... */
#define QF_INT_DISABLE() __asm__ __volatile__ ("cli" ::)
#define QF_INT_ENABLE()  __asm__ __volatile__ ("sei" ::)

/* QF-nano interrupt disabling policy for interrupt level */
/*#define QF_ISR_NEST*/  /* nesting of ISRs not allowed */

/* QK-nano interrupt entry/exit */
#define QK_ISR_ENTRY()   ((void)0)
#define QK_ISR_EXIT()    do { \
    if (QK_sched_() != (uint_fast8_t)0) { \
        QK_activate_(); \
    } \
} while (0)

/* QK sleep mode */
#define QK_CPU_SLEEP()   do { \
    __asm__ __volatile__ ("sleep" ::); \
    SMCR = 0U; \
} while (0)

/* QF CPU reset for AVR */
#define QF_RESET()       __asm__ __volatile__ ("jmp 0x0000" ::)

#include <avr/pgmspace.h>    /* accessing data in program memory (PROGMEM) */
#include <avr/interrupt.h>   /* AVR interrupt support */
#include <avr/io.h>          /* SREG/SMCR definitions */

#include <stdint.h>      /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>     /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"        /* QEP-nano platform-independent public interface */
#include "qfn.h"         /* QF-nano  platform-independent public interface */
#include "qkn.h"         /* QK-nano  platform-independent public interface */

#endif /* QFN_PORT_H */

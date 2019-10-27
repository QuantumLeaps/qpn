/**
* @file
* @brief QF-nano port for Lint, cooperative QV kernel, generic C compiler
* @cond
******************************************************************************
* Last updated for version 5.8.1
* Last updated on  2016-12-16
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
* http://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef QFN_PORT_H
#define QFN_PORT_H

/**
* @description
* This header file contains the port and customization of QP-nano to
* the specific CPU and compiler. Typically, this header file includes
* the platform-independent header file qpn.h.
*/

#define QF_INT_DISABLE() int_disable()
#define QF_INT_ENABLE()  int_enable()

/*! Macro that specifies QV-nano initialization */
/**
* \description
* Some QV-nano ports might need initialization (e.g., seeting the priorities
* of exceptions PendSV and SVCall in ARM Cortex-M port). The QV_INIT() macro
* in this case calls the initialization defined in assembly.
*
* \note The macro QV_INIT() can be left undefined.
*/
#define QV_INIT()  QV_init()
void QV_init(void);

/*! Macro to put the CPU to sleep safely in the cooperative QV kernel
* (inside QV_onIdle()).
*/
/**
* @description
* This macro is provided in some QP ports for the QV cooperative kernel
* and in general it depends on the interrupt disabling policy.
*
* @note The provided code is just an example (for ARM Cortex-M).
*/
#define QV_CPU_SLEEP() do { \
    __disable_interrupt(); \
    QF_INT_ENABLE(); \
    __WFI(); \
    __enable_interrupt(); \
} while (0)

void int_disable(void); /*!< example of a function to disable interrupts */
void int_enable(void);  /*!< example of a function to enable interrupts */

#include <stdint.h>    /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>   /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"      /* QEP-nano platform-independent public interface */
#include "qfn.h"       /* QF-nano platform-independent public interface */
#include "qvn.h"       /* QV-nano platform-independent public interface */

#endif /* QFN_PORT_H */

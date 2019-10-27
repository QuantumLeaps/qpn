/**
* @file
* @brief QF-nano port for Lint, preemptive QK kernel, generic C compiler
* @cond
******************************************************************************
* Last updated for version 6.1.1
* Last updated on  2018-02-18
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
* \endcond
*/
#ifndef QFN_PORT_H
#define QFN_PORT_H

/**
* \description
* This header file contains the port and customization of QP-nano to the
* specific CPU and compiler. The port header file is included in the main qpn.h
* header file.
*/

/*! Macro to specify compiler-specific directive for placing a
* constant object in ROM.
*/
/**
* \description
* Many compilers for 8-bit Harvard-architecture MCUs provide non-stanard
* extensions to support placement of objects in different memories.
* In order to conserve the precious RAM, QP-nano uses the Q_ROM macro for
* all constant objects that can be allocated in ROM.
*
* To override the following empty definition, you need to define the
* Q_ROM macro in the qpn_port.h header file. Some examples of valid
* Q_ROM macro definitions are: __code (IAR 8051 compiler), code (Keil
* 8051 compiler), PROGMEM (gcc for AVR), __flash (IAR for AVR).
*/
#define Q_ROM

/*! Macro to access a byte allocated in ROM */
/**
* \description
* Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
* not generate correct code for accessing data allocated in the program
* space (ROM). The workaround for such compilers is to explicitly add
* assembly code to access each data element allocated in the program
* space. The macro Q_ROM_BYTE() retrieves a byte from the given ROM
* address.
*
* The Q_ROM_BYTE() macro should be defined in the qpn_port.h header file
* for each compiler that cannot handle correctly data allocated in ROM
* (such as the gcc). If the macro is left undefined, the default
* definition simply returns the argument and lets the compiler synthesize
* the correct code.
*/
#define Q_ROM_BYTE(rom_var_) (rom_var_)

/*! Macro to access a pointer allocated in ROM */
/**
* \description
* Some compilers for Harvard-architecture MCUs, such as the gcc for AVR,
* do not generate correct code for accessing data allocated in the program
* space. The workaround for such compilers is to explictly add assembly code
* to access each data element allocated in the program space. The macro
* Q_ROM_PTR() retrieves an object-pointer from the given ROM address. Please
* note that the pointer can be pointing to the object in RAM or ROM.
*
* The Q_ROM_PTR() macro should be defined in the qpn_port.h header file
* for each compiler that cannot handle correctly data allocated in ROM
* (such as the gcc). If the macro is left undefined, the default definition
* simply returns the argument and lets the compiler synthesize the correct
* code.
*/
#define Q_ROM_PTR(rom_var_) (rom_var_)

/*! The macro defines the mechanism of locking the interrupts. */
/**
* \note This is just an example of #QF_INT_DISABLE. You need to define
* the macro appropriately for the CPU/compiler you're using.
*
* \note The #QF_INT_DISABLE macro must be always paired with #QF_INT_ENABLE.
* You should keep the code between these two macros to the minimum.
*/
#define QF_INT_DISABLE() int_disable()

/*! The macro defines the mechanism of unlocking the interrupts. */
/**
* \description
* \note This is just an example of #QF_INT_ENABLE. You need to define
* the macro appropriately for the CPU/compiler you're using.
*
* \note The #QF_INT_ENABLE macro must be always paired with #QF_INT_DISABLE.
* You should keep the code between these two macros to the minimum.
*/
#define QF_INT_ENABLE()  int_enable()

/*! QK-nano ISR entry
*
* an example only
*/
#define QK_ISR_ENTRY()   ((void)0)

/*! QK-nano ISR exit */
#define QK_ISR_EXIT()    (*((uint32_t volatile *)0xE000ED04U) = 0x10000000U)

/*! Macro that specifies whether QK-nano ISR nesting is allowed */
#define QF_ISR_NEST

/*! Macro that specifies QK-nano initialization */
/**
* \description
* Some QK-nano ports might need initialization (e.g., seeting the priorities
* of exceptions PendSV and SVCall in ARM Cortex-M port). The QK_INIT() macro
* in this case calls the initialization defined in assembly.
*
* \note The macro QK_INIT() can be left undefined.
*/
#define QK_INIT()  QK_init()
void QK_init(void);

/*! Macro that enables context-switch callback in QK-nano */
#define QK_ON_CONTEXT_SW

void int_disable(void); /*!< example of a function to disable interrupts */
void int_enable(void);  /*!< example of a function to enable interrupts */

#include <stdint.h>    /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>   /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"      /* QEP-nano platform-independent public interface */
#include "qfn.h"       /* QF-nano platform-independent public interface */
#include "qkn.h"       /* QK-nano platform-independent public interface */

#endif /* QFN_PORT_H */

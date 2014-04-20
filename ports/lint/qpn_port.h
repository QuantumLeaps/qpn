/**
* \file
* \brief QP-nano port for Lint (generic C compiler).
* \ingroup qepn, qfn, qkn
* \cond
******************************************************************************
* Product: QF-nano
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
******************************************************************************
* \endcond
*/
#ifndef qpn_port_h
#define qpn_port_h

/**
* \description
* This header file contains the port and customization of QP-nano to
* the specific CPU and compiler. Typically, this header file includes
* the platform-independent header file qpn.h.
*/

/*! The maximum number of active objects in the application. */
/**
* \description
* This macro should be defined in the QP-nano port header file qpn_port.h
* and should be in range of 1..8, inclusive. In QF-nano, the value of
* QF_MAX_ACTIVE does not incur bigger RAM or ROM overhead, but if you
* use less than 5 active objects, the algorithm used in QF-nano scheduler
* is slightly more efficient.
*/
#define QF_MAX_ACTIVE           8

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
#define Q_ROM_BYTE(rom_var_)   (rom_var_)

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
#define Q_ROM_PTR(rom_var_)    (rom_var_)

/*! The size (in bytes) of the single scalar parameter representation
* in the QEvent struct. Valid values: none (0), 1, 2, or 4; default none (0).
*/
/**
* \description
* This macro can be defined in the QP-nano port header file qpn_port.h to
* configure the parameter of Events. If the macro is not defined, the default
* of no event parameter will be chosen. The valid Q_PARAM_SIZE values of 1, 2,
* or 4, correspond to event parameters of uint8_t, uint16_t, and uint32_t,
* respectively.
*
* \sa ::QEvent, #Q_PAR(), QF_post(), QF_postNoLock()
*/
#define Q_PARAM_SIZE            1

/*! The size (in bytes) of the time event-counter representation in
* the QActive struct. Valid values: none (0), 1, 2, or 4; default none (0).
*/
/**
* \description
* This macro can be defined in the QP-nano port header file qpn_port.h to
* configure the internal tick counters of Time Events. If the macro is not
* defined, the default of no time event counter will be chosen. The valid
* QF_TIMEEVT_CTR_SIZE values of 1, 2, or 4, correspond to tick counters of
* uint8_t, uint16_t, and uint32_t, respectively. The tick counter
* representation determines the dynamic range of time delays that a
* Time Event can handle.
*
* \sa #QTimeEvtCtr
*/
#define QF_TIMEEVT_CTR_SIZE     2

/*! If defined, this macro eliminates the code supporting the simpler
* non-hierarchical FSMs.
*/
/*#define Q_NFSM */

/*! Configuration switch to enable/disable update of the QF timer set. */
/**
* \description
* The QF-nano timer set keeps track of the armed time event set.
* The main use of the timer set is to quickly determine that all time
* events are disarmed.
*/
#define Q_TIMERSET              1

/*! The macro defines the mechanism of locking the interrupts. */
/**
* \note This is just an example of #QF_INT_DISABLE. You need to define
* the macro appropriately for the CPU/compiler you're using.
*
* \note The #QF_INT_DISABLE macro must be always paired with #QF_INT_ENABLE.
* You should keep the code between these two macros to the minimum.
*/
#define QF_INT_DISABLE()        int_disable()


/*! The macro defines the mechanism of unlocking the interrupts. */
/**
* \description
* \note This is just an example of #QF_INT_ENABLE. You need to define
* the macro appropriately for the CPU/compiler you're using.
*
* \note The #QF_INT_ENABLE macro must be always paired with #QF_INT_DISABLE.
* You should keep the code between these two macros to the minimum.
*/
#define QF_INT_ENABLE()         int_enable()

/*! Macro to put the CPU to sleep safely in the cooperative
* Vanilla kernel (inside QF_idle()).
*/
/**
* \description
* This macro is provided in some QP ports for the Vanilla kernel and
* in general it depends on the interrupt disabling policy.
*
* \note The provided code is just an example (for ARM Cortex-M).
*/
#define QF_CPU_SLEEP() do { \
    __disable_interrupt(); \
    QF_INT_ENABLE(); \
    __WFI(); \
    __enable_interrupt(); \
} while (0)

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
#define QK_INIT()        QK_init()

void int_disable(void); /*!< example of a function to disable interrupts */
void int_enable(void);  /*!< example of a function to enable interrupts */

#include <stdint.h>    /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>   /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"      /* QEP-nano platform-independent public interface */
#include "qfn.h"       /* QF-nano platform-independent public interface */
#include "qkn.h"       /* QK-nano platform-independent public interface */

#endif /* qpn_port_h */

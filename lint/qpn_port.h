/*****************************************************************************
* Product: QP-nano port to Lint
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Feb 29, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

/**
* \file
* \ingroup qepn qfn
* \brief QP-nano port.
*
* This header file contains the port and customization of QP-nano to
* the specific CPU and compiler. Typically, this header file includes
* the platform-independent header file qpn.h.
*/


/** \brief Macro to specify compiler-specific directive for placing a
* constant object in ROM.
*
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

/** \brief Macro to specify compiler-specific directive for accessing a
* constant object in ROM.
*
* Many compilers for 8-bit MCUs provide different size pointers for
* accessing objects in various memories. Constant objects allocated
* in ROM (see #Q_ROM macro) often mandate the use of specific-size
* pointers (e.g., far pointers) to get access to ROM objects. The
* macro Q_ROM_VAR specifies the kind of the pointer to be used to access
* the ROM objects.
*
* To override the following empty definition, you need to define the
* Q_ROM_VAR macro in the qpn_port.h header file. An example of valid
* Q_ROM_VAR macro definition is: __far (Freescale HC(S)08 compiler).
*/
#define Q_ROM_VAR

/** \brief Macro to specify compiler-specific directive for generating
* reentrant function.
*
* Some compilers for 8-bit MCUs provide, most notably the Keil C51
* compiler for 8051, don't generate ANSI-C compliant reentrant functions
* by default, due to the limited hardware architecture. These compilers
* allow to dedicate specific functions to be reentrant with a special
* extended keyword (such as "reentrant" for Keil C51). The macro
* Q_REENTRANT is defined to nothing by default, to work with ANSI-C
* compiliant compilers, but can be defined to "reentrant" to work with
* Keil C51 and perhpas other compilers.
*/
#define Q_REENTRANT

/** \brief Macro to access a byte allocated in ROM
*
* Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
* not generate correct code for accessing data allocated in the program
* space (ROM). The workaround for such compilers is to explictly add
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

/** \brief Macro to access a pointer allocated in ROM
*
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

/** \brief The size (in bytes) of the single scalar parameter representation
* in the QEvent struct. Valid values: none (0), 1, 2, or 4; default none (0).
*
* This macro can be defined in the QP-nano port header file qpn_port.h to
* configure the parameter of Events. If the macro is not defined, the default
* of no event parameter will be chosen. The valid Q_PARAM_SIZE values of 1, 2,
* or 4, correspond to event parameters of uint8_t, uint16_t, and uint32_t,
* respectively.
*
* \sa ::QEvent, #Q_PAR(), QF_post(), QF_postNoLock()
*/
#define Q_PARAM_SIZE            1

/** \brief The size (in bytes) of the time event-counter representation in
* the QActive struct. Valid values: none (0), 1, 2, or 4; default none (0).
*
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

/** \brief If defined, this macro eliminates the code supporting the simpler
* non-hierarchical FSMs.
*/
/*#define Q_NFSM */

/** \brief If defined, this macro sets the superclass for QActive to QFsm.
* Otherwise, if the macro is undefined, the default superclass for QActive
* is QHsm.
*
* Defining QF_FSM_ACTIVE can be advantageous in resource-constraint
* applications because avoiding QHsm functions saves about 1KB of code space
* (ROM), and QFsm functions are slightly faster than the more general QHsm
* functions.
*
* Note:
* Once you define QF_FSM_ACTIVE, you must consistently use only FSMs in your
* own application code.
*/
#define QF_FSM_ACTIVE

/** \brief The maximum number of active objects in the application.
*
* This macro should be defined in the QP-nano port header file qpn_port.h
* and should be in range of 1..8, inclusive. In QF-nano, the value of
* QF_MAX_ACTIVE does not incurr bigger RAM or ROM overhead, but if you
* use less than 5 active objects, the algorithm used in QF-nano scheduler
* is slightly more efficient.
*/
#define QF_MAX_ACTIVE           8


/** \brief The macro defines the mechanism of locking the interrupts.
*
* \note This is just an example of #QF_INT_DISABLE. You need to define
* the macro appropriately for the CPU/compiler you're using.
*
* \note The #QF_INT_DISABLE macro must be always paired with #QF_INT_ENABLE.
* You should keep the code between these two macros to the minimum.
*/
#define QF_INT_DISABLE()        int_disable()

/** \brief The macro defines the mechanism of unlocking the interrupts.
*
* \note This is just an example of #QF_INT_ENABLE. You need to define
* the macro appropriately for the CPU/compiler you're using.
*
* \note The #QF_INT_ENABLE macro must be always paired with #QF_INT_DISABLE.
* You should keep the code between these two macros to the minimum.
*/
#define QF_INT_ENABLE()         int_enable()

void int_disable(void);
void int_enable(void);

             /* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
#include <stdint.h>

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

#endif                                                        /* qpn_port_h */

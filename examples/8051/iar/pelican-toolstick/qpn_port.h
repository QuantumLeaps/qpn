/*****************************************************************************
* PELICAN crossing example, 8051, non-preemptive, IAR 8051 compiler
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
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

        /* the __code extended keywords forces allocating const data to ROM */
#define Q_ROM                   __code
#define Q_PARAM_SIZE            1
#define QF_TIMEEVT_CTR_SIZE     2

#define Q_NFSM

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2

            /* interrupt locking policy for C8051F300/1/2/3/4/5, see NOTE01 */
#define QF_INT_DISABLE()        (IE_bit.EA = 0, IE_bit.EA = 0)
#define QF_INT_ENABLE()         (IE_bit.EA = 1)

                                    /* interrupt nesting policy, see NOTE02 */
#define QF_ISR_NEST

             /* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
typedef signed   char  int8_t;
typedef signed   int   int16_t;
typedef signed   long  int32_t;
typedef unsigned char  uint8_t;
typedef unsigned int   uint16_t;
typedef unsigned long  uint32_t;


#include <ioC8051F300.h>           /* SFR declarations for C8051F300 device */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */

  /* shadow of the PCON register for atomic transition to Idle mode, NOTE03 */
#ifdef NDEBUG
extern uint8_t volatile __bdata QF_pcon;
#endif

/*****************************************************************************
* NOTE01:
* According to the C8051F300/1/2/3/4/5 Manual, clearing the IE_bit.EA bit
* must always be followed by another 2-byte opcode ("C8051F300/1/2/3/4/5
* Manual", Sec 8.3)
*
* NOTE02:
* The 80x51 hardware supports interrupt prioritization and does *not*
* disable interrupts upon the entry to the interrupt (the global interrupt
* enable bit EA is *not* cleared). For this reason interrupts generally
* run with interrupts enabled. This is conveniet for using the simple policy
* of unconditional unlocking of interrupts upon exit from a critical section,
* because there is no risk of nesting critical sections. (You should be
* careful not to introduce nesting of critical sections by not disabling
* interrupts before calling any QF services, such as QF_publish().)
* Instead, you can always prioritize all interrupts at the same level to
* prevent preemptions by other interrupts.
*
* NOTE03:
* The low-power mode must be entered with interrupts locked to avoid
* an interrupt to preempt the transition to the low-power mode. However,
* the 80x51 architecture does not provide an *atomic* transition to the Idle
* mode with simultaneous unlocking interrupts. Instead, here the PCON
* register is shadowed in the QF_pcon bdata variable. The idle mode
* is set first in the shadow register QF_pcon. At the end of every interrupt,
* the ISR copies the PCON value into the QF_pcon shadow (the 80x51 clears the
* IDL and PD bits in the PCON register upon interrupt entry). Later in
* QF_onIdle(), the PCON register is restored from the shadow QF_pcon, which
* the 80x51 performs *atomically* as a single machine instruction (such as
* MOV 87H,20H). If the interrupt gets serviced between unlocking interrupts
* and restoring PCON from the shadow, the IDL/PD bits will be cleared in the
* shadow, so the machine will *not* go to the Idle?PD mode. Only if the
* IDL/PD bits survive in the shadow, the Idle/PD mode is entered.
*/

#endif                                                        /* qpn_port_h */


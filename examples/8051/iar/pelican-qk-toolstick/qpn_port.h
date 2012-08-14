/*****************************************************************************
* PELICAN crossing example, 8051, QK-nano, IAR 8051 compiler
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
#define Q_ROM               __code
#define Q_PARAM_SIZE        1
#define QF_TIMEEVT_CTR_SIZE 2

#define Q_NFSM

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE       2

            /* interrupt locking policy for C8051F300/1/2/3/4/5, see NOTE01 */
#define QF_INT_DISABLE()    (IE_bit.EA = 0, IE_bit.EA = 0)
#define QF_INT_ENABLE()     (IE_bit.EA = 1)

                                    /* interrupt nesting policy, see NOTE02 */
#define QF_ISR_NEST

                                        /* QK-nano interrupt entry and exit */
#define QK_ISR_ENTRY()      ((void)0)
#define QK_ISR_EXIT()       do { \
    uint8_t p; \
    QF_INT_DISABLE(); \
    p = QK_schedPrio_(); \
    if (p != (uint8_t)0) { \
        QK_eoi_(); \
        QK_sched_(p); \
    } \
    QF_INT_ENABLE(); \
} while (0)

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
#include "qkn.h"           /* QK-nano platform-independent public interface */

           /* QK-nano function to emulate return from interrupt, see NOTE03 */
void QK_eoi_(void);

/*****************************************************************************
* NOTE00:
* The maximum number of active objects QF_MAX_ACTIVE can be increased to 8
* inclusive. The lower limit of 4 used here is only to conserve some RAM.
*
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
* 8051 performs interrupt prioritization from interrupt entry to the RETI
* instruction. In QK-nano, the last nested interrupt (which returns to
* the task level) must perform the asynchronous preemption(s), that is,
* it needs to descend to the task-level and allow any interrupt to preempt
* the task level. The QK_eoi_() function issues an extra RETI instruction
* to end the interrupt-in-progress and emulate discending to the task level
* without actually returning to the interrupted task, which remains preempted
* until the QK-nano scheduler returns and the original interrupt returns.
*/

#endif                                                        /* qpn_port_h */


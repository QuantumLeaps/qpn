/*****************************************************************************
* Product: PELICAN crossing example, QK-nano kernel
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

#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     4

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2

                                 /* interrupt locking policy for task level */
#define QF_INT_DISABLE()        __asm volatile ("cpsid i")
#define QF_INT_ENABLE()         __asm volatile ("cpsie i")

                                  /* interrupt locking policy for ISR level */
#define QF_ISR_NEST
                                  /* QK-nano ISR entry and exit, see NOTE02 */
#define QK_ISR_ENTRY()   ((void)0)
#define QK_ISR_EXIT()    \
    (*Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (uint32_t)0x10000000) \

#include <stdint.h>       /* GNU provides C99-standard exact-width integers */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

/*****************************************************************************
* NOTE01:
* ARM Cortex does not lock interrupts upon the interrupt entry, so interrupts
* can nest by default. You can prevent interrupts from nesting by setting the
* priorities of all interrupts to the same level. In any case, you should not
* call any QP-nano services with interrupts locked.
*
* NOTE02:
* The QK_ISR_EXIT() macro triggers the Cortex-M3 PendSV exception, which
* in turn causes QK_schedule_() to run. The PendSV exception is prioritized
* lower than all interrupts in the system, so the NVIC performs tail-chaining
* to PendSV only after all nested interrupts are handled. This means that
* in Cortex-M3 you don't need to increment and decrement the nesting level
* QK_intNest_, because it doesn't matter for the tail-chaining mechanism.
*/

#endif                                                        /* qpn_port_h */

/*****************************************************************************
* Product: QDK-nano_ARM-GNU_AT91SAM7S-EK with QK-nano
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 11, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

#define Q_NFSM
#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           1

           /* fast unconditional interrupt disabling/enabling for ARM state */
#define QF_INT_DISABLE_32()   \
    __asm volatile ("MSR cpsr_c,#(0x1F | 0x80 | 0x40)" ::: "cc")

#define QF_INT_ENABLE_32() \
    __asm volatile ("MSR cpsr_c,#(0x1F)" ::: "cc")

#ifdef __thumb__                                             /* THUMB mode? */

                                 /* interrupt locking policy for task-level */
    #define QF_INT_DISABLE()    QF_int_disable_SYS()
    #define QF_INT_ENABLE()     QF_int_enable_SYS()

    void QF_int_disable_SYS(void);
    void QF_int_enable_SYS(void);

                                /* interrupt disabling policy for ISR-level */
    #define QF_ISR_NEST
    #define QF_ISR_KEY_TYPE       unsigned long
    #define QF_ISR_DISABLE(stat_) ((stat_) = QF_isr_lock_SYS())
    #define QF_ISR_RESTORE(stat_) (QF_isr_restore_SYS(stat_))

    QF_ISR_KEY_TYPE QF_isr_disable_SYS(void);
    void QF_isr_restore_SYS(QF_ISR_KEY_TYPE key);

#elif (defined __arm__)                                        /* ARM mode? */

                               /* interrupt disabling policy for task-level */
    #define QF_INT_DISABLE()    QF_INT_DISABLE_32()
    #define QF_INT_ENABLE()     QF_INT_ENABLE_32()

                                /* interrupt disabling policy for ISR-level */
    #define QF_ISR_NEST
    #define QF_ISR_STAT_TYPE     unsigned long

    #define QF_ISR_DISABLE(stat_)   do { \
        __asm volatile ("MRS %0,cpsr" : "=r" (stat_) :: "cc"); \
        QF_INT_DISABLE_32(); \
    } while (0)

    #define QF_ISR_RESTORE(stat_) \
        __asm volatile ("MSR cpsr_c,%0" :: "r" (stat_) : "cc")

#else

    #error Incorrect CPU mode. Must be either __arm__ or __thumb__.

#endif


void QF_reset(void);
void QF_undef(void);
void QF_swi(void);
void QF_pAbort(void);
void QF_dAbort(void);
void QF_reserved(void);

void QK_irq(void);
void QK_fiq(void);

void BSP_irq(void);
void BSP_fiq(void);

#include <stdint.h>    /* Exact-width integer types. WG14/N843 C99 Standard */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

#endif                                                        /* qpn_port_h */

/*****************************************************************************
* Product: QP-nano port for DPP example, with Vanilla kernel
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

#define Q_NFSM
#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           6

           /* fast unconditional interrupt disabling/enabling for ARM state */
#define QF_INT_DISABLE_32()     __asm("MSR cpsr_c,#(0x1F | 0x80 | 0x40)")
#define QF_INT_ENABLE_32()      __asm("MSR cpsr_c,#(0x1F)")

#if (__CPU_MODE__ == 1)                                      /* THUMB mode? */

                               /* interrupt disabling policy for task-level */
    #define QF_INT_DISABLE()    QF_int_disable_SYS()
    #define QF_INT_ENABLE()     QF_int_enable_SYS()

    void QF_int_disable_SYS(void);
    void QF_int_enable_SYS(void);

                                  /* interrupt locking policy for ISR-level */
    #define QF_ISR_NEST
    #define QF_ISR_STAT_TYPE      unsigned long
    #define QF_ISR_DISABLE(stat_) ((stat_) = QF_isr_disable_SYS())
    #define QF_ISR_ENABLE(key_)   (QF_isr_restore_SYS(stat_))

    QF_ISR_STAT_TYPE QF_isr_disable_SYS(void);
    void QF_isr_restore_SYS(QF_ISR_STAT_TYPE stat);

#elif (__CPU_MODE__ == 2)                                      /* ARM mode? */

                               /* interrupt disabling policy for task-level */
    #define QF_INT_DISABLE()    QF_INT_DISABLE_32()
    #define QF_INT_ENABLE()     QF_INT_ENABLE_32()

                       /* interrupt disbling/restoring policy for ISR-level */
    #define QF_ISR_NEST
    #define QF_ISR_STAT_TYPE    unsigned long

    #define QF_ISR_DISABLE(stat_) do { \
        (stat_) = __get_CPSR(); \
        QF_INT_DISABLE_32(); \
    } while (0)

    #define QF_ISR_RESTORE(stat_) __set_CPSR(stat_)

    #include <intrinsics.h>               /* for  __get_CPSR()/__set_CPSR() */

#else

    #error Incorrect __CPU_MODE__. Must be ARM or THUMB.

#endif


void QF_reset(void);
void QF_undef(void);
void QF_swi(void);
void QF_pAbort(void);
void QF_dAbort(void);
void QF_reserved(void);
void QF_irq(void);
void QF_fiq(void);

void BSP_irq(void);
void BSP_fiq(void);

#include <stdint.h>    /* Exact-width integer types. WG14/N843 C99 Standard */

#include "qepn.h"              /* QEP-nano platform-independent header file */
#include "qfn.h"                /* QF-nano platform-independent header file */

#endif                                                        /* qpn_port_h */

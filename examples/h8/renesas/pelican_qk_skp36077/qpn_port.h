/*****************************************************************************
* Product: PELICAN crossing example, H8, SKP36077 board, QK-nano kernel
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 02, 2012
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
#define Q_PARAM_SIZE        1
#define QF_TIMEEVT_CTR_SIZE 2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE       2


                             /* interrupt locking policy for the task level */
#define QF_INT_DISABLE()    set_imask_ccr(1)
#define QF_INT_ENABLE()     set_imask_ccr(0)

                                  /* interrupt locking policy for ISR level */
/* #define QF_ISR_NEST */                    /* nesting of ISRs not allowed */

                                        /* interrupt entry/exit for QK-nano */
#define QK_ISR_ENTRY()      ((void)0)
#define QK_ISR_EXIT()       QK_SCHEDULE_()

   /* exact-width integer types (NC30 compiler does NOT provide <stdint.h>) */
typedef unsigned char  uint8_t;
typedef signed   char  int8_t;
typedef unsigned short uint16_t;
typedef signed   short int16_t;
typedef unsigned long  uint32_t;
typedef signed   long  int32_t;

#include <machine.h>                       /* prototype for set_imask_ccr() */

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

#endif                                                        /* qpn_port_h */

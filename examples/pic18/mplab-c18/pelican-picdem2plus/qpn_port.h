/*****************************************************************************
* PELICAN crossing example, PIC18, non-preemptive, MPLAB C18 compiler
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
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

         /* the 'rom' extended keywords forces allocating const data to ROM */
#define Q_ROM                   rom

#define Q_NFSM
#define Q_PARAM_SIZE            1
#define QF_TIMEEVT_CTR_SIZE     2


/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2U

                       /* task-level interrupt disabling policy, see NOTE01 */
#define QF_INT_DISABLE()    (INTCON &= 0b00111111)
#define QF_INT_ENABLE()     (INTCON |= 0b11000000)

              /* ISR-level interrupt disabling policy for PIC18, see NOTE02 */
#define QF_ISR_NEST
#define QF_ISR_STAT_TYPE      uint8_t
#define QF_ISR_DISABLE(stat_) do { \
    (stat_) = (INTCON & 0b11000000); \
    QF_INT_DISABLE(); \
} while (0)
#define QF_ISR_RESTORE(stat_) (INTCON |= (stat_))


/* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
typedef signed   char  int8_t;
typedef signed   int   int16_t;
typedef signed   long  int32_t;
typedef unsigned char  uint8_t;
typedef unsigned int   uint16_t;
typedef unsigned long  uint32_t;

                                /* INTCON SFR declaration for PIC18 devices */
extern volatile near uint8_t INTCON;


#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */


/*****************************************************************************
* NOTE01:
* The task-level interrupt disabling policy assumes that interrupt priorities
* are allowed (IPEN=1) and that high-priority interrupts can nest on
* low-priority interrupts. In this mode, the bit INTCON<7> is interpreted as
* GIEH (high-priority interrupt enable) and INTCON<6> is interpreted as GIEL
* (low-priority interrupt enable). The task-level interrupt disabling policy
* clears both GIEH and GIEL bits and enabling sets both GIEH and GIEL.
*
* NOTE02:
* The ISR-level interrupt policy allows interrupt nesting. The ISR-level
* interrupt disabling saves the current interrupt status (the bits GIEH and
* GIEL) and clears both GIEH and GIEL. The interrupt enabling policy restores
* the saved state of the GIEH and GIEL bits. In particular, this ISR-level
* policy will never enable interrupts inside high-level ISRs, and will enable
* only the high-level interrupts inside the low-level ISRs. You should not
* enable interrupts explicitly inside ISRs.
*/

#endif                                                        /* qpn_port_h */


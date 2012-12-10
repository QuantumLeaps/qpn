/*****************************************************************************
* Product: QP-nano port for PELICAN crossing example, Qt emulation
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Aug 16, 2012
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

                        /* undefine the conflicitng macro between Qt and QP */
#undef Q_ASSERT

         /* avoid definition of the QEvent type conflicting with Qt::QEvent */
#define Q_NQEVENT               1

#define Q_NFSM                  1
#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           1

                                 /* interrupt locking policy for task level */
/* #define QF_INT_DISABLE() */
/* #define QF_INT_ENABLE() */

                            /* interrupt locking policy for interrupt level */
/* #define QF_ISR_NEST */

#include <stdint.h>            /* Exact-width types. WG14/N843 C99 Standard */
#include "qepn.h"              /* QEP-nano platform-independent header file */
#include "qfn.h"                /* QF-nano platform-independent header file */

void QF_setTickRate(uint32_t ticksPerSec);           /* set clock tick rate */
void QF_onCleanup(void);          /* "big" OS, such as Qt, requires cleanup */

#endif                                                        /* qpn_port_h */

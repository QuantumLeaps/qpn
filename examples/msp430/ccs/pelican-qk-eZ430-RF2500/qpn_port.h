/*****************************************************************************
* Product: PELICAN crossing example, QK-nano
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 09, 2012
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
#define Q_PARAM_SIZE            2
#define QF_TIMEEVT_CTR_SIZE     2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2

                                /* interrupt locking policy for task level */
#define QF_INT_DISABLE()        __disable_interrupt()
#define QF_INT_ENABLE()         __enable_interrupt()

                            /* interrupt locking policy for interrupt level */
/* #define QF_ISR_NEST */                    /* nesting of ISRs not allowed */

                                         /* interrupt entry and exit for QK */
#define QK_ISR_ENTRY()          ((void)0)
#define QK_ISR_EXIT()           QK_SCHEDULE_()


#include <stdint.h>    /* Exact-width integer types. WG14/N843 C99 Standard */
#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */
#include "qkn.h"           /* QK-nano platform-independent public interface */

#endif                                                        /* qpn_port_h */

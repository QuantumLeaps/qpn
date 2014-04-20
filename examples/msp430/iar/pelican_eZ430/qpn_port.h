/*****************************************************************************
* Product: Simple Blinky example, Vanilla kernel
* Last updated for version 5.3.0
* Last updated on  2014-04-19
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

//#define Q_NMSM
#define Q_NFSM
#define Q_NHSM
#define Q_NMSM_HIST
#define Q_PARAM_SIZE            0
#define QF_TIMEEVT_CTR_SIZE     1

/* maximum # active objects--must match EXACTLY the QF_active[] definition */
#define QF_MAX_ACTIVE           2

/* interrupt disabling policy for task level */
#define QF_INT_DISABLE()        __disable_interrupt()
#define QF_INT_ENABLE()         __enable_interrupt()

/* interrupt disabling policy for interrupt level */
/* #define QF_ISR_NEST */ /* nesting of ISRs not allowed */

#include <intrinsics.h> /* contains prototypes for the intrinsic functions */
#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qassert.h"    /* QP-nano assertions header file */

#endif /* qpn_port_h */

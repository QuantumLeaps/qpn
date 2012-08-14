/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Jun 28, 2012
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
#include "qpn_port.h"                                       /* QP-nano port */

Q_DEFINE_THIS_MODULE("qfn_init")

/**
* \file
* \ingroup qfn_init
* QF-nano implementation.
*/

void QF_init(void) {
    QActive *a;
    uint8_t p;

    QF_readySet_ = (uint8_t)0;

#ifdef Q_TIMERSET
    QF_timerSet_ = (uint8_t)0;
#endif

#ifdef QK_PREEMPTIVE
    QK_currPrio_ = (uint8_t)(QF_MAX_ACTIVE + 1);

#ifdef QF_ISR_NEST
    QK_intNest_ = (uint8_t)0;
#endif

#ifdef QK_MUTEX
    QK_ceilingPrio_ = (uint8_t)0;
#endif

#endif                                              /* #ifdef QK_PREEMPTIVE */
                                  /* clear all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        Q_ASSERT(a != (QActive *)0);    /* QF_active[p] must be initialized */
        a->head    = (uint8_t)0;
        a->tail    = (uint8_t)0;
        a->nUsed   = (uint8_t)0;
#if (QF_TIMEEVT_CTR_SIZE != 0)
        a->tickCtr = (QTimeEvtCtr)0;
#endif
    }
}

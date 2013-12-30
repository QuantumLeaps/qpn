/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 30, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
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
    uint8_t n;

#ifdef Q_TIMERSET
    for (n = (uint8_t)0; n < (uint8_t)QF_MAX_TICK_RATE; ++n) {
        QF_timerSetX_[n] = (uint8_t)0;
    }
#endif

    QF_readySet_ = (uint8_t)0;

#ifdef QK_PREEMPTIVE
    QK_currPrio_ = (uint8_t)(QF_MAX_ACTIVE + 1);

#ifdef QF_ISR_NEST
    QK_intNest_ = (uint8_t)0;
#endif

#ifndef QK_NO_MUTEX
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
        for (n = (uint8_t)0; n < (uint8_t)QF_MAX_TICK_RATE; ++n) {
            a->tickCtr[n] = (QTimeEvtCtr)0;
        }
#endif
    }
}

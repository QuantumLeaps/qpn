/**
* \file
* \brief QF_init() implementation.
* \ingroup qfn
* \cond
******************************************************************************
* Product: QF-nano
* Last updated for version 5.3.0
* Last updated on  2014-04-14
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
******************************************************************************
* \endcond
*/
#include "qpn_port.h" /* QP-nano port */

#ifndef qassert_h
    #include "qassert.h" /* QP assertions */
#endif /* qassert_h */

Q_DEFINE_THIS_MODULE("qfn_init")

/****************************************************************************/
/**
* \description
* This function initializes the internal QF-nano variables as well as all
* registered active objects to zero. In the C startup code compliant with
* the C Standard this clearing of internal variables is unnecessary, because
* all static variables are supposed to be cleared in the startup code.
* However in non-compliant implementations calling QF_init() can be very
* handy.
*
* \note Function QF_init() is defined in the separate module qf_init.c, which
* needs to be included in the build only if the non-standard initialization
* is required.
*/
void QF_init(void) {
    QActive *a;
    uint_fast8_t p;
    uint_fast8_t n;

#ifdef Q_TIMERSET
    for (n = (uint_fast8_t)0; n < (uint_fast8_t)QF_MAX_TICK_RATE; ++n) {
        QF_timerSetX_[n] = (uint_fast8_t)0;
    }
#endif

    QF_readySet_ = (uint_fast8_t)0;

#ifdef QK_PREEMPTIVE
    QK_currPrio_ = (uint_fast8_t)(QF_MAX_ACTIVE + 1);

#ifdef QF_ISR_NEST
    QK_intNest_ = (uint_fast8_t)0;
#endif

#ifndef QK_NO_MUTEX
    QK_ceilingPrio_ = (uint_fast8_t)0;
#endif

#endif /* #ifdef QK_PREEMPTIVE */

    /* clear all registered active objects... */
    for (p = (uint_fast8_t)1; p <= (uint_fast8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);

        /* QF_active[p] must be initialized */
        Q_ASSERT_ID(110, a != (QActive *)0);

        a->head    = (uint_fast8_t)0;
        a->tail    = (uint_fast8_t)0;
        a->nUsed   = (uint_fast8_t)0;
#if (QF_TIMEEVT_CTR_SIZE != 0)
        for (n = (uint_fast8_t)0; n < (uint_fast8_t)QF_MAX_TICK_RATE; ++n) {
            a->tickCtr[n] = (QTimeEvtCtr)0;
        }
#endif
    }
}

/*****************************************************************************
* Product: "Reminder" state pattern example2
* Last updated for version 5.4.0
* Last updated on  2015-05-24
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
#include "qpn.h"
#include "bsp.h"
#include "reminder2.h"

/*..........................................................................*/
typedef struct CruncherTag { /* the Cruncher active object */
    QActive super;           /* inherit QActive */
    double sum;              /* internal variable */
} Cruncher;

/* state machine ... */
static QState Cruncher_initial   (Cruncher * const me);
static QState Cruncher_processing(Cruncher * const me);
static QState Cruncher_final     (Cruncher * const me);

/* Global objects ----------------------------------------------------------*/
Cruncher AO_Cruncher;

/*--------------------------------------------------------------------------*/
void Cruncher_ctor(void) {
    QActive_ctor(&AO_Cruncher.super, Q_STATE_CAST(&Cruncher_initial));
}
/*..........................................................................*/
QState Cruncher_initial(Cruncher * const me) {
    return Q_TRAN(&Cruncher_processing);
}
/*..........................................................................*/
QState Cruncher_processing(Cruncher * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QACTIVE_POST(&me->super, CRUNCH_SIG, 0);
            me->sum = 0.0;
            status = Q_HANDLED();
            break;
        }
        case CRUNCH_SIG: {
            uint32_t i = Q_PAR(me);
            uint32_t n = i;
            i += 100;
            for (; n < i; ++n) {
                if ((n & 1) == 0) {
                    me->sum += 1.0/(2*n + 1);
                }
                else {
                    me->sum -= 1.0/(2*n + 1);
                }
            }
            if (i < 0x07000000) {
                QACTIVE_POST(&me->super, CRUNCH_SIG, i);
                status = Q_HANDLED();
            }
            else {
                BSP_echo(me->sum);
                status = Q_TRAN(&Cruncher_processing);
            }
            break;
        }
        case ECHO_SIG: {
            BSP_echo(me->sum);
            status = Q_HANDLED();
            break;
        }
        case TERMINATE_SIG: {
            status = Q_TRAN(&Cruncher_final);
            break;
        }
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}
/*..........................................................................*/
QState Cruncher_final(Cruncher * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QF_stop(); /* terminate the application */
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

/*****************************************************************************
* Product: Orthogonal Component state pattern example
* Last updated for version 5.4.2
* Last updated on  2015-06-07
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
#include "qpn.h"
#include "bsp.h"
#include "clock.h"

#include <stdio.h>

/* HSM methods */
static QState Alarm_initial(Alarm *me);
static QState Alarm_on     (Alarm *me);
static QState Alarm_off    (Alarm *me);

/*..........................................................................*/
void Alarm_ctor(Alarm * const me) {
    QFsm_ctor(&me->super, Q_STATE_CAST(&Alarm_initial));
}

/* HSM definition ----------------------------------------------------------*/
QState Alarm_initial(Alarm * const me) {
    me->alarm_time = 12U*60U;
    return Q_TRAN(&Alarm_off);
}
/*..........................................................................*/
QState Alarm_off(Alarm * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* while in the off state, the alarm is kept in decimal format */
            me->alarm_time = (me->alarm_time/60U)*100U + me->alarm_time%60U;
            printf("*** Alarm OFF %02d:%02d\n",
                   me->alarm_time/100U, me->alarm_time%100);
            fflush(stdout);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            /* upon exit, the alarm is converted to binary format */
            me->alarm_time = (me->alarm_time/100U)*60U + me->alarm_time%100U;
            status = Q_HANDLED();
            break;
        }
        case ALARM_ON_SIG: {
            /* alarm in range? */
            if ((me->alarm_time / 100U < 24U) && (me->alarm_time % 100U < 60U)) {
                status = Q_TRAN(&Alarm_on);
            }
            else { /* alarm out of range -- clear and don't transition */
                me->alarm_time = 0U;
                printf("*** Alarm reset %02d:%02d\n",
                       me->alarm_time/100U, me->alarm_time%100U);
                status = Q_HANDLED();
            }
            break;
        }
        case ALARM_SET_SIG: {
            /* while setting, the alarm is kept in decimal format */
            me->alarm_time = (10U * me->alarm_time + Q_PAR(me)) % 10000U;
            printf("*** Alarm SET %02d:%02d\n",
                   me->alarm_time/100U, me->alarm_time%100U);
            fflush(stdout);
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_IGNORED();
            break;
        }
    }
    return status;
}
/*..........................................................................*/
QState Alarm_on(Alarm * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("*** Alarm ON %02d:%02d\n",
                   me->alarm_time/60, me->alarm_time%60);
            fflush(stdout);
            status = Q_HANDLED();
            break;
        }
        case ALARM_SET_SIG: {
            printf("*** Cannot set Alarm when it is ON\n");
            fflush(stdout);
            status = Q_HANDLED();
            break;
        }
        case ALARM_OFF_SIG: {
            status = Q_TRAN(&Alarm_off);
            break;
        }
        case TIME_SIG: {
            if (Q_PAR(me) == me->alarm_time) {
                printf("ALARM!!!\n");
                /* asynchronously post the event to the container AO */
                QACTIVE_POST((QMActive *)&AO_AlarmClock, ALARM_SIG, 0);
            }
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_IGNORED();
            break;
        }
    }
    return status;
}

/*****************************************************************************
* Product: "Orthogonal Component" state pattern example
* Last updated for version 6.4.0
* Last updated on  2019-02-10
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
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
* https://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpn.h"
#include "bsp.h"
#include "clock.h"

#include <stdio.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
typedef struct AlarmClockTag { /* the AlarmClock active object */
    QActive  super;            /* inherit QActive */
    Alarm    alarm;            /* Alarm orthogonal component */
    uint32_t current_time;     /* the current time in seconds */
} AlarmClock;

static QState AlarmClock_initial    (AlarmClock * const me);
static QState AlarmClock_timekeeping(AlarmClock * const me);
static QState AlarmClock_mode12hr   (AlarmClock * const me);
static QState AlarmClock_mode24hr   (AlarmClock * const me);
static QState AlarmClock_final      (AlarmClock * const me);

/* Global objects ----------------------------------------------------------*/
AlarmClock AO_AlarmClock; /* the single instance of the AlarmClock AO */

/*..........................................................................*/
void AlarmClock_ctor(void) {
    QActive_ctor((QActive *)&AO_AlarmClock,
                 Q_STATE_CAST(&AlarmClock_initial));
    Alarm_ctor(&AO_AlarmClock.alarm); /* orthogonal component ctor */
}
/* HSM definition ----------------------------------------------------------*/
QState AlarmClock_initial(AlarmClock * const me) {
    me->current_time = 0U;
    /* take the initial transition in the alarm component... */
    QHSM_INIT(&me->alarm.super);
    return Q_TRAN(&AlarmClock_timekeeping);
}
/*..........................................................................*/
QState AlarmClock_final(AlarmClock * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> final\n");
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
/*..........................................................................*/
QState AlarmClock_timekeeping(AlarmClock * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* timeout in one second and every second */
            QActive_armX(&me->super, 0U,
                         BSP_TICKS_PER_SEC, BSP_TICKS_PER_SEC);
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            QActive_disarmX(&me->super, 0U);
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status = Q_TRAN(&AlarmClock_mode24hr);
            break;
        }
        case CLOCK_12H_SIG: {
            status = Q_TRAN(&AlarmClock_mode12hr);
            break;
        }
        case CLOCK_24H_SIG: {
            status = Q_TRAN(&AlarmClock_mode24hr);
            break;
        }
        case ALARM_SIG: {
            printf("Wake up!!!\n");
            status = Q_HANDLED();
            break;
        }
        case ALARM_SET_SIG:
        case ALARM_ON_SIG:
        case ALARM_OFF_SIG: {
            /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = Q_SIG(me);
            Q_PAR(&me->alarm) = Q_PAR(me);
            QHSM_DISPATCH(&me->alarm.super);
            status = Q_HANDLED();
            break;
        }
        case TERMINATE_SIG: {
            status = Q_TRAN(&AlarmClock_final);
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
QState AlarmClock_mode24hr(AlarmClock * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("*** 24-hour mode\n");
            status = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            /* roll over in 24-hr mode? */
            if (++me->current_time == 24U*60U) {
                me->current_time = 0U;
            }
            printf("%02d:%02d\n",
                   me->current_time / 60U, me->current_time % 60U);

            /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = TIME_SIG;
            Q_PAR(&me->alarm) = me->current_time;
            QHSM_DISPATCH(&me->alarm.super);
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&AlarmClock_timekeeping);
            break;
        }
    }
    return status;
}
/*..........................................................................*/
QState AlarmClock_mode12hr(AlarmClock * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("*** 12-hour mode\n");
            status = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            uint32_t h; /* temporary variable to hold hour */
            if (++me->current_time == 12U * 60U) {  /* roll over in 12-hr mode? */
                me->current_time = 0U;
            }
            h = me->current_time / 60U;
            printf("%02d:%02d %s\n", (h % 12U) ? (h % 12U) : 12U,
                   me->current_time % 60U, (h / 12U) ? "PM" : "AM");

            /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = TIME_SIG;
            Q_PAR(&me->alarm) = me->current_time;
            QHSM_DISPATCH(&me->alarm.super);
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&AlarmClock_timekeeping);
            break;
        }
    }
    return status;
}

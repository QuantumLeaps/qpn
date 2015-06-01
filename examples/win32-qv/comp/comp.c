/*****************************************************************************
* Product: "Orthogonal Component" state pattern example
* Last updated for version 5.4.0
* Last updated on  2015-05-18
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
#include "comp.h"

#include <stdio.h>

/*..........................................................................*/
typedef struct AlarmClockTag { /* the AlarmClock active object */
    QActive  super;            /* inherit QActive */
    Alarm    alarm;            /* Alarm orthogonal component */
    uint32_t current_time;     /* the current time in seconds */
} AlarmClock;

static QState AlarmClock_initial    (AlarmClock *me);
static QState AlarmClock_timekeeping(AlarmClock *me);
static QState AlarmClock_mode12hr   (AlarmClock *me);
static QState AlarmClock_mode24hr   (AlarmClock *me);
static QState AlarmClock_final      (AlarmClock *me);

/* Global objects ----------------------------------------------------------*/
AlarmClock AO_AlarmClock; /* the single instance of the AlarmClock AO */

/*..........................................................................*/
void AlarmClock_ctor(void) {
    QActive_ctor((QMActive *)&AO_AlarmClock,
                 Q_STATE_CAST(&AlarmClock_initial));
    Alarm_ctor(&AO_AlarmClock.alarm); /* orthogonal component ctor */
}
/* HSM definition ----------------------------------------------------------*/
QState AlarmClock_initial(AlarmClock *me) {
    me->current_time = 0;
    /* take the initial transition in the alarm component... */
    QMSM_INIT(&me->alarm.super);
    return Q_TRAN(&AlarmClock_timekeeping);
}
/*..........................................................................*/
QState AlarmClock_final(AlarmClock *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QF_stop(); /* terminate the application */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState AlarmClock_timekeeping(AlarmClock *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* timeout in one second and every second */
            QActive_armX(&me->super, 0U,
                         BSP_TICKS_PER_SEC, BSP_TICKS_PER_SEC);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm(&me->super);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&AlarmClock_mode24hr);
        }
        case CLOCK_12H_SIG: {
            return Q_TRAN(&AlarmClock_mode12hr);
        }
        case CLOCK_24H_SIG: {
            return Q_TRAN(&AlarmClock_mode24hr);
        }
        case ALARM_SIG: {
            printf("Wake up!!!\n");
            return Q_HANDLED();
        }
        case ALARM_SET_SIG:
        case ALARM_ON_SIG:
        case ALARM_OFF_SIG: {
            /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = Q_SIG(me);
            Q_PAR(&me->alarm) = Q_PAR(me);
            QMSM_DISPATCH(&me->alarm.super);
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            return Q_TRAN(&AlarmClock_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState AlarmClock_mode24hr(AlarmClock *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("*** 24-hour mode\n");
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            /* roll over in 24-hr mode? */
            if (++me->current_time == 24U*60U) {
                me->current_time = 0U;
            }
            printf("%02d:%02d\n",
                   me->current_time/60, me->current_time%60);

            /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = TIME_SIG;
            Q_PAR(&me->alarm) = me->current_time;
            QMSM_DISPATCH(&me->alarm.super);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock_timekeeping);
}
/*..........................................................................*/
QState AlarmClock_mode12hr(AlarmClock *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("*** 12-hour mode\n");
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            uint32_t h; /* temporary variable to hold hour */
            if (++me->current_time == 12*60) {  /* roll over in 12-hr mode? */
                me->current_time = 0;
            }
            h = me->current_time/60;
            printf("%02d:%02d %s\n", (h % 12) ? (h % 12) : 12,
                   me->current_time % 60, (h / 12) ? "PM" : "AM");

            /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = TIME_SIG;
            Q_PAR(&me->alarm) = me->current_time;
            QMSM_DISPATCH(&me->alarm.super);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock_timekeeping);
}

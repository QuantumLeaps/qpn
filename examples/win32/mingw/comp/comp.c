/*****************************************************************************
* Product: "Orthogonal Component" state pattern example
* Last Updated for Version: 5.1.1
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
#include "qpn_port.h"
#include "bsp.h"
#include "comp.h"

#include <stdio.h>

/*..........................................................................*/
typedef struct AlarmClockTag {              /* the AlarmClock active object */
    QActive  super;                                  /* derive from QActive */
    Alarm    alarm;                           /* Alarm orthogonal component */
    uint32_t current_time;                   /* the current time in seconds */
} AlarmClock;

static QState AlarmClock_initial    (AlarmClock *me);
static QState AlarmClock_timekeeping(AlarmClock *me);
static QState AlarmClock_mode12hr   (AlarmClock *me);
static QState AlarmClock_mode24hr   (AlarmClock *me);
static QState AlarmClock_final      (AlarmClock *me);

/* Global objects ----------------------------------------------------------*/
AlarmClock AO_AlarmClock;       /* the single instance of the AlarmClock AO */

/*..........................................................................*/
void AlarmClock_ctor(void) {
    QActive_ctor((QActive *)&AO_AlarmClock,
                 (QStateHandler)&AlarmClock_initial);
    Alarm_ctor(&AO_AlarmClock.alarm);          /* orthogonal component ctor */
}
/* HSM definition ----------------------------------------------------------*/
QState AlarmClock_initial(AlarmClock *me) {
    me->current_time = 0;
                   /* take the initial transition in the alarm component... */
    Alarm_init(&me->alarm);
    return Q_TRAN(&AlarmClock_timekeeping);
}
/*..........................................................................*/
QState AlarmClock_final(AlarmClock *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> final\n"
                   "\nBye!Bye!\n");
            QF_stop();                         /* terminate the application */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState AlarmClock_timekeeping(AlarmClock *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
                                                   /* timeout in one second */
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm((QActive *)me);
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
            Alarm_dispatch(&me->alarm);
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
                                                   /* timeout in one second */
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC);

            if (++me->current_time == 24*60) {  /* roll over in 24-hr mode? */
                me->current_time = 0;
            }
            printf("%02d:%02d\n",
                   me->current_time/60, me->current_time%60);

                      /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = TIME_SIG;
            Q_PAR(&me->alarm) = me->current_time;
            Alarm_dispatch(&me->alarm);
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
            uint32_t h;                  /* temporary variable to hold hour */
                                                   /* timeout in one second */
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC);

            if (++me->current_time == 12*60) {  /* roll over in 12-hr mode? */
                me->current_time = 0;
            }
            h = me->current_time/60;
            printf("%02d:%02d %s\n", (h % 12) ? (h % 12) : 12,
                   me->current_time % 60, (h / 12) ? "PM" : "AM");

                      /* synchronously dispatch to the orthogonal component */
            Q_SIG(&me->alarm) = TIME_SIG;
            Q_PAR(&me->alarm) = me->current_time;
            Alarm_dispatch(&me->alarm);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock_timekeeping);
}

/* test harness ============================================================*/

/* Local-scope objects -----------------------------------------------------*/
static QEvt l_clockQueue[3];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
    { (QActive *)0,              (QEvt *)0,    0U                  },
    { (QActive *)&AO_AlarmClock, l_clockQueue, Q_DIM(l_clockQueue) }
};
uint8_t const Q_ROM QF_activeNum = Q_DIM(QF_active) - 1;

/*..........................................................................*/
int main (void) {
    AlarmClock_ctor();

    printf("Orthogonal Component state pattern\nQP-nano version: %s\n"
           "Press 'o' to turn the Alarm ON\n"
           "Press 'f' to turn the Alarm OFF\n"
           "Press '0'..'9' to set the Alarm time\n"
           "Press 'A' to set the Clock in 12-hour mode\n"
           "Press 'B' to set the Clock in 24-hour mode\n"
           "Press ESC to quit...\n",
           QP_getVersion());


    BSP_init();                                     /* initialize the board */

    return QF_run();                         /* transfer control to QF-nano */
}
/*..........................................................................*/
void BSP_onKeyboardInputISR(uint8_t key) {
    switch (key) {
        case 'o': {                                 /* 'o': Alarm on event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, ALARM_ON_SIG, 0);
            break;
        }
        case 'f': {                                /* 'f': Alarm off event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, ALARM_OFF_SIG, 0);
            break;
        }
        case '1':                                                    /* '1' */
        case '2':                                                    /* '2' */
        case '3':                                                    /* '3' */
        case '4':                                                    /* '4' */
        case '5':                                                    /* '5' */
        case '6':                                                    /* '6' */
        case '7':                                                    /* '7' */
        case '8':                                                    /* '8' */
        case '9': {                                                  /* '9' */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, ALARM_SET_SIG, key-1);
            break;
        }
        case '0': {                                                  /* '0' */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, ALARM_SET_SIG, 0);
            break;
        }
        case 'a': {                                /* 'a': Clock 12H event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, CLOCK_12H_SIG, 0);
            break;
        }
        case 'b': {                                /* 'b': Clock 24H event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, CLOCK_24H_SIG, 0);
            break;
        }
        case '\33': {                                       /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock, TERMINATE_SIG, 0);
            break;
        }
    }
}

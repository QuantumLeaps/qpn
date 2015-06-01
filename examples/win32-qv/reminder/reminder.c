/*****************************************************************************
* Product: "Reminder" state pattern example (Section 5.2 in PSiCC2)
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
#include "reminder.h"

#include <stdio.h>

/*..........................................................................*/
typedef struct SensorTag {
    QActive super;  /* inherit QActive */

    uint16_t pollCtr;
    uint16_t procCtr;
} Sensor;

/* hierarchical state machine ... */
static QState Sensor_initial   (Sensor *me);
static QState Sensor_polling   (Sensor *me);
static QState Sensor_processing(Sensor *me);
static QState Sensor_idle      (Sensor *me);
static QState Sensor_busy      (Sensor *me);
static QState Sensor_final     (Sensor *me);

/* Global objects ----------------------------------------------------------*/
Sensor AO_Sensor;

/*..........................................................................*/
void Sensor_ctor(void) {
    QActive_ctor((QActive *)&AO_Sensor, (QStateHandler)&Sensor_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Sensor_initial(Sensor *me) {
    me->pollCtr = 0;
    me->procCtr = 0;
    return Q_TRAN(&Sensor_polling);
}
/*..........................................................................*/
QState Sensor_final(Sensor *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QF_stop(); /* terminate the application */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Sensor_polling(Sensor *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* timeout in 1/2 second and every 1/2 second  */
            QActive_armX(&me->super, 0U,
                         BSP_TICKS_PER_SEC/2U, BSP_TICKS_PER_SEC/2U);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm(&me->super);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Sensor_processing);
        }
        case Q_TIMEOUT_SIG: {
            /* timeout in 1/2 second */
            ++me->pollCtr;
            printf("poll %3d\n", me->pollCtr);
            if ((me->pollCtr & 0x3U) == 0U) { /* modulo 4 */
                QActive_post((QActive *)me, DATA_READY_SIG, 0);
            }
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            return Q_TRAN(&Sensor_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Sensor_processing(Sensor *me) {
    switch (Q_SIG(me)) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Sensor_idle);
        }
    }
    return Q_SUPER(&Sensor_polling);
}
/*..........................................................................*/
QState Sensor_idle(Sensor *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> idle\n");
            return Q_HANDLED();
        }
        case DATA_READY_SIG: {
            return Q_TRAN(&Sensor_busy);
        }
    }
    return Q_SUPER(&Sensor_processing);
}
/*..........................................................................*/
QState Sensor_busy(Sensor *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> busy\n");
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            /* timeout in 1/2 second and every 1/2 second  */
            QActive_armX(&me->super, 0U,
                         BSP_TICKS_PER_SEC/2U, BSP_TICKS_PER_SEC/2U);
            ++me->procCtr;
            printf("process %3d\n", me->procCtr);
            if ((me->procCtr & 0x1U) == 0U) { /* modulo 2 */
                return Q_TRAN(&Sensor_idle);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Sensor_processing);
}


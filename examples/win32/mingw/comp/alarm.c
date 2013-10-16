/*****************************************************************************
* Product: Orthogonal Component state pattern example
* Last Updated for Version: 4.1.01
* Date of the Last Update:  Nov 05, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"
#include "comp.h"

#include <stdio.h>

/* HSM methods */
static QState Alarm_initial(Alarm *me);
static QState Alarm_on     (Alarm *me);
static QState Alarm_off    (Alarm *me);

/*..........................................................................*/
void Alarm_ctor(Alarm *me) {
    QFsm_ctor(&me->super, (QStateHandler)&Alarm_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Alarm_initial(Alarm *me) {
    me->alarm_time = 12*60;
    return Q_TRAN(&Alarm_off);
}
/*..........................................................................*/
QState Alarm_off(Alarm *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
             /* while in the off state, the alarm is kept in decimal format */
            me->alarm_time = (me->alarm_time/60)*100 + me->alarm_time%60;
            printf("*** Alarm OFF %02d:%02d\n",
                   me->alarm_time/100, me->alarm_time%100);
            fflush(stdout);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
                      /* upon exit, the alarm is converted to binary format */
            me->alarm_time = (me->alarm_time/100)*60 + me->alarm_time%100;
            return Q_HANDLED();
        }
        case ALARM_ON_SIG: {
                                                         /* alarm in range? */
            if ((me->alarm_time / 100 < 24) && (me->alarm_time % 100 < 60)) {
                return Q_TRAN(&Alarm_on);
            }
            else {      /* alarm out of range -- clear and don't transition */
                me->alarm_time = 0;
                printf("*** Alarm reset %02d:%02d\n",
                       me->alarm_time/100, me->alarm_time%100);
                return Q_HANDLED();
            }
        }
        case ALARM_SET_SIG: {
                      /* while setting, the alarm is kept in decimal format */
            me->alarm_time = (10 * me->alarm_time + Q_PAR(me)) % 10000;
            printf("*** Alarm SET %02d:%02d\n",
                   me->alarm_time/100, me->alarm_time%100);
            fflush(stdout);
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}
/*..........................................................................*/
QState Alarm_on(Alarm *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("*** Alarm ON %02d:%02d\n",
                   me->alarm_time/60, me->alarm_time%60);
            fflush(stdout);
            return Q_HANDLED();
        }
        case ALARM_SET_SIG: {
            printf("*** Cannot set Alarm when it is ON\n");
            fflush(stdout);
            return Q_HANDLED();
        }
        case ALARM_OFF_SIG: {
            return Q_TRAN(&Alarm_off);
        }
        case TIME_SIG: {
            if (Q_PAR(me) == me->alarm_time) {
                printf("ALARM!!!\n");
                       /* asynchronously post the event to the container AO */
                QActive_post((QActive *)&AO_AlarmClock, ALARM_SIG, 0);
            }
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}

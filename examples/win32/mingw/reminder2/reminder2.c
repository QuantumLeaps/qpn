/*****************************************************************************
* Product: "Reminder" state pattern example2
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 12, 2013
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
#include "qpn_port.h"
#include "bsp.h"
#include "reminder2.h"

#include <stdio.h>

/*..........................................................................*/
typedef struct CruncherTag {                  /* the Cruncher active object */
    QActive super;                                   /* derive from QActive */
    double sum;                                        /* internal variable */
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
            QACTIVE_POST((QActive *)me, CRUNCH_SIG, 0);
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
                QACTIVE_POST((QActive *)me, CRUNCH_SIG, i);
                status = Q_HANDLED();
            }
            else {
                printf("pi=%16.14f\n", 4.0*me->sum);
                status = Q_TRAN(&Cruncher_processing);
            }
            break;
        }
        case ECHO_SIG: {
            printf("Echo! pi=%16.14f\n", 4.0*me->sum);
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
            printf("final-ENTRY;\nBye!Bye!\n");
            QF_stop();                         /* terminate the application */
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

/* test harness ============================================================*/

/* Local-scope objects -----------------------------------------------------*/
static QEvt l_cruncherQueue[2];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
    { (QActive *)0,            (QEvt *)0,       0U                   },
    { (QActive *)&AO_Cruncher, l_cruncherQueue, Q_DIM(l_cruncherQueue) }
};

/* make sure that the QF_active[] array matches QF_MAX_ACTIVE in qpn_port.h */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

/*..........................................................................*/
int main (void) {
    Cruncher_ctor();

    printf("Reminder state pattern example2\nQP-nano version: %s\n"
           "Press 'e' to echo the current state of number cruncher ON\n"
           "Press ESC to quit\n"
           "Number crunching started...\n",
           QP_getVersion());

    BSP_init();                                     /* initialize the board */

    return QF_run();                         /* transfer control to QF-nano */
}
/*..........................................................................*/
void BSP_onKeyboardInputISR(uint8_t key) {
    switch (key) {
        case '\33': {                                       /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_Cruncher, TERMINATE_SIG, 0);
            break;
        }
        case 'e': {                                           /* echo event */
            QACTIVE_POST_ISR((QActive *)&AO_Cruncher, ECHO_SIG, 0);
            break;
        }
    }
}


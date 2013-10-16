/*****************************************************************************
* Product: History state pattern example
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 11, 2013
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

#include <dos.h>                       /* for _dos_setvect()/_dos_getvect() */
#include <conio.h>                                      /* for inp()/outp() */
#include <stdlib.h>                                          /* for _exit() */
#include <stdio.h>

Q_DEFINE_THIS_FILE

enum ToasterOvenSignals {
    TERMINATE_SIG = Q_USER_SIG,                /* terminate the application */
    OPEN_SIG,
    CLOSE_SIG,
    TOAST_SIG,
    BAKE_SIG,
    OFF_SIG
};
/*..........................................................................*/
typedef struct ToasterOvenTag {
    QHsm super;                                         /* derive from QHsm */
    QStateHandler doorClosed_history;    /* history of the doorClosed state */
} ToasterOven;

void ToasterOven_ctor(ToasterOven *me);

static QState ToasterOven_initial   (ToasterOven *me);
static QState ToasterOven_doorClosed(ToasterOven *me);
static QState ToasterOven_off       (ToasterOven *me);
static QState ToasterOven_heating   (ToasterOven *me);
static QState ToasterOven_toasting  (ToasterOven *me);
static QState ToasterOven_baking    (ToasterOven *me);
static QState ToasterOven_doorOpen  (ToasterOven *me);
static QState ToasterOven_final     (ToasterOven *me);

/*..........................................................................*/
void ToasterOven_ctor(ToasterOven *me) {                    /* default ctor */
    QHsm_ctor(&me->super, (QStateHandler)&ToasterOven_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState ToasterOven_initial(ToasterOven *me) {
    me->doorClosed_history = Q_STATE_CAST(&ToasterOven_off);
    return Q_TRAN(&ToasterOven_doorClosed);
}
/*..........................................................................*/
QState ToasterOven_final(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> final\n");
            printf("\nBye!Bye!\n");
            _exit(0);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState ToasterOven_doorClosed(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("door-Closed;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            me->doorClosed_history = QHsm_state(&me->super);/*<-save HISTORY*/
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&ToasterOven_off);
        }
        case OPEN_SIG: {
            return Q_TRAN(&ToasterOven_doorOpen);
        }
        case TOAST_SIG: {
            return Q_TRAN(&ToasterOven_toasting);
        }
        case BAKE_SIG: {
            return Q_TRAN(&ToasterOven_baking);
        }
        case OFF_SIG: {
            return Q_TRAN(&ToasterOven_off);
        }
        case TERMINATE_SIG: {
            return Q_TRAN(&ToasterOven_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState ToasterOven_off(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("toaster-Off;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&ToasterOven_doorClosed);
}
/*..........................................................................*/
QState ToasterOven_heating(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("heater-On;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("heater-Off;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&ToasterOven_doorClosed);
}
/*..........................................................................*/
QState ToasterOven_toasting(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("toasting;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&ToasterOven_heating);
}
/*..........................................................................*/
QState ToasterOven_baking(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("baking;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&ToasterOven_heating);
}
/*..........................................................................*/
QState ToasterOven_doorOpen(ToasterOven *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("door-Open,lamp-On;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("lamp-Off;");
            return Q_HANDLED();
        }
        case CLOSE_SIG: {
            return Q_TRAN(me->doorClosed_history);     /*<-tran. to HISTORY */
        }
    }
    return Q_SUPER(&QHsm_top);
}
/* test harness ============================================================*/

#include <conio.h>

static ToasterOven l_test;

int main() {
    printf("History state pattern\nQP-nano version: %s\n"
           "Press 'o' to OPEN  the door\n"
           "Press 'c' to CLOSE the door\n"
           "Press 't' to start TOASTING\n"
           "Press 'b' to start BAKING\n"
           "Press 'f' to turn the oven OFF\n"
           "Press ESC to quit...\n",
           QP_getVersion());

    ToasterOven_ctor(&l_test);        /* instantiate the ToasterOven object */
    QHsm_init((QHsm *)&l_test);           /* trigger the initial transition */

    for (;;) {
        uint8_t c;

        printf("\n");
        c = (uint8_t)_getch();       /* read one character from the console */
        printf("%c: ", c);

        switch (c) {
            case 'o':  Q_SIG(&l_test) = OPEN_SIG;      break;
            case 'c':  Q_SIG(&l_test) = CLOSE_SIG;     break;
            case 't':  Q_SIG(&l_test) = TOAST_SIG;     break;
            case 'b':  Q_SIG(&l_test) = BAKE_SIG;      break;
            case 'f':  Q_SIG(&l_test) = OFF_SIG;       break;
            case 0x1B: Q_SIG(&l_test) = TERMINATE_SIG; break;
        }
                               /* dispatch the event into the state machine */
        QHsm_dispatch((QHsm *)&l_test);
    }
    return 0;
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    _exit(-1);
}

/*****************************************************************************
* Product: "Ultimate Hook" state pattern example
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Q_DEFINE_THIS_FILE

enum UltimateHookSignals {
    A_SIG = Q_USER_SIG,
    B_SIG,
    C_SIG,
    D_SIG
};

/*..........................................................................*/
typedef struct UltimateHookTag {
    QHsm super;                                         /* derive from QHsm */
} UltimateHook;

void UltimateHook_ctor(UltimateHook *me);

static QState UltimateHook_initial (UltimateHook *me);
static QState UltimateHook_generic (UltimateHook *me);
static QState UltimateHook_specific(UltimateHook *me);
static QState UltimateHook_final   (UltimateHook *me);

/*..........................................................................*/
void UltimateHook_ctor(UltimateHook *me) {
    QHsm_ctor(&me->super, (QStateHandler)&UltimateHook_initial);
}
/*..........................................................................*/
QState UltimateHook_initial(UltimateHook *me) {
    return Q_TRAN(&UltimateHook_generic);
}
/*..........................................................................*/
QState UltimateHook_final(UltimateHook *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> final\n");
            printf("\nBye!Bye!\n");
            exit(0);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState UltimateHook_generic(UltimateHook *me) {
    switch (Q_SIG(me)) {
        case Q_INIT_SIG: {
            printf("generic:init;");
            return Q_TRAN(&UltimateHook_specific);
        }
        case A_SIG: {
            printf("generic:A;");
        }
        case B_SIG: {
            printf("generic:B;");
        }
        case C_SIG: {
            printf("generic:C(reset);");
            return Q_TRAN(&UltimateHook_generic);
        }
        case D_SIG: {
            return Q_TRAN(&UltimateHook_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState UltimateHook_specific(UltimateHook *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("specific:entry;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("specific:exit;");
            return Q_HANDLED();
        }
        case A_SIG: {
            printf("specific:A;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&UltimateHook_generic);
}
/* test harness ============================================================*/

#include <conio.h>

static UltimateHook l_test;

int main() {
    printf("Ultimate Hook pattern\nQP-nano version: %s\n"
           "Press 'a'..'c' to inject signals A..C\n"
           "Press 'd' or ESC to inject signal D and quit\n",
           QP_getVersion());

    UltimateHook_ctor(&l_test);      /* instantiate the UltimateHook object */
    QHsm_init((QHsm *)&l_test);           /* trigger the initial transition */

    for (;;) {
        uint8_t c;

        printf("\n");
        c = (uint8_t)_getch();       /* read one character from the console */
        printf("%c: ", c);

        switch (c) {
            case 'a':  Q_SIG(&l_test) = A_SIG;  break;
            case 'b':  Q_SIG(&l_test) = B_SIG;  break;
            case 'c':  Q_SIG(&l_test) = C_SIG;  break;
            case 'd':
            case 0x1B: Q_SIG(&l_test) = D_SIG;  break;         /* terminate */
        }
                               /* dispatch the event into the state machine */
        QHsm_dispatch((QHsm *)&l_test);
    }
    return 0;
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(-1);
}

/*****************************************************************************
* Product: History Example for QP-nano, Win32
* Last Updated for Version: 5.3.0
* Date of the Last Update:  Apr 16, 2014
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
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "qassert.h"
#include "history.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>

/*..........................................................................*/
int main() {
    printf("History state pattern with QMsm\nQP-nano version: %s\n"
           "Press 'o' to OPEN  the door\n"
           "Press 'c' to CLOSE the door\n"
           "Press 't' to start TOASTING\n"
           "Press 'b' to start BAKING\n"
           "Press 'f' to turn the oven OFF\n"
           "Press ESC to quit...\n",
           QP_getVersion());

    ToastOven_ctor(); /* instantiate the ToasterOven object */
    QMSM_INIT(the_oven); /* trigger the initial transition */

    for (;;) {
        uint8_t c;

        printf("\n");
        c = (uint8_t)_getch();  /* read one character from the console */
        printf("%c: ", c);

        switch (c) {
            case 'o':  Q_SIG(the_oven) = OPEN_SIG;      break;
            case 'c':  Q_SIG(the_oven) = CLOSE_SIG;     break;
            case 't':  Q_SIG(the_oven) = TOAST_SIG;     break;
            case 'b':  Q_SIG(the_oven) = BAKE_SIG;      break;
            case 'f':  Q_SIG(the_oven) = OFF_SIG;       break;
            case 0x1B: Q_SIG(the_oven) = TERMINATE_SIG; break;
        }

        /* dispatch the event into the state machine */
        QMSM_DISPATCH(the_oven);
    }
    return 0;
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    fprintf(stderr, "Assertion failed in %s, line/ID %d", file, line);
    _exit(-1);
}

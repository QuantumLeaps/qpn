/*****************************************************************************
* Product: BSP for Blinky example, Win32
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
#include "bsp.h"

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
void BSP_init() {
    printf("Simple Blinky example\nQP version: %s\n"
           "Press ESC to quit...\n",
           QP_VERSION_STR);
}
/*..........................................................................*/
void BSP_ledOff() {
    printf("OFF\n");
}
/*..........................................................................*/
void BSP_ledOn() {
    printf("ON\n");
}
/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const Q_ROM_VAR file, int_t line) {
    fprintf(stderr, "\nAssertion failed in %s, line %d\n", file, line);
    exit(-1);
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC);
}
/*..........................................................................*/
void QF_onCleanup(void) {
    printf("\nBye! Bye!\n");
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
    QF_tickXISR(0U);           /* perform the QF-nano clock tick processing */

    if (_kbhit()) {                                     /* any key pressed? */
        int ch = _getch();
        if (ch == 0x1B) {                     /* see if the ESC key pressed */
            QF_stop();
        }
    }
}





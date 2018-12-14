/*****************************************************************************
* Product: BSP for QP-nano example, Win32
* Last Updated for Version: 6.3.7
* Date of the Last Update:  2018-12-13
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2018 Quantum Leaps, LLC. All rights reserved.
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
#include "reminder2.h"

#include <stdlib.h>
#include <stdio.h>

//Q_DEFINE_THIS_FILE

/*..........................................................................*/
void BSP_init(void) {
    printf("Reminder state pattern example2\nQP-nano version: %s\n"
           "Press 'e' to echo the current state of number cruncher ON\n"
           "Press ESC to quit\n"
           "Number crunching started...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_echo(double sum) {
    printf("Echo! pi=%16.14f\n", 4.0*sum);
    fflush(stdout);
}
/*..........................................................................*/
void BSP_result(double sum) {
    printf("pi=%16.14f\n", 4.0*sum);
    fflush(stdout);
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    QF_consoleSetup();
    QF_setTickRate(BSP_TICKS_PER_SEC, 50); /* desired tick rate/ticker-prio */
}
/*..........................................................................*/
void QF_onCleanup(void) {
    printf("\nBye! Bye!\n");
    QF_consoleCleanup();
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
    QF_tickXISR(0U); /* perform the QF-nano clock tick processing */

    switch (QF_consoleGetKey()) {
        case '\33': { /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_Cruncher, TERMINATE_SIG, 0);
            break;
        }
        case 'e': {   /* echo event */
            QACTIVE_POST_ISR((QActive *)&AO_Cruncher, ECHO_SIG, 0);
            break;
        }
    }
}

/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const file, int_t line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(-1);
}


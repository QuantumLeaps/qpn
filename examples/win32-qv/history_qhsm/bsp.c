/*****************************************************************************
* Product: Board Support Package (BSP) for the History example
* Last updated for version 5.4.2
* Last updated on  2015-06-12
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
#include "qpn.h"     /* QP-nano */
#include "bsp.h"     /* Board Support Package */
#include "history.h" /* Application interface */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*..........................................................................*/
/* dummy definition of the QF_active[] array (not used in this example) */
QMActiveCB const Q_ROM QF_active[] = {
    { (QMActive *)0,  (QEvt *)0, 0U }
};

/*..........................................................................*/
void BSP_init(void) {
    printf("History state pattern\nQP-nano version: %s\n"
           "Press 'o' to OPEN  the door\n"
           "Press 'c' to CLOSE the door\n"
           "Press 't' to start TOASTING\n"
           "Press 'b' to start BAKING\n"
           "Press 'f' to turn the oven OFF\n"
           "Press ESC to quit...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_exit(void) {
    printf("\nBye! Bye!");
    fflush(stdout);
    _exit(0);
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
}
/*..........................................................................*/
void QF_onCleanup(void) {
    /* NOTE:
    * This application does not call QF_run(), so it cannot be terminated
    * with the QF_stop() call. Instead, this example uses BSP_exit() to
    * terminate.
    */
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
}

/*..........................................................................*/
/* this function is used by the QP embedded systems-friendly assertions */
void Q_onAssert(char const * const file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
    _exit(-1);
}

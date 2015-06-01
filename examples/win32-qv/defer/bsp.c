/*****************************************************************************
* Product: BSP for QP-nano "Deferred Event" state pattern example, Win32
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-05-24
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
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
* Web  : http://www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#include "qpn.h"     /* QP-nano API */
#include "bsp.h"     /* Board Support Package */
#include "defer.h"   /* Application interface */

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
void BSP_init(void) {
}
/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const file, int_t line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(-1);
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC);
}
/*..........................................................................*/
void QF_onCleanup(void) {
    printf("\nBye!Bye!\n");
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
    QF_tickXISR(0U); /* perform the QF-nano clock tick processing */

    if (_kbhit()) {  /* any key pressed? */
        uint8_t key = _getch();
        switch (key) {
            case 'n': { /* 'n': new request? */
                static uint8_t reqCtr = 0U; /* count the requests */
                QACTIVE_POST_ISR((QMActive *)&AO_TServer,
                                 NEW_REQUEST_SIG, ++reqCtr);
                break;
            }
            case '\33': { /* ESC pressed? */
                QACTIVE_POST_ISR((QMActive *)&AO_TServer,
                                 TERMINATE_SIG, 0U);
                break;
            }
        }
    }
}




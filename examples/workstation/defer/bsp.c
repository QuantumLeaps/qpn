/*****************************************************************************
* Product: BSP for QP-nano "Deferred Event" state pattern example, Win32
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
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"     /* QP-nano API */
#include "bsp.h"     /* Board Support Package */
#include "defer.h"   /* Application interface */

#include <stdlib.h>
#include <stdio.h>

//Q_DEFINE_THIS_FILE

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
        case 'n': { /* 'n': new request? */
            static uint8_t reqCtr = 0U; /* count the requests */
            QACTIVE_POST_ISR((QActive *)&AO_TServer,
                             NEW_REQUEST_SIG, ++reqCtr);
            break;
        }
        case '\33': { /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_TServer,
                             TERMINATE_SIG, 0U);
            break;
        }
    }
}


/*****************************************************************************
* Product: BSP for QP-nano Comp example, Win32
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
#include "qpn.h"
#include "bsp.h"
#include "clock.h"

#include <stdlib.h>
#include <stdio.h>

//Q_DEFINE_THIS_FILE

/*..........................................................................*/
void BSP_init(void) {
    printf("Orthogonal Component state pattern\nQP-nano version: %s\n"
           "Press 'o' to turn the Alarm ON\n"
           "Press 'f' to turn the Alarm OFF\n"
           "Press '0'..'9' to set the Alarm time\n"
           "Press 'A' to set the Clock in 12-hour mode\n"
           "Press 'B' to set the Clock in 24-hour mode\n"
           "Press ESC to quit...\n",
           QP_versionStr);
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
    uint8_t key;

    QF_tickXISR(0U); /* perform the QF-nano clock tick processing */

    key = QF_consoleGetKey();
    switch (key) {
        case 'o': { /* 'o': Alarm on event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                             ALARM_ON_SIG, 0);
            break;
        }
        case 'f': { /* 'f': Alarm off event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                             ALARM_OFF_SIG, 0);
            break;
        }
        case '1':   /* '1' */
        case '2':   /* '2' */
        case '3':   /* '3' */
        case '4':   /* '4' */
        case '5':   /* '5' */
        case '6':   /* '6' */
        case '7':   /* '7' */
        case '8':   /* '8' */
        case '9': { /* '9' */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                              ALARM_SET_SIG, key - '0');
            break;
        }
        case '0': { /* '0' */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                              ALARM_SET_SIG, 0U);
            break;
        }
        case 'a': { /* 'a': Clock 12H event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                              CLOCK_12H_SIG, 0U);
            break;
        }
        case 'b': { /* 'b': Clock 24H event? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                             CLOCK_24H_SIG, 0U);
            break;
        }
        case '\33': { /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_AlarmClock,
                             TERMINATE_SIG, 0U);
            break;
        }
    }
}
/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const file, int_t line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(-1);
}





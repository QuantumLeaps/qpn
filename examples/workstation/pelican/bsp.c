/*****************************************************************************
* Product: BSP for PELICAN crossing example,  Win32-GUI
* Last Updated for Version: 6.3.7
* Date of the Last Update:  2018-11-09
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
#include "qpn.h"     /* QP-nano */
#include "bsp.h"     /* Board Support Package (BSP) */
#include "pelican.h" /* application interface */

#include "safe_std.h" /* portable "safe" <stdio.h>/<string.h> facilities */
#include <stdlib.h>

//Q_DEFINE_THIS_MODULE("bsp")

/*..........................................................................*/
void BSP_init(void) {
    PRINTF_S("Pedestrian LIght CONtrolled crossing example"
           "\nQP-nano %s\n"
           "Press 'p' to generate PED_WAITING\n"
           "Press 'f' to generate OFF\n"
           "Press 'o' to generate ON\n"
           "Press ESC to quit...\n",
           QP_VERSION_STR);
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
    QF_stop();
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    PRINTF_S("STATE: %s -----\n", state);
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            PRINTF_S("%s\n", "cars: RED");
            break;
        }
        case CARS_YELLOW: {
            PRINTF_S("%s\n", "cars: YELLOW");
            break;
        }
        case CARS_GREEN: {
            PRINTF_S("%s\n", "cars: GREEN");
            break;
        }
        case CARS_BLANK: {
            PRINTF_S("%s\n", "cars: BLANK");
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            PRINTF_S("%s\n", "peds: DON'T WALK");
            break;
        }
        case PEDS_BLANK: {
            PRINTF_S("%s\n", "peds: BLANK");
            break;
        }
        case PEDS_WALK: {
            PRINTF_S("%s\n", "peds: WALK");
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    QF_consoleSetup();
    QF_setTickRate(BSP_TICKS_PER_SEC, 50); /* desired tick rate/ticker-prio */
}
/*..........................................................................*/
void QF_onCleanup(void) {
    PRINTF_S("\n%s\n", "Bye! Bye!");
    QF_consoleCleanup();
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
    QF_tickXISR(0U); /* perform the QF-nano clock tick processing */

    switch (QF_consoleGetKey()) {
        case 'p':
            PRINTF_S("%s\n", "-----> PEDS_WAITING");
            QACTIVE_POST_ISR((QActive *)&AO_Pelican,
                              PEDS_WAITING_SIG, 0U);
            break;
        case 'f':
            PRINTF_S("%s\n", "-----> OFF");
            QACTIVE_POST_ISR((QActive *)&AO_Pelican,
                             OFF_SIG, 0U);
            break;
        case 'o':
            PRINTF_S("%s\n", "-----> ON");
            QACTIVE_POST_ISR((QActive *)&AO_Pelican,
                             ON_SIG, 0U);
            break;
        case '\33': /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_Pelican,
                             TERMINATE_SIG, 0U);
            break;
        default:
            break;
    }
}
/*--------------------------------------------------------------------------*/
Q_NORETURN Q_onAssert(char const * const module, int location) {
    QF_INT_DISABLE(); /* cut-off all interrupts */
    FPRINTF_S(stderr, "Assertion failed in %s:%d", module, location);
    BSP_terminate(-1);
}

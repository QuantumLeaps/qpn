/*****************************************************************************
* Product: BSP for PELICAN crossing example, Win32-QV
* Last updated for version 5.4.0
* Last updated on  2015-05-18
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
#include "bsp.h"     /* Board Support Package (BSP) */
#include "pelican.h" /* application interface */

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

Q_DEFINE_THIS_MODULE("bsp")

/*..........................................................................*/
void BSP_init(void) {
    printf("Pedestrian LIght CONtrolled crossing example"
           "\nQP-nano %s\n"
           "Press 'p' to generate PED_WAITING\n"
           "Press 'f' to generate OFF\n"
           "Press 'o' to generate ON\n"
           "Press ESC to quit...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    printf("STATE: %s -----\n", state);
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            printf("cars: RED\n");
            break;
        }
        case CARS_YELLOW: {
            printf("cars: YELLOW\n");
            break;
        }
        case CARS_GREEN: {
            printf("cars: GREEN\n");
            break;
        }
        case CARS_BLANK: {
            printf("cars: BLANK\n");
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            printf("peds: DON'T WALK\n");
            break;
        }
        case PEDS_BLANK: {
            printf("peds: BLANK\n");
            break;
        }
        case PEDS_WALK: {
            printf("peds: WALK\n");
            break;
        }
    }
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
    QF_tickXISR(0U); /* perform the QF clock tick processing */

    if (_kbhit()) {  /* any key pressed? */
        switch (_getch()) {
            case 'p':
                printf("-----> PEDS_WAITING\n");
                QACTIVE_POST_ISR((QMActive *)&AO_Pelican,
                                  PEDS_WAITING_SIG, 0U);
                break;
            case 'f':
                printf("-----> OFF\n");
                QACTIVE_POST_ISR((QMActive *)&AO_Pelican,
                                 OFF_SIG, 0U);
                break;
            case 'o':
                printf("-----> ON\n");
                QACTIVE_POST_ISR((QMActive *)&AO_Pelican,
                                 ON_SIG, 0U);
                break;
            case '\33': /* ESC pressed? */
                QACTIVE_POST_ISR((QMActive *)&AO_Pelican,
                                 TERMINATE_SIG, 0U);
                break;
            default:
                break;
        }
    }
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    QF_INT_DISABLE();                             /* cut-off all interrupts */
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    BSP_terminate(-1);
}

/*****************************************************************************
* Product: BSP for DPP example, Win32
* Last Updated for Version: 4.5.05
* Date of the Last Update:  Mar 28, 2013
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
#include "dpp.h"
#include "bsp.h"

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

Q_DEFINE_THIS_FILE

/* local variables ---------------------------------------------------------*/
static uint32_t l_rnd;                                       /* random seed */

/*..........................................................................*/
void BSP_init(void) {
    printf("Dining Philosopher Problem example\n"
           "QP-nano %s\n"
           "Press 'p' to pause\n"
           "Press ESC to quit...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
    QF_stop();
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char_t const *stat) {
    printf("Philosopher %2d is %s\n", (int)n, stat);
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    printf("%s\n", paused ? "PAUSED" : "RUNNING...");
}
/*..........................................................................*/
uint32_t BSP_random(void) {  /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const Q_ROM_VAR file, int_t line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
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
            QACTIVE_POST_ISR((QActive *)&AO_Table, TERMINATE_SIG, 0U);
        }
        else if (ch == 'p') {
            QACTIVE_POST_ISR((QActive *)&AO_Table, PAUSE_SIG, 0U);
        }
    }
}




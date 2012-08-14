/*****************************************************************************
* Product: DPP example, 80x86, Win32
* Last Updated for Version: 4.5.01
* Date of the Last Update:  Jun 26, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>                                           /* Win32 API */

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Q_DEFINE_THIS_FILE

/* local variables ---------------------------------------------------------*/
static uint8_t  l_running;
static uint32_t l_rnd;                                       /* random seed */

/*..........................................................................*/
static DWORD WINAPI isrThread(LPVOID par) {  /* Win32 thred to emulate ISRs */
    (void)par;                                          /* unused parameter */
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    l_running = (uint8_t)1;
    do {
        Sleep(1000UL / BSP_TICKS_PER_SEC);      /* wait for the tick period */
        QF_tickISR();          /* perform the QF-nano clock tick processing */
    } while (l_running);
    return 0U;                                            /* return success */
}
/*..........................................................................*/
void BSP_init(void) {
    printf("Dining Philosopher Problem example"
           "\nQP-nano %s\n"
           "Press ESC to quit...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_terminate(uint16_t result) {
    QF_stop();
    exit(result);
}
/*..........................................................................*/
void QF_onStartup(void) {
    Q_ALLEGE(CreateThread(NULL, 1024, &isrThread, 0, 0, NULL)
             != (HANDLE)0);                       /* thread must be created */
}
/*..........................................................................*/
void QF_onCleanup(void) {
    l_running = (uint8_t)0;
}
/*..........................................................................*/
void QF_onIdle(void) {                    /* called within critical section */
    QF_INT_ENABLE();
    if (_kbhit()) {                                     /* any key pressed? */
        if (_getch() == 0x1B) {               /* see if the ESC key pressed */
            QActive_post((QActive *)&AO_Table, TERMINATE_SIG, 0U);
        }
    }
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
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
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    QF_stop();
}

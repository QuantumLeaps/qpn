/*****************************************************************************
* Product: BSP for DPP example, Linux emulation
* Last Updated for Version: 5.11
* Date of the Last Update:  Oct 12, 2013
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>                            /* for memcpy() and memset() */
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

Q_DEFINE_THIS_MODULE("bsp")

/* Local-scope objects -----------------------------------------------------*/
static struct termios l_tsav;   /* structure with saved terminal attributes */
static uint32_t l_rnd;                                       /* random seed */

/*..........................................................................*/
void BSP_init(void) {
    printf("Dining Philosopher Problem example"
           "\nQP-nano %s\n"
           "Press 'p' to pause\n"
           "Press ESC to quit...\n",
           QP_getVersion());
    BSP_randomSeed(1234U);
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char_t const *stat) {
    printf("Philosopher %2d is %s\n", (int)n, stat);
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    printf("\nBye! Bye!\n");
    QF_stop();
    (void)result;
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
void QF_onStartup(void) {
    struct termios tio;                     /* modified terminal attributes */

    tcgetattr(0, &l_tsav);          /* save the current terminal attributes */
    tcgetattr(0, &tio);           /* obtain the current terminal attributes */
    tio.c_lflag &= ~(ICANON | ECHO);   /* disable the canonical mode & echo */
    tcsetattr(0, TCSANOW, &tio);                  /* set the new attributes */

    QF_setTickRate(BSP_TICKS_PER_SEC);         /* set the desired tick rate */
}
/*..........................................................................*/
void QF_onCleanup(void) {
    tcsetattr(0, TCSANOW, &l_tsav);/* restore the saved terminal attributes */
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
    struct timeval timeout = { 0 };                 /* timeout for select() */
    fd_set con;                          /* FD set representing the console */

    QF_tickXISR(0U);                /* perform the QF clock tick processing */

    FD_ZERO(&con);
    FD_SET(0, &con);
    /* check if a console input is available, returns immediately */
    if (0 != select(1, &con, 0, 0, &timeout)) {      /* any descriptor set? */
        char ch;
        read(0, &ch, 1);
        if (ch == '\33') {                                  /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_Table, TERMINATE_SIG, 0U);
        }
        else if (ch == 'p') {
            QACTIVE_POST_ISR((QActive *)&AO_Table, PAUSE_SIG, 0U);
        }
    }
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    QF_INT_DISABLE();                             /* cut-off all interrupts */
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    BSP_terminate(-1);
}

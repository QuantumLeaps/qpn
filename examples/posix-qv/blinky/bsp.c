/*****************************************************************************
* Product: BSP for Blinky example, POSIX with cooperative QV kernel
* Last updated for version 5.4.0
* Last updated on  2015-05-24
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
#include "qpn.h"
#include "bsp.h"
#include "blinky.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for memcpy() and memset() */
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
static struct termios l_tsav; /* structure with saved terminal attributes */

/*..........................................................................*/
void BSP_init() {
    printf("Simple Blinky example\nQP-nano version: %s\n"
           "Press ESC to quit...\n",
           QP_VERSION_STR);
}
/*..........................................................................*/
void BSP_ledOff() {
    printf("OFF\n");
}
/*..........................................................................*/
void BSP_ledOn() {
    printf("ON\n");
}
/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const file, int_t line) {
    fprintf(stderr, "\nAssertion failed in %s, line %d\n", file, line);
    exit(-1);
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    struct termios tio;  /* modified terminal attributes */

    tcgetattr(0, &l_tsav); /* save the current terminal attributes */
    tcgetattr(0, &tio);    /* obtain the current terminal attributes */
    tio.c_lflag &= ~(ICANON | ECHO); /* disable the canonical mode & echo */
    tcsetattr(0, TCSANOW, &tio); /* set the new attributes */

    QF_setTickRate(BSP_TICKS_PER_SEC);
}
/*..........................................................................*/
void QF_onCleanup(void) {
    /* restore the saved terminal attributes */
    tcsetattr(0, TCSANOW, &l_tsav);
    printf("\nBye! Bye!\n");
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
    struct timeval timeout = { 0U, 0U }; /* timeout for select() */
    fd_set con; /* FD set representing the console */

    QF_tickXISR(0U); /* perform the QF-nano clock tick processing */

    FD_ZERO(&con);
    FD_SET(0, &con);
    /* check if a console input is available, returns immediately */
    if (0 != select(1, &con, 0, 0, &timeout)) { /* any descriptor set? */
        char ch;
        read(0, &ch, 1);
        if (ch == '\33') { /* ESC pressed? */
            QF_stop();
        }
    }
}





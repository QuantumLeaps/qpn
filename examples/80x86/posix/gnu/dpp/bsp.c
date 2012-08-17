/*****************************************************************************
* Product: DPP example, Linux emulation
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Jun 29, 2012
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>                            /* for memcpy() and memset() */
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

Q_DEFINE_THIS_MODULE("bsp")

/* Local-scope objects -----------------------------------------------------*/
static struct termios l_tsav;   /* structure with saved terminal attributes */
static uint8_t l_running;                          /* for the ticker thread */
static uint32_t l_rnd;                                       /* random seed */

/*..........................................................................*/
/* The ticker thread emulates the interrupt level. It executes QF_tick
* and other functions related to timing.
*/
static void *tickerThread(void *par) {   /* the expected P-Thread signature */
    (void)par;                                          /* unused parameter */
    l_running = 1;
    while (l_running) {
        struct timeval timeout = { 0 };             /* timeout for select() */
        fd_set con;                      /* FD set representing the console */
        FD_ZERO(&con);
        FD_SET(0, &con);
        timeout.tv_usec = 8000;

        /* sleep for the full tick or until a console input arrives */
        if (0 != select(1, &con, 0, 0, &timeout)) {  /* any descriptor set? */
            char ch;
            read(0, &ch, 1);
            if (ch == '\33') {                              /* ESC pressed? */
                QF_stop();
            }
        }

        QF_INT_DISABLE(); /* enter critical section to emulate entry to ISR */
        QF_tickISR();               /* perform the QF clock tick processing */
        QF_INT_ENABLE(); /*leave critical section, to emulate exit from ISR */
    }
    return (void *)0;                                     /* return success */
}

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
    QF_stop();
    exit(result);
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
    pthread_attr_t attr;
    struct sched_param param;
    pthread_t ticker;

    tcgetattr(0, &l_tsav);          /* save the current terminal attributes */
    tcgetattr(0, &tio);           /* obtain the current terminal attributes */
    tio.c_lflag &= ~(ICANON | ECHO);   /* disable the canonical mode & echo */
    tcsetattr(0, TCSANOW, &tio);                  /* set the new attributes */

    /* SCHED_FIFO corresponds to real-time preemptive priority-based scheduler
    * NOTE: This scheduling policy requires the superuser priviledges
    */
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);

    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&ticker, &attr, &tickerThread, 0) != 0) {
               /* Creating the p-thread with the SCHED_FIFO policy failed.
               * Most probably this application has no superuser privileges,
               * so we just fall back to the default SCHED_OTHER policy
               * and priority 0.
               */
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        param.sched_priority = 0;
        pthread_attr_setschedparam(&attr, &param);
        Q_ALLEGE(pthread_create(&ticker, &attr, &tickerThread, 0) == 0);
    }
    pthread_attr_destroy(&attr);
}
/*..........................................................................*/
void QF_onCleanup(void) {
    l_running = 0;                           /* terminate the ticker thread */
    tcsetattr(0, TCSANOW, &l_tsav);/* restore the saved terminal attributes */
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();                             /* cut-off all interrupts */
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    BSP_terminate(-1);
}

/*****************************************************************************
* NOTE01:
* The sofware interrupt generated at the end of the tick ISR invokes the
* original DOS tick ISR handler, which was installed at the spare vector
* 0x81. The DOS tick ISR handler generates the EOI (End-Of-Interrupt)
* to the master 8259A PIC.
*
* NOTE02:
* The QF_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally enable interrupts, ideally atomically
* with putting the CPU to the power-saving mode.
*/

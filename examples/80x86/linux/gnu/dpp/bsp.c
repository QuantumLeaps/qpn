/*****************************************************************************
* Product: DPP example, Linux emulation
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
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
static uint32_t l_delay = 0UL; /* limit for the loop counter in busyDelay() */
static uint8_t l_running;                          /* for the ticker thread */

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
void BSP_init(int argc, char *argv[]) {
    if (argc > 1) {
        l_delay = atol(argv[1]);         /* set the delay from the argument */
    }
    printf("Dining Philosopher Problem example"
           "\nQP-nano %s\n"
           "Press ESC to quit...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_displyPhilStat(uint8_t n, char const *stat) {
    printf("Philosopher %2d is %s\n", (int)n, stat);
}
/*..........................................................................*/
void BSP_busyDelay(void) {
    uint32_t volatile i = l_delay;
    while (i-- > 0UL) {                                   /* busy-wait loop */
    }
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
    QF_stop();
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

/*****************************************************************************
* Product: DPP example with QK-nano
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
#include "video.h"

#include <dos.h>                       /* for _dos_setvect()/_dos_getvect() */
#include <conio.h>                                      /* for inp()/outp() */
#include <stdlib.h>                                          /* for _exit() */
#include <stdio.h>

Q_DEFINE_THIS_MODULE("bsp")

/* Local-scope objects -----------------------------------------------------*/
static void interrupt (*l_dosTmrISR)();
static void interrupt (*l_dosKbdISR)();
static uint32_t l_delay = 0UL; /* limit for the loop counter in busyDelay() */

#define TMR_VECTOR      0x08
#define KBD_VECTOR      0x09

#define TMR_ISR_PRIO    (0xFF)
#define KBD_ISR_PRIO    (0xFF - 1)

static void dispPreemptions(uint8_t pisr);                   /* for testing */

/*..........................................................................*/
static void interrupt tmrISR(void) {
    dispPreemptions(TMR_ISR_PRIO);              /* for testing only, NOTE01 */
    QK_ISR_ENTRY();                /* inform QK-nano about entering the ISR */

    QF_tickISR();                          /* process all armed time events */
    BSP_busyDelay();                                 /* for testing, NOTE02 */

    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}
/*..........................................................................*/
static void interrupt kbdISR() {
    uint8_t key = inp(0x60);      /* key scan code from 8042 kbd controller */
    uint8_t kcr = inp(0x61);               /* get keyboard control register */

    dispPreemptions(KBD_ISR_PRIO);              /* for testing only, NOTE01 */
    QK_ISR_ENTRY();                /* inform QK-nano about entering the ISR */

    outp(0x61, (uint8_t)(kcr | 0x80));       /* toggle acknowledge bit high */
    outp(0x61, kcr);                          /* toggle acknowledge bit low */

    if (key == (uint8_t)129) {                          /* ESC key pressed? */
        QActive_postISR((QActive *)&AO_Table, TERMINATE_SIG, 0);
    }
    Video_printNumAt(60, 12 + 0, VIDEO_FGND_YELLOW, key);/* display the key */

    BSP_busyDelay();                                 /* for testing, NOTE02 */

    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}
/*..........................................................................*/
void BSP_init(int argc, char *argv[]) {
    uint8_t n;

    if (argc > 1) {
        l_delay = atol(argv[1]);    /* set the delay counter for busy delay */
    }
    Video_clearScreen(VIDEO_BGND_BLACK);
    Video_clearRect( 0,  0, 80,  7, VIDEO_BGND_LIGHT_GRAY);
    Video_clearRect( 0, 11, 80, 12, VIDEO_BGND_LIGHT_GRAY);
    Video_clearRect( 0, 12, 41, 23, VIDEO_BGND_BLUE);
    Video_clearRect(41, 12, 80, 23, VIDEO_BGND_RED);
    Video_clearRect( 0, 23, 80, 24, VIDEO_BGND_LIGHT_GRAY);

    n = VIDEO_FGND_BLUE;
    Video_printStrAt(10, 0, n, "  __");
    Video_printStrAt(10, 1, n, " /  |      _   _ -|-     _ _");
    Video_printStrAt(10, 2, n, " \\__| | |  _\\ | \\ | | | | \\ \\");
    Video_printStrAt(10, 3, n, "    | \\_/ |_| | | | \\_| | | |");
    Video_printStrAt(10, 4, n, "    |");
    n = VIDEO_FGND_RED;
    Video_printStrAt(43, 0, n, "    _       __ ");
    Video_printStrAt(43, 1, n, "|  /_\\     |  \\  TM");
    Video_printStrAt(43, 2, n, "|  \\_   _  |__/ _");
    Video_printStrAt(43, 3, n, "|       _\\ |   |_");
    Video_printStrAt(43, 4, n, "|___   |_| |    _|");
    Video_printStrAt(10, 5, VIDEO_FGND_BLUE,
                     "_____________________________________________________");
    Video_printStrAt(10, 6, VIDEO_FGND_RED,
                     "i n n o v a t i n g   e m b e d d e d   s y s t e m s");
    Video_printStrAt(18,  7, VIDEO_FGND_WHITE,
                     "Dining Philosophers Problem (DPP)");
    Video_printStrAt(18,  8, VIDEO_FGND_WHITE, "QP-nano (QK) ");
    Video_printStrAt(32,  8, VIDEO_FGND_YELLOW, QP_getVersion());
    Video_printStrAt(41, 10, VIDEO_FGND_WHITE, "Delay Counter");
    Video_printNumAt(54, 10, VIDEO_FGND_YELLOW, l_delay);

    Video_printStrAt( 1, 11, VIDEO_FGND_BLUE,
                     "Active Object   State     Preemptions");

    Video_printStrAt(42, 11, VIDEO_FGND_RED,
                     "ISR      Calls    Data    Preemptions");
    for (n = 0; n < N_PHILO; ++n) {
        Video_printStrAt( 1, 12 + n, VIDEO_FGND_WHITE, "Philosopher");
        Video_printNumAt(12, 12 + n, VIDEO_FGND_WHITE, n);
    }
    Video_printStrAt( 1, 12 + N_PHILO, VIDEO_FGND_WHITE,  "Table");
    Video_printStrAt(17, 12 + N_PHILO, VIDEO_FGND_YELLOW, "serving");
    Video_printStrAt( 1, 12 + N_PHILO + 1, VIDEO_FGND_WHITE,  "KbdMgr");
    Video_printStrAt(17, 12 + N_PHILO + 1, VIDEO_FGND_YELLOW, "active");
    Video_printStrAt( 1, 12 + N_PHILO + 3, VIDEO_FGND_WHITE,  "Locked Sched");

    Video_printStrAt(42, 12 + 0, VIDEO_FGND_WHITE,  "kbdISR");
    Video_printStrAt(42, 12 + 1, VIDEO_FGND_WHITE,  "tmrISR");

    Video_printStrAt(10, 23, VIDEO_FGND_BLUE,
         "* Copyright (c) Quantum Leaps, LLC * www.quantum-leaps.com *");
    Video_printStrAt(28, 24, VIDEO_FGND_LIGHT_RED,
         "<< Press Esc to quit >>");
}
/*..........................................................................*/
void BSP_displyPhilStat(uint8_t n, char const *stat) {
    Video_printStrAt(17, 12 + n, VIDEO_FGND_YELLOW, stat);
}
/*..........................................................................*/
void BSP_busyDelay(void) {
    uint32_t volatile i = l_delay;
    while (i-- > 0UL) {                                   /* busy-wait loop */
    }
}
/*..........................................................................*/
void dispPreemptions(uint8_t pisr) {             /* for testing, see NOTE01 */
    if (pisr == TMR_ISR_PRIO) {
        static uint32_t tmrIsrCtr;               /* timer interrupt counter */
        Video_printNumAt(51, 12 + 1, VIDEO_FGND_YELLOW, ++tmrIsrCtr);
    }
    else if (pisr == KBD_ISR_PRIO) {
        static uint32_t kbdIsrCtr;                 /* kbd interrupt counter */
        Video_printNumAt(51, 12 + 0, VIDEO_FGND_YELLOW, ++kbdIsrCtr);
    }
    else {
        Q_ERROR();                         /* unexpected interrupt priority */
    }

#ifdef QF_ISR_NEST
    if (QK_intNest_ == (uint8_t)0) {          /* is this a task preemption? */
        if (QK_currPrio_ > (uint8_t)0) {
            static uint32_t preCtr[QF_MAX_ACTIVE + 1];
            Video_printNumAt(30, 12 + QK_currPrio_ - 1, VIDEO_FGND_YELLOW,
                             ++preCtr[QK_currPrio_]);
        }
    }
    else if (QK_intNest_ == (uint8_t)1) {      /* this is an ISR preemption */
        if (pisr == TMR_ISR_PRIO) {          /* TMR_ISR preempting KBD_ISR? */
            static uint32_t kbdPreCtr;        /* kbd ISR preemption counter */
            Video_printNumAt(71, 12 + 0, VIDEO_FGND_YELLOW, ++kbdPreCtr);
        }
        else {
            static uint32_t tmrPreCtr;        /* tmr ISR preemption counter */
            Video_printNumAt(71, 12 + 1, VIDEO_FGND_YELLOW, ++tmrPreCtr);
        }
    }
    else {
        Q_ERROR();         /* impossible ISR nesting level with just 2 ISRs */
    }
#else
    if (QK_currPrio_ > (uint8_t)0) {
        static uint32_t preCtr[QF_MAX_ACTIVE + 1];
        Video_printNumAt(30, 12 + QK_currPrio_ - 1, VIDEO_FGND_YELLOW,
                         ++preCtr[QK_currPrio_]);
    }
#endif
}
/*..........................................................................*/
void QF_onStartup(void) {
                                      /* save the origingal DOS vectors ... */
    l_dosTmrISR = _dos_getvect(TMR_VECTOR);
    l_dosKbdISR = _dos_getvect(KBD_VECTOR);

    QF_INT_DISABLE();
    _dos_setvect(TMR_VECTOR, &tmrISR);
    _dos_setvect(KBD_VECTOR, &kbdISR);
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QF_stop(void) {
                                    /* restore the original DOS vectors ... */
    if (l_dosTmrISR != (void interrupt (*)(void))0) { /* DOS vectors saved? */
        QF_INT_DISABLE();
        _dos_setvect(TMR_VECTOR, l_dosTmrISR);
        _dos_setvect(KBD_VECTOR, l_dosKbdISR);
        QF_INT_ENABLE();
    }
                                                       /* clear the display */
    //Video_clearScreen(VIDEO_BGND_BLACK | VIDEO_FGND_LIGHT_GRAY);
    _exit(0);                                                /* exit to DOS */
}
/*..........................................................................*/
void QK_onIdle(void) {
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();                             /* cut-off all interrupts */
    Video_clearRect(0, 24, 80, 25, VIDEO_BGND_RED);
    Video_printStrAt(0, 24, VIDEO_FGND_WHITE, "ASSERTION FAILED in file:");
    Video_printStrAt(26, 24, VIDEO_FGND_YELLOW, file);
    Video_printStrAt(57, 24, VIDEO_FGND_WHITE, "line:");
    Video_printNumAt(62, 24, VIDEO_FGND_YELLOW, line);
    QF_stop();
}

/*****************************************************************************
* NOTE01:
* The function call to displayPreemptions() is added only to monitor the
* "asynchronous" preemptions within the QK-nano.
*
* NOTE02:
* The call to BSP_busyDelay() (see main.c) is added only to extend the
* execution time of the code to increase the chance of an "asynchronous"
* preemption.
*/

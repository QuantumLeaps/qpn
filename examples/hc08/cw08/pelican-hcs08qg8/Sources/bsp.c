/*****************************************************************************
* Product: BSP for PELICAN crossing example
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 02, 2012
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
#include "bsp.h"
#include "pelican.h"

                                                        /* define the LED's */
#define LED1      PTBD_PTBD6
#define LED2      PTBD_PTBD7
#define LED_ON    ((uint8_t)0)
#define LED_OFF   ((uint8_t)1)

                 /* CPU clock [Hz], 4 MHz internal oscillator out of reset  */
#define BSP_CPU_HZ           4000000

/*..........................................................................*/
interrupt 12 void  MTIM_ISR(void) {
    MTIMSC_TOF = 0;                                            /* clear TOF */
    QF_tick();
}
/*..........................................................................*/
void BSP_init(void) {
    ICSC2_BDIV = 3;

    PTBDD_PTBDD6 = 1;                      /* Set PTBDD_PTBDD6 as an output */
    PTBDD_PTBDD7 = 1;                      /* Set PTBDD_PTBDD7 as an output */
    LED1 = LED_OFF;                         /* Port B6 is connected to LED1 */
    LED2 = LED_OFF;                         /* Port B7 is connected to LED2 */

                                                              /* MTIM setup */
    MTIMCLK_PS = 8;                        /* select MTIM prescaler to 1/32 */
    MTIMCLK_CLKS = 0;
    MTIMMOD = (uint8_t)(((BSP_CPU_HZ / BSP_TICKS_PER_SEC / 32) - 1) >> 8);
    MTIMMOD = (uint8_t)(((BSP_CPU_HZ / BSP_TICKS_PER_SEC / 32) - 1) & 0xFF);

//    SOPT1_COPE = 0;    /* Disable "Computer Operating Correctly" (Watchdog) */
}
/*..........................................................................*/
void QF_onStartup(void) {
    MTIMSC = 0x60;                     /* reset and start MTIM, enable ints */
}
/*..........................................................................*/
void QF_onIdle(void) {                    /* entered with interrupts LOCKED */
    __RESET_WATCHDOG();                                /* feed the watchdog */

#ifdef NDEBUG
    __asm WAIT;                          /* enter the WAIT mode, see NOTE01 */
#else
    QF_INT_ENABLE();                              /* only enable interrupts */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();                             /* cut off the interrupts */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
        __RESET_WATCHDOG();                            /* feed the watchdog */
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED:       LED2 = LED_OFF; break;
        case CARS_YELLOW:    LED2 = LED_ON;  break;
        case CARS_GREEN:     LED2 = LED_ON;  break;
        case CARS_OFF:       LED2 = LED_OFF; break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: LED1 = LED_ON;  break;
        case PEDS_BLANK:     LED1 = LED_OFF; break;
        case PEDS_WALK:      LED1 = LED_OFF; break;
    }
}

/*****************************************************************************
* NOTE01:
* As described in the "HC(S)08 Datasheet" the WAIT instruction as well as
* the STOP instruction ATOMICALLY enable the interrupts.
*/

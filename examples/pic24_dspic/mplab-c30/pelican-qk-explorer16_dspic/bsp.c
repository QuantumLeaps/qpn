/*****************************************************************************
* BSP for Explorer 16 board with dsPIC33FJ256GP710, QK-nano kernel
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

Q_DEFINE_THIS_FILE

                  /* prmiary oscillator frequency for the Explorer 16 board */
#define BSP_PRIMARY_OSC_HZ      8000000UL
                                  /* CPU frequency after the PLL (FOSC / 2) */
#define BSP_FCY_HZ              25000000UL


#define LED_ON(leds_)           (LATA |= (leds_))
#define LED_OFF(leds_)          (LATA &= ~(leds_))

#define IDLE_LED                (1 << 0)
#define CARS_RED_LED            (1 << 2)
#define CARS_YLW_LED            (1 << 3)
#define CARS_GRN_LED            (1 << 4)
#define PEDS_WALK_LED           (1 << 6)
#define PEDS_DONT_LED           (1 << 7)

/* ISRs --------------------------------------------------------------------*/
#define TIMER2_ISR_PRIO         4
#define INT0_ISR_PRIO           6

QK_ISR(no_auto_psv) _T2Interrupt() {
    _T2IF = 0;                              /* clear Timer 2 interrupt flag */
    QF_tick();                   /* handle all armed time events in QF-nano */

    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}
/*..........................................................................*/
QK_ISR(auto_psv) _INT0Interrupt() {
    _INT0IF = 0;
    QActive_postISR(&AO_Ped, PEDS_WAITING_SIG, 0);

    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}

/*--------------------------------------------------------------------------*/
void BSP_init(void) {
    RCONbits.SWDTEN = 0;                                /* disable Watchdog */

                             /* set up the oscillator and PLL as follows... */
    PLLFBDbits.PLLDIV  = 160;
    CLKDIVbits.PLLPRE  = 8;
    CLKDIVbits.PLLPOST = 2;
    CLKDIVbits.FRCDIV  = 0;
    CLKDIVbits.DOZE    = 0;
    CLKDIVbits.ROI     = 0;

    LATA  = 0xFF00;            /* set LEDs (D3-D10/RA0-RA7) drive state low */
    TRISA = 0xFF00;             /* set LED pins (D3-D10/RA0-RA7) as outputs */
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */
    T2CON  = 0x0010;    /* Internal Osc (Fcy), 16 bit mode, prescaler = 1:8 */
    TMR2   = 0;      /* Start counting from 0 and clear the prescaler count */
    PR2    = (BSP_FCY_HZ / 8 / BSP_TICKS_PER_SEC) - 1;    /* Timer 2 period */
    _T2IP  = TIMER2_ISR_PRIO;             /* set Timer 2 interrupt priority */
    _T2IF  = 0;                          /* clear the interrupt for Timer 2 */
    _T2IE  = 1;                             /* enable interrupt for Timer 2 */
    T2CONbits.TON = 1;                                     /* start Timer 2 */


    INTCON2bits.INT0EP = 0;              /* INT0 interrupt on positive edge */
    _INT0IP = INT0_ISR_PRIO;                 /* set INT0 interrupt priority */
    _INT0IF = 0;                            /* clear the interrupt for INT0 */
    _INT0IE = 1;                                   /* enable INT0 interrupt */
}
/*..........................................................................*/
void QK_onIdle(void) {

    QF_INT_DISABLE();
    LED_ON (IDLE_LED);                    /* blink the IDLE LED, see NOTE01 */
    LED_OFF(IDLE_LED);
    QF_INT_ENABLE();

#ifdef NDEBUG
    Idle();                                      /* transition to Idle mode */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();               /* make sure that interrupts are locked */
    for (;;) {
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    LED_OFF(CARS_RED_LED | CARS_YLW_LED | CARS_GRN_LED);
    switch (sig) {
        case CARS_OFF:
            break;
        case CARS_RED:
            LED_ON(CARS_RED_LED);
            break;
        case CARS_YELLOW:
            LED_ON(CARS_YLW_LED);
            break;
        case CARS_GREEN:
            LED_ON(CARS_GRN_LED);
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    LED_OFF(PEDS_WALK_LED | PEDS_DONT_LED);
    switch (sig) {
        case PEDS_BLANK:
            break;
        case PEDS_WALK:
            LED_ON(PEDS_WALK_LED);
            break;
        case PEDS_DONT_WALK:
            LED_ON(PEDS_DONT_LED);
            break;
    }
}

/*****************************************************************************
* NOTE01:
* The LED 0 (D3 on the Explorer 16 board) is used to visualize the idle loop
* activity. The brightness of the LED is proportional to the frequency of
* invcations of the idle loop. Please note that the LED is toggled with
* interrupts locked, so no interrupt execution time contributes to the
* brightness of the User LED.
*
*/


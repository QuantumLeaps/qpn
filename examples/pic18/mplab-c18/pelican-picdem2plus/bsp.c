/*****************************************************************************
* BSP for PICDEM 2 PLUS with PIC18F452
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
#include "bsp.h"
#include "pelican.h"

#define LED_GRN(state_)      (LATBbits.LATB0 = (state_))
#define LED_YLW(state_)      (LATBbits.LATB1 = (state_))
#define LED_RED(state_)      (LATBbits.LATB2 = (state_))
#define LED_PED(state_)      (LATBbits.LATB3 = (state_))

#define BSP_FOSC_HZ          4000000UL
#define BSP_TMR1_HZ          32768UL
#define SYSTEM_TICK_TOUT     (0x10000 - BSP_TMR1_HZ/BSP_TICKS_PER_SEC)

/*..........................................................................*/
void BSP_init(void) {
    TRISB = 0;                  /* data direction for Port B (LEDs): output */

    LED_RED(0);
    LED_YLW(0);
    LED_GRN(0);
    LED_PED(0);
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */

    RCONbits.IPEN = 1;                            /* enable priority levels */

    TMR1H = 0;
    TMR1L = 1;                              /* make TMR1 exprie immediately */
    T1CON = 0x8F;      /* ext Timer1 clock, 16-bit, no synch, prescaler 1:1 */
    IPR1bits.TMR1IP = 1;             /* set TMR1 interrupt as high priority */
    PIE1bits.TMR1IE = 1;                           /* enable TMR1 interrupt */
}
/*..........................................................................*/
void QF_onIdle(void) {        /* entered with interrupts locked, see NOTE01 */

#ifdef NDEBUG
    Sleep();                        /* transition to SLEEP mode, see NOTE02 */
#endif

    QF_INT_ENABLE();                       /* enable interrupts, see NOTE01 */
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();             /* make sure that interrupts are disabled */
    for (;;) {
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_BLANK:
            LED_RED(0);
            LED_YLW(0);
            LED_GRN(0);
            break;
        case CARS_RED:
            LED_RED(1);
            LED_YLW(0);
            LED_GRN(0);
            break;
        case CARS_YELLOW:
            LED_RED(0);
            LED_YLW(1);
            LED_GRN(0);
            break;
        case CARS_GREEN:
            LED_RED(0);
            LED_YLW(0);
            LED_GRN(1);
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK:
            LED_PED(1);
            break;
        case PEDS_BLANK:
            LED_PED(0);
            break;
        case PEDS_WALK:
            LED_PED(0);
            break;
    }
}

/*..........................................................................*/
#pragma code
#pragma interrupt ISR_hi

void ISR_hi(void) {                                       /* high-level ISR */

    if (PIR1bits.TMR1IF) {                               /* TMR1 interrupt? */
        PIR1bits.TMR1IF = 0;                   /* clear TMR1 interrupt flag */

                                          /* program TMR1 for the next tick */
        TMR1H = (uint8_t)(SYSTEM_TICK_TOUT >> 8);
        TMR1L = (uint8_t)(SYSTEM_TICK_TOUT);

        QF_tickISR();            /* handle all armed time events in QF-nano */
    }
    /*
    else if (...) {
        // handle other interrupt source...
    }
    */
}

#pragma code intVector = 0x08
void intVector_hi(void) {                    /* high-level Interrupt Vector */
    _asm
        goto ISR_hi                                   /* jump to the ISR_hi */
    _endasm
}

/*****************************************************************************
* NOTE01:
* The callback function QF_onIdle() is called with interrupts locked, because
* the idle condition can be invalidated by any enabled interrupt that would
* post events. The QF_onIdle() function *must* enable interrupts internally
*
* NOTE02:
* The low-power mode must be entered with interrupts disabled to avoid
* an interrupt to preempt the transition to the low-power mode. According to
* the "PIC18FXX2 Data Sheet", the Sleep mode can be exited even if the global
* inetrrupt enable flag is cleared (INTCON<7> == 0). This allows for
* an atomic transition to the SLEEP mode.
*
* Selected interrupts, such as Timer1 interrupt with external clock (not
* synchronized to the CPU clock) can wake up the CPU from the SLEEP mode.
* after waking up, the CPU does not service the interrupt immediately,
* because interrupts are still disabled. The interrupt gets serviced only
* after the CPU enables interrupts.
*/


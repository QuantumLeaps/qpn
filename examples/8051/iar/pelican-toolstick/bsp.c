/*****************************************************************************
* PELICAN crossing example
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
#include "pelican.h"
#include "bsp.h"

#define LED_RED(brightness_) (PCA0CPH0 = 0xFF - (brightness_))
#define LED_GRN(brightness_) (PCA0CPH1 = 0xFF - (brightness_))

/* global objects ----------------------------------------------------------*/
#ifdef NDEBUG
uint8_t volatile __bdata QF_pcon;                   /* PCON shadow register */
#endif

/*..........................................................................*/
#pragma vector=0x2B
__interrupt void timer2_ISR(void) {
    TMR2CN &= ~(1 << 7);         /* clear the Timer2 interrupt-pending flag */
    QF_tick();                   /* handle all armed time events in QF-nano */

#ifdef NDEBUG
    QF_pcon = PCON;  /* disable Idle mode after retrun from the ISR, NOTE02 */
#endif
}
/*..........................................................................*/
void BSP_init(void) {
    OSCICN = 0x04;                      /* set SYSCLK as Int Osc 24.5/8 MHz */
    RSTSRC = 0x00;                        /* disable missing clock detector */

    PCA0CN = 0x40;                       /* Enable PCA Counter/Timer (PCA0) */
    PCA0MD = 0x00;  /* Set PCA timebase = SYSCLK / 12, disable watchdog WDT */

                                     /* Duty cycle = (256 - PCA0CPHn) / 256 */
    PCA0CPH0  = 0x00;      /* Config Mod0 PWM for a 100% initial duty cycle */
    PCA0CPH1  = 0x00;      /* Config Mod1 PWM for a 100% initial duty cycle */

    PCA0CPM0  = 0x42;                           /* Enable 8-bit PWM on CEX0 */
    PCA0CPM1  = 0x42;                           /* Enable 8-bit PWM on CEX1 */

    /* Configure the Crossbar and GPIO ports.
    * P0.0  -  Skipped
    * P0.1  -  Skipped
    * P0.2  -  CEX0 (PCA),  Push-Pull,  Digital  (Red LED)
    * P0.3  -  CEX1 (PCA),  Push-Pull,  Digital  (Green LED)
    * P0.4  -  Unassigned
    * P0.5  -  Unassigned
    * P0.6  -  Unassigned
    * P0.7  -  Unassigned
    */
    P0MDOUT = 0x0C;              /* Config P0.2 & P0.3 as push-pull outputs */
    XBR0    = 0x03;                                     /* Skip P0.0 & P0.1 */
    XBR1    = 0x80;             /* Enable PCA Capture/Compare Outputs 0 & 1 */
    XBR2    = 0x40;                                      /* Enable Crossbar */

    IE_bit.ET2 = 0;                            /* disable Timer2 interrupts */
    TMR2CN  = 0x00;                    /* Stop Timer2; Clear TF2, SYSCLK/12 */
    CKCON  &= ~0x60;                      /* Timer2 clocked based on T2XCLK */

                                                            /* reload value */
    TMR2RLL = (uint8_t)(0x10000 - (BSP_MCK/12)/BSP_TICKS_PER_SEC);
    TMR2RLH = (0x10000 - (BSP_MCK/12)/BSP_TICKS_PER_SEC) >> 8;

    TMR2L    = TMR2RLL;                /* Initialize Timer2 to reload value */
    TMR2H    = TMR2RLH;                /* Initialize Timer2 to reload value */
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */

    /* Set the priority of interrupts for your application by writing to the
    * IP and EIP1 registers. Here the pririties are left at the default
    * level (low) ...
    */

    IE_bit.ET2 = 1;                             /* Enable Timer2 interrupts */
    TMR2CN  = (1 << 2);                                     /* Start Timer2 */
}
/*..........................................................................*/
void QF_onIdle(void) {                  /* entered with interrupts disabled */
#ifdef NDEBUG
    QF_pcon = PCON | 0x01;    /* choose low-power mode for your app, NOTE02 */
    QF_INT_ENABLE();                       /* unlock interrupts, see NOTE01 */
    PCON = QF_pcon;                          /* go to low-power, see NOTE02 */
#else
    QF_INT_ENABLE();                       /* enable interrupts, see NOTE01 */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();               /* make sure that interrupts are locked */
    LED_RED(255);
    LED_GRN(0);
    for (;;) {
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_OFF:                        /* intentionally fall-through */
        case CARS_RED:
            LED_GRN(0);
            break;
        case CARS_YELLOW:
            LED_GRN(25);
            break;
        case CARS_GREEN:
            LED_GRN(255);
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK:
            LED_RED(255);
            break;
        case PEDS_BLANK:
            LED_RED(25);
            break;
        case PEDS_WALK:
            LED_RED(0);
            break;
    }
}

/*****************************************************************************
* NOTE01:
* The callback function QF_onIdle() is called with interrupts locked, because
* the idle condition can be invalidated by any enabled interrupt that would
* post events. The QF_onIdle() function *must* unlock interrupts internally
*
* NOTE02:
* The low-power mode must be entered with interrupts disabled to avoid
* an interrupt to preempt the transition to the low-power mode. However,
* the 8051 and 80251 architecture does not allow an *atomic* transition to
* the Idle mode with simultaneous enabling interrupts. Instead, here
* the PCON register is shadowed in the QF_pcon bdata variable. The idle mode
* is set first in the shadow register QF_pcon. At the end of every interrupt
* (see timer2_ISR()), the ISR copies the PCON value into the QF_pcon
* shadow (the 8051/251 clears the IDL and PD bits in the PCON register upon
* interrupt entry). Later in the QF_onIdle(), the PCON register is restored
* from the shadow QF_pcon, which the 8051/251 performs *atomically* as a
* single machine instruction. If the interrupt was serviced between enabling
* interrupts and restoring PCON from the shadow, the IDL/PD bits will be
* cleared in the shadow, so the machine will *not* go to the idle mode, as
* expected.
*/


/*****************************************************************************
* PELICAN crossing example with QK-nano
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

/*..........................................................................*/
#pragma vector=0x2B
__interrupt void timer2_ISR(void) {
    QK_ISR_ENTRY();          /* inform QK-nano that an ISR has been entered */
    TMR2CN &= ~(1 << 7);         /* clear the Timer2 interrupt-pending flag */
    QF_tick();                   /* handle all armed time events in QF-nano */
    QK_ISR_EXIT();                  /* inform QK-nano that an ISR is exited */
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
      /* set all interrupts to high priority (no interrupt nesting), NOTE01 */
    IP   = ~0;
    EIP1 = ~0;

    IE_bit.ET2 = 1;                             /* Enable Timer2 interrupts */
    TMR2CN  = (1 << 2);                                     /* Start Timer2 */
}
/*..........................................................................*/
void QK_onIdle(void) {                  /* entered with interrupts unlocked */
#ifdef NDEBUG
    PCON |= 0x01;                     /* choose low-power mode for your app */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();             /* make sure that interrupts are disabled */
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
* QK-nano for 8051 requires that interrupts do not nest (i.e., an interrupt
* cannot preempt another interrupt). Setting all interrupts to the high
* priority level guarantees that interrupts will NOT preempt interrupts.
*
* 8051 enters an interrupt with interrupts enabled (IE bit set), and there
* is NO WAY of detecting reliably in software that an interrupt preempted
* another interrupt. Without this information, a real-time kernel, such as
* QK-nano, cannot distinguis the last interrupt in the preemption chain to
* perform the asynchronus task switch. Scheduling a task upon exit from an
* interrupt that nests on another interrupt would lead to priority inversion
* (a task preempting an interrupt).
*/


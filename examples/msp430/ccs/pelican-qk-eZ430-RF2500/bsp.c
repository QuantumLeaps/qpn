/*****************************************************************************
* Product: PELICAN crossing example, QK-nano kernel
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 09, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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
#include "bsp.h"
#include "pelican.h"

#define LED0_on()     (P1OUT |= (uint8_t)BIT0)
#define LED0_off()    (P1OUT &= (uint8_t)~BIT0)

#define LED1_on()     (P1OUT |= (uint8_t)BIT1)
#define LED1_off()    (P1OUT &= (uint8_t)~BIT1)

/*..........................................................................*/
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
#ifndef NDEBUG
    __low_power_mode_off_on_exit();
#endif
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */

    QF_tickISR();

    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
#pragma vector = PORT1_VECTOR
__interrupt void port1_ISR(void) {                           /* for testing */
#ifdef NDEBUG
    __low_power_mode_off_on_exit();
#endif

    P1IFG &= ~BIT2;                               /* clear interrupt source */

    QK_ISR_ENTRY();                     /* inform QK kernel about ISR entry */

    QActive_postISR((QActive *)&AO_Ped, TEST_SIG, 0);    /* post test event */

    QK_ISR_EXIT();                       /* inform QK kernel about ISR exit */
}

/*..........................................................................*/
void BSP_init(void) {
    WDTCTL   = (WDTPW | WDTHOLD);                               /* Stop WDT */

    /* configure the Basic Clock Module */
    DCOCTL   = CALDCO_8MHZ;                              /* Set DCO to 8MHz */
    BCSCTL1  = CALBC1_8MHZ;

    TACTL    = (ID_3 | TASSEL_2 | MC_1);       /* SMCLK, /8 divider, upmode */
    TACCR0   = (((BSP_SMCLK / 8) + BSP_TICKS_PER_SEC/2) / BSP_TICKS_PER_SEC);

    P1DIR   |= (BIT0 | BIT1);               /* P1.0 and P1.1 outputs (LEDs) */

    P1DIR  &= ~BIT2;                             /* P1.2 input (Switch TS1) */
    P1REN  |=  BIT2;                     /* enable pull-up resistor on P1.2 */
    P1SEL  &= ~BIT2;                         /* enable I/O function on P1.2 */
    P1IES  |=  BIT2;                     /* interrupt edge select high->low */
    P1IFG  &= ~BIT2;                              /* clear interrupt source */

}
/*..........................................................................*/
void QF_onStartup(void) {
    CCTL0 = CCIE;                                 /* CCR0 interrupt enabled */

    P1IE   |= BIT2;                   /* P1.2 interrupt enable (Switch TS1) */
}
/*..........................................................................*/
void QK_onIdle(void) {
//    LED1_on();                                    /* switch LED1 on and off */
//    LED1_off();

#ifndef NDEBUG
    __low_power_mode_1();                                     /* Enter LPM1 */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();             /* make sure that interrupts are disabled */
    for (;;) {
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    if (sig == PEDS_DONT_WALK) {
        LED0_on();
    }
    else {
        LED0_off();
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    if (sig == CARS_GREEN) {
        LED1_on();
    }
    else {
        LED1_off();
    }
}

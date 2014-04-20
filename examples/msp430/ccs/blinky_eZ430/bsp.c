/*****************************************************************************
* Product: BSP for the Simple Blinky example, eZ430 stick, Vanilla kernel
* Last updated for version 5.3.0
* Last updated on  2014-04-18
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
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
#include "qpn_port.h"
#include "bsp.h"

#include <msp430f2013.h> /* MSP430 variant used on eZ430 stick */

/*--------------------------------------------------------------------------*/
#define BSP_SMCLK   1100000UL

/*..........................................................................*/
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
#ifdef NDEBUG
    __low_power_mode_off_on_exit(); /* disable low-power mode on exit, NOTE1*/
#endif
    QF_tickXISR(0U);  /* process all time events at clock tick rate 0 */
}
/*..........................................................................*/
void BSP_init(void) {
    WDTCTL = (WDTPW | WDTHOLD); /* Stop WDT */
    P1DIR |= (1U << 0); /* configure P1.0 as output (for the LED) */
    CCR0 = (BSP_SMCLK + BSP_TICKS_PER_SEC/2) / BSP_TICKS_PER_SEC;
    TACTL = (TASSEL_2 | MC_1 | TACLR); /* SMCLK, upmode, clear timer */
}
/*..........................................................................*/
void BSP_ledOff(void) {
    P1OUT |= (uint8_t)(1U << 0); /* turn the LED at P1.0 on */
}
/*..........................................................................*/
void BSP_ledOn(void) {
    P1OUT &= (uint8_t)~(1U << 0); /* turn the LED at P1.0 off */
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    CCTL0 = CCIE; /* CCR0 interrupt enabled */
}
/*..........................................................................*/
void QF_onIdle(void) {
#ifdef NDEBUG
    /* adjust the low-power mode to your application */
    __low_power_mode_1(); /* Enter LPM1 */
#else
    QF_INT_ENABLE();
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    (void)file;       /* avoid compiler warning */
    (void)line;       /* avoid compiler warning */
    QF_INT_DISABLE();  /* make sure that interrupts are disabled */
    for (;;) {
    }
}

/*****************************************************************************
* NOTE1:
* The MSP430 interrupt processing restores the CPU status register upon
* exit from the ISR, so if any low-power mode has been set before the
* interrupt, it will be restored upon the ISR exit. This is not what
* you want in QP, because it will stop the CPU. Therefore, to prevent
* this from happening, the macro __low_power_mode_off_on_exit() clears
* any low-power mode in the *stacked* CPU status register, so that the
* low-power mode won't be restored upon the ISR exit.
*/

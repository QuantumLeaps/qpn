/*****************************************************************************
* Product: PELICAN crossing example for MSP-EXP430G2 LaunchPad, QK-nano
* Last updated for version 5.3.0
* Last updated on  2014-04-14
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
#include "qpn_port.h"  /* QP-nano port */
#include "bsp.h"       /* Board Support Package (BSP) */
#include "pelican.h"   /* application interface */

#include <msp430g2553.h> /* MSP430 variant used on MSP-EXP430G2 LaunchPad */

/*--------------------------------------------------------------------------*/
#define BSP_SMCLK   1100000UL

/* pin assignments to LEDs */
#define LED1        (1U << 0)
#define LED2        (1U << 6)
#define LED1_on()   (P1OUT |= (uint8_t)LED1)
#define LED1_off()  (P1OUT &= (uint8_t)~LED1)
#define LED2_on()   (P1OUT |= (uint8_t)LED2)
#define LED2_off()  (P1OUT &= (uint8_t)~LED2)

#define S2          (1U << 3)
#define S2_read()   (P1IN & S2)

/*..........................................................................*/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerA_ISR(void) {
    static struct ButtonsDebouncing {
        uint8_t depressed;
        uint8_t previous;
    } buttons;
    uint8_t current;
    uint8_t tmp;

    /* don't need to exit the low-power mode, see NOTE01 */
    QK_ISR_ENTRY();  /* inform QK-nano about ISR entry */

    QF_tickXISR(0U); /* process time events at rate 0 */

    /* read the switches and pack them into a signle byte 'current',
    * also, adjust the polarity of the bits in "current" so that 'depressed'
    * corresponds to bit 1 and 'released' corresponds to bit 0.
    */
    current = ~S2_read();

    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if (tmp != 0U) { /* any new buttons debounced? */
        if ((buttons.depressed & S2) != 0U) { /* S2 depressed? */
            LED1_on();
            QACTIVE_POST_ISR((QActive *)&AO_Pelican,
                             PEDS_WAITING_SIG, 0U);
        }
        else { /* S2 released */
            LED1_off();
        }
    }

    QK_ISR_EXIT();   /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
#pragma vector = NMI_VECTOR
__interrupt void nmi_ISR(void) {
    WDTCTL = (WDTPW | WDTHOLD); /* Stop WDT */
}
/*..........................................................................*/
void BSP_init(void) {
    WDTCTL = (WDTPW | WDTHOLD); /* Stop WDT */

    P1DIR |= LED1 | LED2; /* configure LED1 and LED2 as outputs */

    P1DIR &= (uint8_t)(~S2); /* configure S2 as input */
    P1REN |= S2; /* enable pullup resistor for S2 */
    P1OUT |= S2; /* drive the output high */

    CCR0 = (BSP_SMCLK + BSP_TICKS_PER_SEC/2) / BSP_TICKS_PER_SEC;
    TACTL = (TASSEL_2 | MC_1 | TACLR); /* SMCLK, upmode, clear timer */
}
/*..........................................................................*/
void QF_onStartup(void) {
    CCTL0 = CCIE; /* CCR0 interrupt enabled */
}
/*..........................................................................*/
void QK_onIdle(void) {
    //QF_INT_DISABLE();
    //LED1_on();
    //LED1_off();
    //QF_INT_ENABLE();

#ifdef NDEBUG
    /* adjust the low-power mode to your application */
    __low_power_mode_1(); /* enter LPM1, see NOTE02 */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    (void)file;       /* avoid compiler warning */
    (void)line;       /* avoid compiler warning */
    QF_INT_DISABLE();  /* make sure that interrupts are locked */
    for (;;) {
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    if (sig == PEDS_DONT_WALK) {
        LED2_on();
    }
    else {
        LED2_off();
    }
}

/*****************************************************************************
* NOTE01:
* With QK, ISRs don't need to restore the CPU power bits in the SR, because
* QK drops down to the idle task only when finished processing all events.
*
* NOTE02:
* When low power mode is used, the QK_onIdle() is actually called just once
* and never loops again. This is because the MSP430 core keeps the
* power-control bits in the SR register of the CPU. The SR register gets
* automatically restored upon interrupt return and turns off the CPU,
* if any of the power-saving mode is selected. The CPU stops and doesn't
* actually execute any code of the QK idle loop. If you choose to perform
* some processing in the QK idle loop before going to sleep, you need to call
* __low_power_mode_off_on_exit() in the ISRs.
*/

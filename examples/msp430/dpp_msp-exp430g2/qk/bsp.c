/*****************************************************************************
* Product: DPP on MSP-EXP430G2 board, preemptive QK-nano kernel
* Last Updated for Version: 5.8.0
* Date of the Last Update:  2016-11-07
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
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
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"
#include "dpp.h"
#include "bsp.h"

#include <msp430g2553.h>  /* MSP430 variant used */
/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
/* 8MHz clock setting, see BSP_init() */
#define BSP_MCK     8000000U
#define BSP_SMCLK   8000000U

/* LEDs on the MSP-EXP430G2 board */
#define LED1        (1U << 0)
#define LED2        (1U << 6)

/* Buttons on the MSP-EXP430G2 board */
#define BTN1        (1U << 3)

/* random seed */
static uint32_t l_rnd;

/* ISRs used in this project ===============================================*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    __interrupt void TIMER0_A0_ISR(void); /* prototype */
    #pragma vector=TIMER0_A0_VECTOR
    __interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(TIMER0_A0_VECTOR)))
    TIMER0_A0_ISR(void)
#else
    #error MSP430 compiler not supported!
#endif
{
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint8_t depressed;
        uint8_t previous;
    } buttons = { (uint8_t)~0U, (uint8_t)~0U };
    uint8_t current;
    uint8_t tmp;

    QK_ISR_ENTRY();    /* inform QK about entering the ISR */

    TACTL &= ~TAIFG;   /* clear the interrupt pending flag */

    QF_tickXISR(0U);   /* process time events for rate 0 */

    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = ~P1IN; /* read P1 port with the state of BTN1 */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & BTN1) != 0U) {     /* debounced BTN1 state changed? */
        if ((buttons.depressed & BTN1) != 0U) { /* is BTN1 depressed? */
            QACTIVE_POST_ISR((QActive *)&AO_Table, PAUSE_SIG, 0U);
        }
        else {            /* the button is released */
            QACTIVE_POST_ISR((QActive *)&AO_Table, SERVE_SIG, 0U);
        }
    }

    QK_ISR_EXIT();     /* inform QK about exiting the ISR */
}

/* BSP functions ===========================================================*/
void BSP_init(void) {
    WDTCTL = WDTPW | WDTHOLD; /* stop watchdog timer */

    /* configure the Basic Clock Module */
    DCOCTL = 0;             /* Select lowest DCOx and MODx settings */
    BCSCTL1 = CALBC1_8MHZ;  /* Set DCO */
    DCOCTL = CALDCO_8MHZ;

    /* configure pins for LEDs */
    P1DIR |= (LED1 | LED2); /* set LED1 and LED2 pins to output */

    /* configure pin for Button */
    P1DIR &= ~BTN1;   /* set BTN1 pin as input */
    P1OUT |= BTN1;    /* drive output to hi */
    P1REN |= BTN1;    /* enable internal pull up register */
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
    if (stat[0] == 'h') { /* is Philo hungry? */
        P1OUT |=  LED1;  /* turn LED1 on */
    }
    else {
        P1OUT &= ~LED1;  /* turn LED1 off */
    }
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    /* not enouhg LEDs to implement this feature */
    if (paused != 0U) {
        //P1OUT |=  LED1;
    }
    else {
        //P1OUT &= ~LED1;
    }
}
/*..........................................................................*/
uint32_t BSP_random(void) { /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * ((uint32_t)3U*7U*11U*13U*23U);

    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}


/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    TACTL  = (ID_3 | TASSEL_2 | MC_1);  /* SMCLK, /8 divider, upmode */
    TACCR0 = (((BSP_SMCLK / 8U) + BSP_TICKS_PER_SEC/2U) / BSP_TICKS_PER_SEC);
    CCTL0 = CCIE;  /* CCR0 interrupt enabled */
}
/*..........................................................................*/
void QK_onIdle(void) {
    /* toggle LED2 on and then off, see NOTE1 */
    QF_INT_DISABLE();
    P1OUT |=  LED2;  /* turn LED2 on */
    P1OUT &= ~LED2;  /* turn LED2 off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular MSP430 MCU.
    */
    __low_power_mode_1(); /* Enter LPM1; also ENABLES interrupts */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    /* implement the error-handling policy for your application!!! */
    QF_INT_DISABLE(); /* disable all interrupts */

    /* cause the reset of the CPU... */
    WDTCTL = WDTPW | WDTHOLD;
    __asm("    push &0xFFFE");
    /* return from function does the reset */
}

/*****************************************************************************
* NOTE1:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/


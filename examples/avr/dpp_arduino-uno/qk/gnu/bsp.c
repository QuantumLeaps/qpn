/*****************************************************************************
* Product: DPP example, Arduino-UNO board, preemptive QK kernel, GNU-AVR
* Last updated for version 6.6.0
* Last updated on  2019-10-15
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
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
* along with this program. If not, see <www.gnu.org/licenses>.
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"
#include "dpp.h"
#include "bsp.h"

/* the AVR device specific header <avr/io.h> is already included */
/* add other drivers if necessary... */

Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
/* Arduino-UNO runs off the 16MHz oscillator */
#define F_CPU   16000000UL

/* the on-board LED labeled "L" on Arduino-UNO (PORTB) */
#define LED_L   (1U << 5)

/* external button BTN_EXT (PORTD), http://arduino.cc/en/tutorial/button */
#define BTN_EXT (1U << 2)

static unsigned  l_rnd;  /* random seed */


/* ISRs used in this project ===============================================*/
ISR(TIMER2_COMPA_vect) {
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint8_t depressed;
        uint8_t previous;
    } buttons = { 0xFFU, 0xFFU };
    uint8_t current;
    uint8_t tmp;

    QK_ISR_ENTRY();  /* inform QK about entering an ISR */

    QF_tickXISR(0U); /* process time events for rate 0 */

    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = PIND; /* read PORTD with the state of BTN_EXT */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & BTN_EXT) != 0U) {  /* debounced BTN_EXT state changed? */
        if ((buttons.depressed & BTN_EXT) != 0U) { /* is BTN_EXT depressed? */
            QACTIVE_POST_ISR(&AO_Table, PAUSE_SIG, 0U);
        }
        else {            /* the button is released */
            QACTIVE_POST_ISR(&AO_Table, SERVE_SIG, 0U);
        }
    }

    QK_ISR_EXIT();  /* inform QK about exiting an ISR */
}

/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* setup the port for the LED (PORTB.5) */
    DDRB   =  0xFFU;    /* set all pins on PORTB as output */
    PORTB &= ~LED_L;    /* drive LED_L pin low */

    /* setup the port for the Button (PORTD.2) */
    DDRD  &= ~BTN_EXT;  /* set PORTD pin BTN_EXT as input */
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
    if (stat[0] == 'h') {
        PORTB |= LED_L;
    }
    else {
        PORTB &= ~LED_L;
    }
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    /* not enouhg LEDs to implement this feature */
    if (paused != 0U) {
        //PORTB |= LED_L;
    }
    else {
        //PORTB &= ~LED_L;
    }
}
/*..........................................................................*/
uint32_t BSP_random(void) { /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);

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
    /* set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking... */
    TCCR2A = (1U << WGM21) | (0U << WGM20);
    TCCR2B = (1U << CS22 ) | (1U << CS21) | (1U << CS20); /* 1/2^10 */
    ASSR  &= ~(1U << AS2);
    TIMSK2 = (1U << OCIE2A); /* Enable TIMER2 compare Interrupt */
    TCNT2  = 0U;
    OCR2A  = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;
}
/*..........................................................................*/
void QF_onCleanup(void) {
}

/*..........................................................................*/
void QK_onIdle(void) {
    /* toggle the User LED, see NOTE1 , not enough LEDs to implement! */
    //QF_INT_DISABLE();
    //PORTB |= LED_L;
    //PORTB &= ~LED_L;
    //QF_INT_ENABLE();

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular AVR MCU.
    */
    SMCR = (0 << SM0) | (1 << SE); /* idle mode, adjust to your project */
    QK_CPU_SLEEP(); /* put CPU to sleep */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    /* implement the error-handling policy for your application!!! */
    QF_INT_DISABLE(); /* disable all interrupts */
    QF_RESET();  /* reset the CPU */
}

/*****************************************************************************
* NOTE1:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

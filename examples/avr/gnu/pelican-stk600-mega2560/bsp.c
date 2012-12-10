/*****************************************************************************
* PELICAN crossing example for STK600-ATMEGA2560
* Date of the Last Update:  Sep 17, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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
#include <avr\io.h>                                              /* AVR I/O */

#define LED_ON(num_)       (PORTD &= ~(1 << (num_)))
#define LED_OFF(num_)      (PORTD |= (1 << (num_)))
#define LED_ON_ALL()       (PORTD = 0x00)
#define LED_OFF_ALL()      (PORTD = 0xFF)

/*..........................................................................*/
ISR(TIMER2_COMPA_vect) {
    /* No need to clear the interrupt source since the Timer0 compare
    * interrupt is automatically cleared in hardware when the ISR runs.
    */
    QF_tick();
}
/*..........................................................................*/
void BSP_init(void) {
    DDRD  = 0xFF;                    /* All PORTD pins are outputs for LEDs */
    LED_OFF_ALL();                                     /* trun off all LEDs */
}
/*..........................................................................*/
void QF_onStartup(void) {
       /* set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking */
    TCCR2A = ((1 << WGM21) | (0 << WGM20));
    TCCR2B = ((1 << CS22) | (1 << CS21) | (1 << CS20));           /* 1/2^10 */
    ASSR  &= ~(1 << AS2);
    TIMSK2 = (1 << OCIE2A);              /* enable TIMER2 compare Interrupt */
    TCNT2 = 0;
    OCR2A = ((F_CPU / BSP_TICKS_PER_SEC / 1024) - 1);          /* keep last */
}
/*..........................................................................*/
void QF_onIdle(void) {        /* entered with interrupts LOCKED, see NOTE01 */

                     /* toggle the LED number 7 on and then off, see NOTE02 */
    LED_ON(7);
    LED_OFF(7);

#ifdef NDEBUG

    SMCR = (0 << SM0) | (1 << SE);/*idle sleep mode, adjust to your project */

      /* never separate the following two assembly instructions, see NOTE03 */
    __asm__ __volatile__ ("sei" "\n\t" :: );
    __asm__ __volatile__ ("sleep" "\n\t" :: );

    SMCR = 0;                                           /* clear the SE bit */
#else
    QF_INT_ENABLE();
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();
    LED_ON_ALL();                                            /* all LEDs on */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED:       LED_ON(0);  LED_OFF(1); LED_OFF(2); break;
        case CARS_YELLOW:    LED_OFF(0); LED_ON(1);  LED_OFF(2); break;
        case CARS_GREEN:     LED_OFF(0); LED_OFF(1); LED_ON(2);  break;
        case CARS_BLANK:     LED_OFF(0); LED_OFF(1); LED_OFF(2); break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: LED_ON(4);  LED_OFF(5); break;
        case PEDS_BLANK:     LED_OFF(4); LED_OFF(5); break;
        case PEDS_WALK:      LED_OFF(4); LED_ON(5);  break;
    }
}

/*****************************************************************************
* NOTE01:
* The QF_onIdle() callback is called with interrupts locked, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally unlock interrupts, ideally atomically
* with putting the CPU to the power-saving mode.
*
* NOTE02:
* The LED[7] is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*
* NOTE03:
* As described in the "AVR Datasheet" in Section "Reset and Interrupt
* Handling", when using the SEI instruction to enable interrupts, the
* instruction following SEI will be executed before any pending interrupts.
* As the Datasheet shows in the assembly example, the pair of instructions
*     SEI       ; enable interrupts
*     SLEEP     ; go to the sleep mode
* executes ATOMICALLY, and so no interrupt can be serviced between these
* instructins. You should NEVER separate these two lines.
*/

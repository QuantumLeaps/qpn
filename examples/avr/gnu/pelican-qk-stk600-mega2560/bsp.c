/*****************************************************************************
* PELICAN crossing example for STK600-ATMEGA2560
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

#define LED_ON(num_)       (PORTD &= ~(1 << (num_)))
#define LED_OFF(num_)      (PORTD |= (1 << (num_)))
#define LED_ON_ALL()       (PORTD = 0x00)
#define LED_OFF_ALL()      (PORTD = 0xFF)

/*..........................................................................*/
ISR(TIMER2_COMPA_vect) {
    /* No need to clear the interrupt source since the Timer0 compare
    * interrupt is automatically cleared in hardware when the ISR runs.
    */
    QK_ISR_ENTRY();                     /* inform QK kernel about ISR entry */

    QF_tick();

    QK_ISR_EXIT();                       /* inform QK kernel about ISR exit */
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
void QK_onIdle(void) {
                     /* toggle the LED number 7 on and then off, see NOTE01 */
    QF_INT_DISABLE();
    LED_ON(7);
    LED_OFF(7);
    QF_INT_ENABLE();

#ifdef NDEBUG

    SMCR = (0 << SM0) | (1 << SE);/*idle sleep mode, adjust to your project */
    __asm__ __volatile__ ("sleep" "\n\t" :: );
    SMCR = 0;                                           /* clear the SE bit */

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
* The LED[7] is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*/

/*****************************************************************************
* BSP for Microstick II board, PIC24FJ64GB002 device, Vanilla kernel
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 22, 2012
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
#include "pelican.h"

#include "p24FJ64GB002.h"                /* header for PIC24F device in use */

Q_DEFINE_THIS_FILE

_CONFIG2(FNOSC_FRC);              /* set flash configuration for the device */

/* Local-scope objects -----------------------------------------------------*/
                                 /* frequency of the FRC oscillator ~ 8 MHz */
#define FOSC_HZ                 8000000.0
                                       /* instruction cycle clock frequency */
#define FCY_HZ                  (FOSC_HZ / 2.0)

                                    /* controlling the LED of Microstick II */
#define LED_ON()                (LATA |= (1U << 0))
#define LED_OFF()               (LATA &= ~(1U << 0))
#define LED_TOGGLE()            (LATA ^= (1U << 0))


/* ISRs --------------------------------------------------------------------*/
#define TIMER2_ISR_PRIO         4

void  __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void) {
    _T2IF = 0;                              /* clear Timer 2 interrupt flag */
    QF_tickISR();                /* handle all armed time events in QF-nano */
}

/*--------------------------------------------------------------------------*/
void BSP_init(void) {
    RCONbits.SWDTEN = 0;                                /* disable Watchdog */

    TRISA = 0x00;                                /* set LED pins as outputs */
    PORTA = 0x00;                               /* set LEDs drive state low */
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */
    T2CON = 0x0000U;  /* Use Internal Osc (Fcy), 16 bit mode, prescaler = 1 */
    TMR2  = 0x0000U; /* Start counting from 0 and clear the prescaler count */
    PR2   = (uint16_t)((FCY_HZ / BSP_TICKS_PER_SEC) - 1.0 + 0.5); /* period */
    _T2IP = TIMER2_ISR_PRIO;              /* set Timer 2 interrupt priority */
    _T2IF = 0;                           /* clear the interrupt for Timer 2 */
    _T2IE = 1;                              /* enable interrupt for Timer 2 */
    T2CONbits.TON = 1;                                     /* start Timer 2 */
}
/*..........................................................................*/
void QF_onIdle(void) {        /* entered with interrupts locked, see NOTE01 */

    /* NOTE: not enough LEDs on the Microstick II board to implement
    *  the idle loop activity indicator ...
    */
    //LED_ON ();
    //LED_OFF();

#ifdef NDEBUG
    __asm__ volatile("disi #0x0001");
    Idle();                          /* transition to Idle mode, see NOTE02 */
#else
    QF_INT_ENABLE();                       /* enable interrupts, see NOTE01 */
#endif
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
    /* NOTE: not enough LEDs on the Microstick II board to implement
     *  the lights for cars ...
     */
    //LED_OFF(CARS_RED_LED | CARS_YLW_LED | CARS_GRN_LED);
    switch (sig) {
        case CARS_BLANK:
            break;
        case CARS_RED:
            //LED_ON(CARS_RED_LED);
            break;
        case CARS_YELLOW:
            //LED_ON(CARS_YLW_LED);
            break;
        case CARS_GREEN:
            //LED_ON(CARS_GRN_LED);
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    LED_OFF();
    switch (sig) {
        case PEDS_BLANK:
            break;
        case PEDS_WALK:
            LED_ON();
            break;
        case PEDS_DONT_WALK:
            LED_ON();
            break;
    }
}

/*****************************************************************************
* NOTE01:
* The callback function QF_onIdle() is called with interrupts disabled,
* because the idle condition can be invalidated by any enabled interrupt
* that would post events. The QF_onIdle() function *must* enable interrupts
* internally
*
* NOTE02:
* To be on the safe side, the DISICNT counter is set to just 1 cycle just
* before entering the Idle mode (or Sleep mode, if you choose). This way,
* interrupts (with priorities 1-6) get enabled at the same time as the
* transition to the low-power mode.
*/


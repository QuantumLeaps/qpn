/*****************************************************************************
* BSP for Explorer 16 board with dsPIC33FJ256GP710, Vanilla kernel
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 26, 2012
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

Q_DEFINE_THIS_FILE

_FOSCSEL(FNOSC_FRC);              /* set flash configuration for the device */
_FOSC(FCKSM_CSDCMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);
                             /* frequency of the FRC oscillator for dsPICFJ */
#define FOSC_HZ                 7370000.0
                                       /* instruction cycle clock frequency */
#define FCY_HZ                  (FOSC_HZ / 2.0)

#define LED_ON(leds_)           (LATA |= (leds_))
#define LED_OFF(leds_)          (LATA &= ~(leds_))

#define IDLE_LED                (1 << 0)
#define CARS_RED_LED            (1 << 2)
#define CARS_YLW_LED            (1 << 3)
#define CARS_GRN_LED            (1 << 4)
#define PEDS_WALK_LED           (1 << 6)
#define PEDS_DONT_LED           (1 << 7)

/* ISRs --------------------------------------------------------------------*/
#define TIMER2_ISR_PRIO         4

void  __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void) {
    _T2IF = 0;                              /* clear Timer 2 interrupt flag */
    QF_tickISR();                /* handle all armed time events in QF-nano */
}

/*--------------------------------------------------------------------------*/
void BSP_init(void) {
    RCONbits.SWDTEN = 0;                                /* disable Watchdog */

    LATA  = 0xFF00;            /* set LEDs (D3-D10/RA0-RA7) drive state low */
    TRISA = 0xFF00;             /* set LED pins (D3-D10/RA0-RA7) as outputs */
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

    LED_ON (IDLE_LED);                    /* blink the IDLE LED, see NOTE02 */
    LED_OFF(IDLE_LED);

#ifdef NDEBUG
    __asm__ volatile("disi #0x0001");
    Idle();                          /* transition to Idle mode, see NOTE03 */
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
    LED_OFF(CARS_RED_LED | CARS_YLW_LED | CARS_GRN_LED);
    switch (sig) {
        case CARS_OFF:
            break;
        case CARS_RED:
            LED_ON(CARS_RED_LED);
            break;
        case CARS_YELLOW:
            LED_ON(CARS_YLW_LED);
            break;
        case CARS_GREEN:
            LED_ON(CARS_GRN_LED);
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    LED_OFF(PEDS_WALK_LED | PEDS_DONT_LED);
    switch (sig) {
        case PEDS_BLANK:
            break;
        case PEDS_WALK:
            LED_ON(PEDS_WALK_LED);
            break;
        case PEDS_DONT_WALK:
            LED_ON(PEDS_DONT_LED);
            break;
    }
}

/*****************************************************************************
* NOTE01:
* The callback function QF_onIdle() is called with interrupts locked, because
* the idle condition can be invalidated by any enabled interrupt that would
* post events. The QF_onIdle() function *must* enable interrupts internally
*
* NOTE02:
* The LED 0 (D3 on the Explorer 16 board) is used to visualize the idle loop
* activity. The brightness of the LED is proportional to the frequency of
* invcations of the idle loop. Please note that the LED is toggled with
* interrupts disabled, so no interrupt execution time contributes to the
* brightness of the User LED.
*
* NOTE03:
* To be on the safe side, the DISICNT counter is set to just 1 cycle just
* before entering the Idle mode (or Sleep mode, if you choose). This way,
* interrupts (with priorities 1-6) get enabled at the same time as the
* transition to the low-power mode.
*/


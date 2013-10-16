/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 14, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
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

#define LED0_on()     (P1OUT |= (uint8_t)1)
#define LED0_off()    (P1OUT &= (uint8_t)~1)
#define LED0_toggle() (P1OUT ^= (uint8_t)1)
#define LED1_on()     (P1OUT |= (uint8_t)2)
#define LED1_off()    (P1OUT &= (uint8_t)~2)
#define LED1_toggle() (P1OUT ^= (uint8_t)2)


/* prototypes of ISRs defined in this module ...............................*/
__interrupt void timerA_ISR(void);

/*..........................................................................*/
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
#ifndef NDEBUG
    __low_power_mode_off_on_exit();
#endif
    QF_tickISR();
}
/*..........................................................................*/
void BSP_init(void) {
    WDTCTL = (WDTPW | WDTHOLD);                                 /* Stop WDT */
    P1DIR |= (0x01 | 0x02);                        /* P1.0 and P1.1 outputs */
    CCR0   = ((BSP_SMCLK + BSP_TICKS_PER_SEC/2) / BSP_TICKS_PER_SEC);
    TACTL  = (TASSEL_2 | MC_1);                            /* SMCLK, upmode */
}
/*..........................................................................*/
void QF_onStartup(void) {
    CCTL0 = CCIE;                                 /* CCR0 interrupt enabled */
}
/*..........................................................................*/
void QF_onIdle(void) {
#ifndef NDEBUG
    __low_power_mode_1();                                     /* Enter LPM1 */
#endif
    QF_INT_ENABLE();
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
void BSP_displayPhilStat(uint8_t n, char const *stat) {
    if (n == 0) {                                           /* for Philo[0] */
        if (stat[0] == 'e') {                       /* ON when Philo eating */
            LED0_on();
        }
        else {
            LED0_off();
        }
    }
    else if (n == 1) {                                      /* for Philo[1] */
        if (stat[0] == 'e') {                       /* ON when Philo eating */
            LED1_on();
        }
        else {
            LED1_off();
        }
    }
}

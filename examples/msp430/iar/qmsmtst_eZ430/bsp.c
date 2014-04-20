/*****************************************************************************
* Product: QHsmTst example
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

#include <yfuns.h>       /* for terminal I/O __read() and __write() */
#include <string.h>      /* for strlen() */

/*..........................................................................*/
void BSP_init(void) {
    WDTCTL = (WDTPW | WDTHOLD); /* Stop WDT */
    P1DIR |= (1U << 0); /* configure P1.0 as output (for the LED) */
}
/*..........................................................................*/
void BSP_exit(void) {
}
/*..........................................................................*/
void BSP_LED_on(void) {
    P1OUT |= (1U << 0);
}
/*..........................................................................*/
void BSP_LED_off(void) {
    P1OUT &= ~(1U << 0);
}
/*..........................................................................*/
void BSP_display(char const *msg) {
    __write(1, (unsigned char const *)msg, strlen(msg));
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    (void)file;  /* avoid compiler warning */
    (void)line;  /* avoid compiler warning */
    for (;;) {
    }
}

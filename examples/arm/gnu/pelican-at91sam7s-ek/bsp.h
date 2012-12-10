/*****************************************************************************
* Product: BSP for PELICAN crossing example, AT91SAM7S-EK board
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 11, 2012
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
#ifndef bsp_h
#define bsp_h

#include <AT91SAM7S64.H>                        /* AT91SAMT7S64 definitions */

#define BSP_TICKS_PER_SEC   100U

enum BSP_CarsSignal {
    CARS_RED, CARS_YELLOW, CARS_GREEN, CARS_BLANK
};

enum BSP_PedsSignal {
    PEDS_DONT_WALK, PEDS_WALK, PEDS_BLANK
};

void BSP_init(void);
void BSP_showState(char_t const *state);
void BSP_signalCars(enum BSP_CarsSignal sig);
void BSP_signalPeds(enum BSP_PedsSignal sig);
void BSP_terminate(int16_t result);

                                         /* External Oscillator MAINCK [Hz] */
#define MAINCK               18432000

                                     /* Maseter Clock (PLLRC div by 2) [Hz] */
#define MCK                  47923200

extern uint32_t const BSP_led[4];         /* LEDs of the AT91SAM7S-EK board */
#define LED_ON(num_)       (AT91C_BASE_PIOA->PIO_CODR = BSP_led[num_])
#define LED_OFF(num_)      (AT91C_BASE_PIOA->PIO_SODR = BSP_led[num_])

extern uint32_t const BSP_pb[4];  /* push buttons of the AT91SAM7S-EK board */
#define PB_STAT(num_)      ((AT91C_BASE_PIOA->PIO_PDSR & BSP_pb[num_]) == 0U)

#endif                                                             /* bsp_h */

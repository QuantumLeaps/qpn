/*****************************************************************************
* Product: PELICAN crossing example
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 29, 2013
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
#ifndef bsp_h
#define bsp_h

#include <msp430x20x3.h>                             /* MSP430 variant used */

/*--------------------------------------------------------------------------*/
#define BSP_MCK              16000000
#define BSP_SMCLK            1100000
#define BSP_TICKS_PER_SEC    20

/* street signals ..........................................................*/
enum BSP_CarsSignal {
    CARS_RED, CARS_YELLOW, CARS_GREEN, CARS_OFF
};

enum BSP_PedsSignal {
    PEDS_DONT_WALK, PEDS_BLANK, PEDS_WALK
};

void BSP_init(void);
#define BSP_signalCars(sig_)         ((void)0)
void BSP_signalPeds(enum BSP_PedsSignal sig);
#define BSP_showState(prio_, state_) ((void)0)
#define BSP_terminate(stat_)         ((void)0)

#endif                                                             /* bsp_h */


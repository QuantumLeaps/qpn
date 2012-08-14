/*****************************************************************************
* Product: PELICAN crossing example, MPLAB-C30 compiler
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 24, 2012
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
#include "qpn_port.h"                                       /* QP-nano port */
#include "bsp.h"                             /* Board Support Package (BSP) */
#include "pelican.h"                               /* application interface */

/*..........................................................................*/
static QEvt  l_pelicanQueue[2];
static QEvt  l_pedQueue[1];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
    { (QActive *)0, (QEvt *)0,      0U                    },
    { &AO_Pelican,  l_pelicanQueue, Q_DIM(l_pelicanQueue) },
    { &AO_Ped,      l_pedQueue,     Q_DIM(l_pedQueue)     }
};

/* make sure that the QF_active[] array matches QF_MAX_ACTIVE in qpn_port.h */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

/*..........................................................................*/
int main(void) {
    Pelican_ctor();                          /* instantiate the  Pelican AO */
    Ped_ctor();                              /* instantiate the  Ped     AO */

    BSP_init();                                     /* initialize the board */

    return QF_run();                         /* transfer control to QF-nano */
}

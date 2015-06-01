/*****************************************************************************
* Product: Comp ("Orthogonal Component) example
* Last updated for version 5.4.0
* Last updated on  2015-05-18
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
#include "qpn.h"     /* QP-nano API */
#include "bsp.h"     /* Board Support Package */
#include "comp.h"    /* Application interface */

/* Local-scope objects -----------------------------------------------------*/
static QEvt l_clockQueue[3];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,              (QEvt *)0,    0U                  },
    { (QActive *)&AO_AlarmClock, l_clockQueue, Q_DIM(l_clockQueue) }
};
uint8_t const Q_ROM QF_activeNum = Q_DIM(QF_active) - 1;

/*..........................................................................*/
int main (void) {
    AlarmClock_ctor();

    BSP_init(); /* initialize the board */

    return QF_run(); /* transfer control to QF-nano */
}

/*****************************************************************************
* Product: "Fly 'n' Shoot" game example
* Last Updated for Version: 5.8.0
* Date of the Last Update:  2016-11-06
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"   /* QP-nano */
#include "game.h"  /* Game application */
#include "bsp.h"   /* Board Support Package */

/*..........................................................................*/
static QEvt l_tunnelQueue[GAME_MINES_MAX + 5];
static QEvt l_shipQueue[3];
static QEvt l_missileQueue[3];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,           (QEvt *)0,      0U                    },
    { (QActive *)&AO_Tunnel,  l_tunnelQueue,  Q_DIM(l_tunnelQueue)  },
    { (QActive *)&AO_Ship,    l_shipQueue,    Q_DIM(l_shipQueue)    },
    { (QActive *)&AO_Missile, l_missileQueue, Q_DIM(l_missileQueue) }
};

/*..........................................................................*/
int_t main() {
    Tunnel_ctor();
    Ship_ctor();
    Missile_ctor(GAME_MISSILE_SPEED_X);

    QF_init(Q_DIM(QF_active)); /* initialize the QF-nano framework */
    BSP_init();      /* initialize the Board Support Package */

    return QF_run(); /* transfer control to QF-nano */
}

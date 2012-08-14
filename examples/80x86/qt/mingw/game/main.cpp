/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 18, 2012
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
#include "gui.h"
#include "qpn_app.h"                   // Qt Application for emulating QP-nano
//-----------------
extern "C" {
    #include "qpn_port.h"
    #include "bsp.h"
    #include "game.h"
}

/*..........................................................................*/
static QEvt l_tunnelQueue[GAME_MINES_MAX + 5];
static QEvt l_shipQueue[3];
static QEvt l_missileQueue[3];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
    { (QActive *)0,           (QEvt *)0,      0U                    },
    { (QActive *)&AO_Tunnel,  l_tunnelQueue,  Q_DIM(l_tunnelQueue)  },
    { (QActive *)&AO_Ship,    l_shipQueue,    Q_DIM(l_shipQueue)    },
    { (QActive *)&AO_Missile, l_missileQueue, Q_DIM(l_missileQueue) }
};

/* make sure that the QF_active[] array matches QF_MAX_ACTIVE in qpn_port.h */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

/*..........................................................................*/
int main(int argc, char *argv[]) {
                          /* explicitly invoke the active objects' ctors... */
    Missile_ctor();
    Ship_ctor();
    Tunnel_ctor();

    QPnApp app(argc, argv);

    Gui *gui = new Gui;
    gui->show();

    BSP_init();           /* initialize the Board Support Package */
    QF_init();     /* initialize the framework and the underlying RT kernel */

                                           /* initialize the event pools... */
    BSP_init();                                     /* initialize the board */

    return QF_run();                         /* transfer control to QF-nano */
}




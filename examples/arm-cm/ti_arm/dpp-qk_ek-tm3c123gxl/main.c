/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Jun 29, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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
#include "qpn_port.h"                                       /* QP-nano port */
#include "dpp.h"                                   /* application interface */
#include "bsp.h"                             /* Board Support Package (BSP) */

/*..........................................................................*/
static QEvt l_philoQueue[N_PHILO][N_PHILO];
static QEvt l_tableQueue[2];

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                      },
    { (QActive *)&AO_Philo0,  l_philoQueue[0],  Q_DIM(l_philoQueue[0])  },
    { (QActive *)&AO_Philo1,  l_philoQueue[1],  Q_DIM(l_philoQueue[1])  },
    { (QActive *)&AO_Philo2,  l_philoQueue[2],  Q_DIM(l_philoQueue[2])  },
    { (QActive *)&AO_Philo3,  l_philoQueue[3],  Q_DIM(l_philoQueue[3])  },
    { (QActive *)&AO_Philo4,  l_philoQueue[4],  Q_DIM(l_philoQueue[4])  },
    { (QActive *)&AO_Table,   l_tableQueue,     Q_DIM(l_tableQueue)     }
};

/* make sure that the QF_active[] array matches QF_MAX_ACTIVE in qpn_port.h */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

/*..........................................................................*/
int main(void) {
    Philo_ctor();                              /* instantiate all Philo AOs */
    Table_ctor();                               /* instantiate the Table AO */

    BSP_init();

    return QF_run();                         /* transfer control to QF-nano */
}

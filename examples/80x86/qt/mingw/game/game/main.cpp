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
#include "qp_app.h"
//-----------------
extern "C" {
#include "qp_port.h"
#include "bsp.h"
#include "game.h"
}

/* Local-scope objects -----------------------------------------------------*/
static QEvt const * l_missileQueueSto[2];
static QEvt const * l_shipQueueSto[3];
static QEvt const * l_tunnelQueueSto[GAME_MINES_MAX + 5];

static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvt)];
    /* ... other event types to go into this pool */
} l_smlPoolSto[10];                     /* storage for the small event pool */

static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(ObjectPosEvt)];
    uint8_t e2[sizeof(ObjectImageEvt)];
    uint8_t e3[sizeof(MineEvt)];
    uint8_t e4[sizeof(ScoreEvt)];
    /* ... other event types to go into this pool */
} l_medPoolSto[2*GAME_MINES_MAX + 8];  /* storage for the medium event pool */

static QSubscrList    l_subscrSto[MAX_PUB_SIG];

/*..........................................................................*/
int main(int argc, char *argv[]) {
                          /* explicitly invoke the active objects' ctors... */
    Missile_ctor();
    Ship_ctor();
    Tunnel_ctor();

    QPApp app(argc, argv);

    Gui *gui = new Gui;
    gui->show();

    BSP_init();           /* initialize the Board Support Package */

    QF_init();     /* initialize the framework and the underlying RT kernel */

                                           /* initialize the event pools... */
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));   /* init publish-subscribe */

                            /* send object dictionaries for event queues... */
    QS_OBJ_DICTIONARY(l_missileQueueSto);
    QS_OBJ_DICTIONARY(l_shipQueueSto);
    QS_OBJ_DICTIONARY(l_tunnelQueueSto);

                             /* send object dictionaries for event pools... */
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);

               /* send signal dictionaries for globally published events... */
    QS_SIG_DICTIONARY(TIME_TICK_SIG,      0);
    QS_SIG_DICTIONARY(PLAYER_TRIGGER_SIG, 0);
    QS_SIG_DICTIONARY(PLAYER_QUIT_SIG,    0);
    QS_SIG_DICTIONARY(GAME_OVER_SIG,      0);

                                             /* start the active objects... */
    QActive_start(AO_Missile,
                  1,                                            /* priority */
                  (QEvt const **)0, (uint32_t)0,
                  (void *)0, (uint32_t)0,
                  (QEvt const *)0);
    QActive_start(AO_Ship,
                  2,                                            /* priority */
                  (QEvt const **)0, (uint32_t)0,
                  (void *)0, (uint32_t)0,
                  (QEvt const *)0);
    QActive_start(AO_Tunnel,
                  3,                                            /* priority */
                  (QEvt const **)0, (uint32_t)0,
                  (void *)0, (uint32_t)0,
                  (QEvt const *)0);
    return QF_run();                              /* run the QF application */
}




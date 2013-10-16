/*****************************************************************************
* Product: PELICAN crossing example
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 13, 2013
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
#include "pelican.h"

/*--------------------------------------------------------------------------*/
typedef struct PedTag {                         /* Pedestrian active object */
    QMActive super;                                /* derived from QMActive */
    uint8_t retryCtr;
} Ped;

static QState Ped_initial(Ped * const me);   /* initial pseudostate-handler */

static QState Ped_wait  (Ped * const me);
static QState Ped_wait_e(Ped * const me);
static QMState const Ped_wait_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Ped_wait),
    Q_ACTION_CAST(0)
};

static QState Ped_off  (Ped * const me);
static QState Ped_off_e(Ped * const me);
static QMState const Ped_off_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Ped_off),
    Q_ACTION_CAST(0)
};

enum PedTimeouts {                             /* various timeouts in ticks */
    N_ATTEMPTS = 10,                      /* number of PED_WAITING attempts */
    WAIT_TOUT = BSP_TICKS_PER_SEC * 3,  /* wait between posting PED_WAITING */
    OFF_TOUT  = BSP_TICKS_PER_SEC * 8    /* wait time after posting OFF_SIG */
};

/* Global objects ----------------------------------------------------------*/
Ped AO_Ped;                 /* the single instance of the Ped active object */

/*..........................................................................*/
void Ped_ctor(void) {
    QMActive_ctor((QActive *)&AO_Ped, Q_STATE_CAST(&Ped_initial));
}

/* MSM code ----------------------------------------------------------------*/
/* NOTE: the following code has been written by hand, but it really
* is intended for automatic generation by the free QM modeling tool.
* The QMSM code generation will be supported starting with QM 3.0.0.
*/
/*..........................................................................*/
static QState Ped_initial(Ped * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Ped_wait_e),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Ped_wait_, act_);
}

/*..........................................................................*/
static QState Ped_wait_e(Ped * const me) {
    BSP_showState(me->super.prio, "wait");
    me->retryCtr = N_ATTEMPTS;
    QActive_arm((QActive *)me, WAIT_TOUT);
    return QM_ENTRY(&Ped_wait_);
}
/*..........................................................................*/
static QState Ped_wait(Ped * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            if ((--me->retryCtr) != 0) {
                QActive_arm((QActive *)me, WAIT_TOUT);
                QACTIVE_POST((QActive *)&AO_Pelican, PEDS_WAITING_SIG);
                status_ = QM_HANDLED();
            }
            else {
                static QActionHandler const act_[] = {
                    Q_ACTION_CAST(&Ped_off_e),
                    Q_ACTION_CAST(0)
                };
                status_ = QM_TRAN(&Ped_off_, act_);
            }
            return Q_HANDLED();
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Ped_off_e(Ped * const me) {
    BSP_showState(me->super.prio, "off");
    QActive_arm((QActive *)me, OFF_TOUT);
    QACTIVE_POST((QActive *)&AO_Pelican, OFF_SIG);
    return QM_ENTRY(&Ped_off_);
}
/*..........................................................................*/
static QState Ped_off(Ped * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Ped_wait_e),
                Q_ACTION_CAST(0)
            };
            QACTIVE_POST((QActive *)&AO_Pelican, ON_SIG);
            status_ = QM_TRAN(&Ped_wait_, act_);
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

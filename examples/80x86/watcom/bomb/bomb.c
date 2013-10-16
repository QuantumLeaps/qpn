/*****************************************************************************
* Product: Time Bomb example
* Last Updated for Version: 5.0.0
* Date of the Last Update:  Aug 07, 2013
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
#include "bomb.h"

/* Active object class -----------------------------------------------------*/
typedef struct BombTag {
    QMActive super;                                 /* derive from QMActive */
    uint8_t ctr;                                        /* internal counter */
} Bomb;

static QState Bomb_initial (Bomb *me);

static QState Bomb_off     (Bomb *me);
static QState Bomb_off_e   (Bomb *me);
static QMState const Bomb_off_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Bomb_off),
    Q_ACTION_CAST(0)
};

static QState Bomb_timing  (Bomb *me);
static QState Bomb_timing_e(Bomb *me);
static QMState const Bomb_timing_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Bomb_timing),
    Q_ACTION_CAST(0)
};

static QState Bomb_blast   (Bomb *me);
static QState Bomb_blast_e (Bomb *me);
static QMState const Bomb_blast_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Bomb_blast),
    Q_ACTION_CAST(0)
};

/* Global objects ----------------------------------------------------------*/
Bomb AO_Bomb;              /* the single instance of the Bomb active object */

/*..........................................................................*/
void Bomb_ctor(void) {
    QMActive_ctor(&AO_Bomb.super, Q_STATE_CAST(&Bomb_initial));
}
/*..........................................................................*/
QState Bomb_initial(Bomb *me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Bomb_off_e),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Bomb_off_, act_);
}

/*..........................................................................*/
QState Bomb_off_e(Bomb *me) {
    BSP_ledOff();
    return QM_ENTRY(&Bomb_off_);
}
/*..........................................................................*/
QState Bomb_off(Bomb *me) {
    QState status_;
    switch (Q_SIG(me)) {
        case ARM_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Bomb_timing_e),
                Q_ACTION_CAST(0)
            };
            me->ctr = Q_PAR(me) * 4U;                /* arm the downcounter */
            status_ = QM_TRAN(&Bomb_timing_, act_);
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
QState Bomb_timing_e(Bomb *me) {
    QActive_armX(&me->super, 0U, BSP_TICKS_PER_SEC/4U);
    return QM_ENTRY(&Bomb_timing_);
}
/*..........................................................................*/
QState Bomb_timing(Bomb *me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            QActive_armX(&me->super, 0U, BSP_TICKS_PER_SEC/4U);
            if (me->ctr > 0U) {
                if ((me->ctr & 0x01U) != 0U) {
                    BSP_ledOn();
                }
                else {
                    BSP_ledOff();
                }
                --me->ctr;
                status_ = QM_HANDLED();
            }
            else {                                       /* timeout expired */
                static QActionHandler const act_[] = {
                    Q_ACTION_CAST(&Bomb_blast_e),
                    Q_ACTION_CAST(0)
                };
                status_ = QM_TRAN(&Bomb_blast_, act_);
            }
        }
    }
    return status_;
}

/*..........................................................................*/
QState Bomb_blast_e(Bomb *me) {
    QActive_armX(&me->super, 0U, 2U * BSP_TICKS_PER_SEC);
    BSP_ledOn();
    return QM_ENTRY(&Bomb_blast_);
}
/*..........................................................................*/
QState Bomb_blast(Bomb *me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Bomb_off_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Bomb_off_, act_);
        }
    }
    return status_;
}

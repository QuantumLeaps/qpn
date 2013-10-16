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

/*--------------------------------------------------------------------------*/
typedef struct OperTag {
    QMActive super;                                 /* derive from QMActive */
    uint8_t bomb_min;                   /* the minimum timeout for the bomb */
    uint8_t bomb_max;                   /* the maximum timeout for the bomb */
    uint8_t bomb_prev;                 /* the previous timeout for the bomb */
    uint8_t interval;               /* the interval between arming the bomb */
} Oper;

static QState Oper_initial(Oper *me);

static QState Oper_run    (Oper *me);
static QState Oper_run_e  (Oper *me);
static QState Oper_run_x  (Oper *me);
static QMState const Oper_run_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Oper_run),
    Q_ACTION_CAST(&Oper_run_x)
};

/* Global objects ----------------------------------------------------------*/
Oper AO_Oper;              /* the single instance of the Oper active object */

/*..........................................................................*/
void Oper_ctor(uint8_t min, uint8_t max, uint8_t every) {
    Oper *me = &AO_Oper;

    QMActive_ctor(&me->super, Q_STATE_CAST(&Oper_initial));
    me->bomb_min  = min;
    me->bomb_max  = max;
    me->bomb_prev = min;
    me->interval  = every;
}
/*..........................................................................*/
QState Oper_initial(Oper *me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Oper_run_e),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Oper_run_, act_);
}


/*..........................................................................*/
QState Oper_run_e(Oper *me) {
    QActive_armX(&me->super, 0U, me->interval);
    return QM_ENTRY(&Oper_run_);
}
/*..........................................................................*/
QState Oper_run_x(Oper *me) {
    uint8_t timeout = me->bomb_prev + 1;
    if (timeout > me->bomb_max) {
        timeout = me->bomb_min;
    }
    me->bomb_prev = timeout;
    QACTIVE_POST((QActive *)&AO_Bomb, ARM_SIG, timeout);
    return QM_EXIT(&Oper_run_);
}
/*..........................................................................*/
QState Oper_run(Oper *me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Oper_run_x),
                Q_ACTION_CAST(&Oper_run_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Oper_run_, act_);
            break;
        }
        case TERMINATE_SIG: {
            QF_stop();
            status_ = QM_HANDLED();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
void BSP_onKeyboardInputISR(uint8_t key) {
    if (key == 129U) {                                       /* ESC pressed? */
        QACTIVE_POST_ISR((QActive *)&AO_Oper, TERMINATE_SIG, 0U);
    }
}

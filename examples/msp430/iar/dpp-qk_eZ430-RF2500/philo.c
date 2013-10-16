/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 14, 2013
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
#include "qpn_port.h"                                       /* QP-nano port */
#include "dpp.h"                                   /* application interface */
#include "bsp.h"                             /* Board Support Package (BSP) */

//Q_DEFINE_THIS_MODULE("philo")

/* Active object class -----------------------------------------------------*/
typedef struct PhiloTag {
    QMActive super;                                     /* inherit QMActive */
} Philo;

static QState Philo_initial (Philo * const me);

static QState Philo_thinking  (Philo * const me);
static QState Philo_thinking_e(Philo * const me);
static QMState const Philo_thinking_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Philo_thinking),
    Q_ACTION_CAST(0)
};

static QState Philo_hungry  (Philo * const me);
static QState Philo_hungry_e(Philo * const me);
static QMState const Philo_hungry_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Philo_hungry),
    Q_ACTION_CAST(0)
};

static QState Philo_eating  (Philo * const me);
static QState Philo_eating_e(Philo * const me);
static QState Philo_eating_x(Philo * const me);
static QMState const Philo_eating_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Philo_eating),
    Q_ACTION_CAST(&Philo_eating_x)
};

#define THINK_TIME  17U
#define EAT_TIME    11U

/* Global objects ----------------------------------------------------------*/
Philo AO_Philo0;
Philo AO_Philo1;
Philo AO_Philo2;
//Philo AO_Philo3;
//Philo AO_Philo4;

/*..........................................................................*/
void Philo_ctor(void) {                    /* instantiate all Philo objects */
    QMActive_ctor(&AO_Philo0.super, Q_STATE_CAST(&Philo_initial));
    QMActive_ctor(&AO_Philo1.super, Q_STATE_CAST(&Philo_initial));
    QMActive_ctor(&AO_Philo2.super, Q_STATE_CAST(&Philo_initial));
//    QMActive_ctor(&AO_Philo3.super, Q_STATE_CAST(&Philo_initial));
//    QMActive_ctor(&AO_Philo4.super, Q_STATE_CAST(&Philo_initial));
}

/* MSM code ----------------------------------------------------------------*/
/* NOTE: the following code has been written by hand, but it really
* is intended for automatic generation by the free QM modeling tool.
* The QMSM code generation will be supported starting with QM 3.0.0.
*/
/*..........................................................................*/
static QState Philo_initial(Philo * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Philo_thinking_e),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Philo_thinking_, act_);
}

/*..........................................................................*/
static QState Philo_thinking_e(Philo * const me) {
    QActive_arm((QActive *)me, THINK_TIME);
    return QM_ENTRY(&Philo_thinking_);
}
/*..........................................................................*/
static QState Philo_thinking(Philo * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Philo_hungry_e),
                Q_ACTION_CAST(0)
            };
            BSP_busyDelay();
            status_ = QM_TRAN(&Philo_hungry_, act_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Philo_hungry_e(Philo * const me) {
    QACTIVE_POST((QActive *)&AO_Table, HUNGRY_SIG, me->super.prio);
    return QM_ENTRY(&Philo_hungry_);
}
/*..........................................................................*/
static QState Philo_hungry(Philo * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case EAT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Philo_eating_e),
                Q_ACTION_CAST(0)
            };
            BSP_busyDelay();
            status_ = QM_TRAN(&Philo_eating_, act_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Philo_eating_e(Philo * const me) {
    QActive_arm((QActive *)me, EAT_TIME);
    return QM_ENTRY(&Philo_eating_);
}
/*..........................................................................*/
static QState Philo_eating_x(Philo * const me) {
    BSP_busyDelay();
    QACTIVE_POST((QActive *)&AO_Table, DONE_SIG, me->super.prio);
    return QM_ENTRY(&Philo_eating_);
}
/*..........................................................................*/
static QState Philo_eating(Philo * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Philo_eating_x),
                Q_ACTION_CAST(&Philo_thinking_e),
                Q_ACTION_CAST(0)
            };
            BSP_busyDelay();
            status_ = QM_TRAN(&Philo_thinking_, act_);
            break;
        }
//        case EAT_SIG:
//        case DONE_SIG: {
//            Q_ERROR();    /* these events should never arrive in this state */
//            status_ = QM_HANDLED();
//            break;
//        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}


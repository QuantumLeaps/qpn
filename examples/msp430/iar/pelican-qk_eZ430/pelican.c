/*****************************************************************************
* Product: PELICAN crossing example
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
#include "qpn_port.h"
#include "bsp.h"
#include "pelican.h"

/* Pelican class -----------------------------------------------------------*/
typedef struct PelicanTag {
    QMActive super;                                /* derived from QMActive */
    uint8_t flashCtr;                           /* pedestrian flash counter */
} Pelican;

static QState Pelican_initial(Pelican *me);

static QState Pelican_offline  (Pelican * const me);
static QState Pelican_offline_e(Pelican * const me);
static QMState const Pelican_offline_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Pelican_offline),
    Q_ACTION_CAST(0)
};

static QState Pelican_operational  (Pelican * const me);
static QState Pelican_operational_e(Pelican * const me);
static QState Pelican_operational_i(Pelican * const me);
static QMState const Pelican_operational_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Pelican_operational),
    Q_ACTION_CAST(0)
};

static QState Pelican_carsEnabled  (Pelican * const me);
static QState Pelican_carsEnabled_x(Pelican * const me);
static QState Pelican_carsEnabled_i(Pelican * const me);
static QMState const Pelican_carsEnabled_ = {
    &Pelican_operational_,
    Q_STATE_CAST(&Pelican_carsEnabled),
    Q_ACTION_CAST(&Pelican_carsEnabled_x)
};

static QState Pelican_carsGreen  (Pelican * const me);
static QState Pelican_carsGreen_e(Pelican * const me);
static QState Pelican_carsGreen_i(Pelican * const me);
static QMState const Pelican_carsGreen_ = {
    &Pelican_carsEnabled_,
    Q_STATE_CAST(&Pelican_carsGreen),
    Q_ACTION_CAST(0)
};

static QState Pelican_carsGreenNoPed  (Pelican * const me);
static QState Pelican_carsGreenNoPed_e(Pelican * const me);
static QMState const Pelican_carsGreenNoPed_ = {
    &Pelican_carsGreen_,
    Q_STATE_CAST(&Pelican_carsGreenNoPed),
    Q_ACTION_CAST(0)
};

static QState Pelican_carsGreenPedWait  (Pelican * const me);
static QState Pelican_carsGreenPedWait_e(Pelican * const me);
static QMState const Pelican_carsGreenPedWait_ = {
    &Pelican_carsGreen_,
    Q_STATE_CAST(&Pelican_carsGreenPedWait),
    Q_ACTION_CAST(0)
};

static QState Pelican_carsGreenInt  (Pelican * const me);
static QState Pelican_carsGreenInt_e(Pelican * const me);
static QMState const Pelican_carsGreenInt_ = {
    &Pelican_carsGreen_,
    Q_STATE_CAST(&Pelican_carsGreenInt),
    Q_ACTION_CAST(0)
};

static QState Pelican_carsYellow  (Pelican * const me);
static QState Pelican_carsYellow_e(Pelican * const me);
static QMState const Pelican_carsYellow_ = {
    &Pelican_carsEnabled_,
    Q_STATE_CAST(&Pelican_carsYellow),
    Q_ACTION_CAST(0)
};

static QState Pelican_pedsEnabled  (Pelican * const me);
static QState Pelican_pedsEnabled_x(Pelican * const me);
static QState Pelican_pedsEnabled_i(Pelican * const me);
static QMState const Pelican_pedsEnabled_ = {
    &Pelican_operational_,
    Q_STATE_CAST(&Pelican_pedsEnabled),
    Q_ACTION_CAST(&Pelican_pedsEnabled_x)
};

static QState Pelican_pedsWalk  (Pelican * const me);
static QState Pelican_pedsWalk_e(Pelican * const me);
static QMState const Pelican_pedsWalk_ = {
    &Pelican_pedsEnabled_,
    Q_STATE_CAST(&Pelican_pedsWalk),
    Q_ACTION_CAST(0)
};

static QState Pelican_pedsFlash  (Pelican * const me);
static QState Pelican_pedsFlash_e(Pelican * const me);
static QMState const Pelican_pedsFlash_ = {
    &Pelican_pedsEnabled_,
    Q_STATE_CAST(&Pelican_pedsFlash),
    Q_ACTION_CAST(0)
};

enum PelicanTimeouts {                         /* various timeouts in ticks */
    CARS_GREEN_MIN_TOUT = BSP_TICKS_PER_SEC * 8,      /* min green for cars */
    CARS_YELLOW_TOUT = BSP_TICKS_PER_SEC * 3,            /* yellow for cars */
    PEDS_WALK_TOUT   = BSP_TICKS_PER_SEC * 3,      /* walking time for peds */
    PEDS_FLASH_TOUT  = BSP_TICKS_PER_SEC / 5,  /* flashing timeout for peds */
    PEDS_FLASH_NUM   = 5*2,                   /* number of flashes for peds */
    OFF_FLASH_TOUT   = BSP_TICKS_PER_SEC / 2   /* flashing timeout when off */
};

/* Global objects ----------------------------------------------------------*/
Pelican AO_Pelican;     /* the single instance of the Pelican active object */

/*..........................................................................*/
void Pelican_ctor(void) {
    QMActive_ctor((QActive *)&AO_Pelican, Q_STATE_CAST(&Pelican_initial));
}

/* MSM code ----------------------------------------------------------------*/
/* NOTE: the following code has been written by hand, but it really
* is intended for automatic generation by the free QM modeling tool.
* The QMSM code generation will be supported starting with QM 3.0.0.
*/
/*..........................................................................*/
static QState Pelican_initial(Pelican * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Pelican_operational_e),
        Q_ACTION_CAST(&Pelican_operational_i),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Pelican_operational_, act_);
}

/*..........................................................................*/
static QState Pelican_operational_e(Pelican * const me) {
    BSP_signalCars(CARS_RED);
    BSP_signalPeds(PEDS_DONT_WALK);
    return QM_ENTRY(&Pelican_operational_);
}
/*..........................................................................*/
static QState Pelican_operational_i(Pelican * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Pelican_carsEnabled_i),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Pelican_carsEnabled_, act_);
}
/*..........................................................................*/
static QState Pelican_operational(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case OFF_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_offline_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_offline_, act_);
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Pelican_carsEnabled_x(Pelican * const me) {
    BSP_signalCars(CARS_RED);
    return QM_EXIT(&Pelican_carsEnabled_);
}
/*..........................................................................*/
static QState Pelican_carsEnabled_i(Pelican * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Pelican_carsGreen_e),
        Q_ACTION_CAST(&Pelican_carsGreen_i),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Pelican_carsGreen_, act_);
}
/*..........................................................................*/
static QState Pelican_carsEnabled(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Pelican_carsGreen_e(Pelican * const me) {
    QActive_arm((QActive *)me, CARS_GREEN_MIN_TOUT);
    BSP_signalCars(CARS_GREEN);
    return QM_ENTRY(&Pelican_carsGreen_);
}
/*..........................................................................*/
static QState Pelican_carsGreen_i(Pelican * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Pelican_carsGreenNoPed_e),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Pelican_carsGreenNoPed_, act_);
}
/*..........................................................................*/
static QState Pelican_carsGreen(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Pelican_carsGreenNoPed_e(Pelican * const me) {
    BSP_showState(me->super.prio, "carsGreenNoPed");
    return QM_ENTRY(&Pelican_carsGreenNoPed_);
}
/*..........................................................................*/
static QState Pelican_carsGreenNoPed(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case PEDS_WAITING_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_carsGreenPedWait_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_carsGreenPedWait_, act_);
            break;
        }
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_carsGreenInt_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_carsGreenInt_, act_);
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
static QState Pelican_carsGreenPedWait_e(Pelican * const me) {
    BSP_showState(me->super.prio, "carsGreenPedWait");
    return QM_ENTRY(&Pelican_carsGreenPedWait_);
}
/*..........................................................................*/
static QState Pelican_carsGreenPedWait(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_carsYellow_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_carsYellow_, act_);
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
static QState Pelican_carsGreenInt_e(Pelican * const me) {
    BSP_showState(me->super.prio, "carsGreenInt");
    return QM_ENTRY(&Pelican_carsGreenInt_);
}
/*..........................................................................*/
static QState Pelican_carsGreenInt(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case PEDS_WAITING_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_carsYellow_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_carsYellow_, act_);
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Pelican_carsYellow_e(Pelican * const me) {
    BSP_showState(me->super.prio, "carsYellow");
    QActive_arm((QActive *)me, CARS_YELLOW_TOUT);
    BSP_signalCars(CARS_YELLOW);
    return QM_ENTRY(&Pelican_carsYellow_);
}
/*..........................................................................*/
static QState Pelican_carsYellow(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_carsEnabled_x),
                Q_ACTION_CAST(&Pelican_pedsEnabled_i),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_pedsEnabled_, act_);
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
static QState Pelican_pedsEnabled_x(Pelican * const me) {
    BSP_signalPeds(PEDS_DONT_WALK);
    return QM_EXIT(&Pelican_pedsEnabled_);
}
/*..........................................................................*/
static QState Pelican_pedsEnabled_i(Pelican * const me) {
    static QActionHandler const act_[] = {
        Q_ACTION_CAST(&Pelican_pedsWalk_e),
        Q_ACTION_CAST(0)
    };
    return QM_INITIAL(&Pelican_pedsWalk_, act_);
}
/*..........................................................................*/
static QState Pelican_pedsEnabled(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Pelican_pedsWalk_e(Pelican * const me) {
    BSP_showState(me->super.prio, "pedsWalk");
    QActive_arm((QActive *)me, PEDS_WALK_TOUT);
    BSP_signalPeds(PEDS_WALK);
    return QM_ENTRY(&Pelican_pedsWalk_);
}
/*..........................................................................*/
static QState Pelican_pedsWalk(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_pedsFlash_e),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_pedsFlash_, act_);
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
static QState Pelican_pedsFlash_e(Pelican * const me) {
    BSP_showState(me->super.prio, "pedsFlash");
    QActive_arm((QActive *)me, PEDS_FLASH_TOUT);
    me->flashCtr = PEDS_FLASH_NUM*2 + 1;
    return QM_ENTRY(&Pelican_pedsFlash_);
}
/*..........................................................................*/
static QState Pelican_pedsFlash(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            if (me->flashCtr != 0) {                     /* still flashing? */
                if ((me->flashCtr & 1) == 0) {             /* even counter? */
                    BSP_signalPeds(PEDS_DONT_WALK);
                }
                else {                               /* must be odd counter */
                    BSP_signalPeds(PEDS_BLANK);
                }
                --me->flashCtr;
                QActive_arm((QActive *)me, PEDS_FLASH_TOUT);
                status_ = QM_HANDLED();
                break;
            }
            else {                                         /* done flashing */
                static QActionHandler const act_[] = {
                    Q_ACTION_CAST(&Pelican_pedsEnabled_x),
                    Q_ACTION_CAST(&Pelican_carsEnabled_i),
                    Q_ACTION_CAST(0)
                };
                status_ = QM_TRAN(&Pelican_carsEnabled_, act_);
                break;
            }
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

/*..........................................................................*/
static QState Pelican_offline_e(Pelican * const me) {
    BSP_showState(me->super.prio, "offline");
    QActive_arm((QActive *)me, OFF_FLASH_TOUT);
    me->flashCtr = 0;
    return QM_ENTRY(&Pelican_offline_);
}
/*..........................................................................*/
static QState Pelican_offline(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, OFF_FLASH_TOUT);
            me->flashCtr ^= 1;                  /* toggle the flash counter */
            if ((me->flashCtr & 1) == 0) {                 /* even counter? */
                BSP_signalCars(CARS_RED);
                BSP_signalPeds(PEDS_DONT_WALK);
                status_ = QM_HANDLED();
                break;
            }
            else {
                BSP_signalCars(CARS_OFF);
                BSP_signalPeds(PEDS_BLANK);
                status_ = QM_HANDLED();
                break;
            }
        }
        case ON_SIG: {
            static QActionHandler const act_[] = {
                Q_ACTION_CAST(&Pelican_operational_e),
                Q_ACTION_CAST(&Pelican_operational_i),
                Q_ACTION_CAST(0)
            };
            status_ = QM_TRAN(&Pelican_operational_, act_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

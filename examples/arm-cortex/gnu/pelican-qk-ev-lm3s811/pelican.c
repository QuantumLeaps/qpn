/*****************************************************************************
* Product: PELICAN crossing example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 05, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"
#include "pelican.h"

/* Pelican class -----------------------------------------------------------*/
typedef struct PelicanTag {
    QActive super;                                  /* derived from QActive */
    uint8_t flashCtr;                           /* pedestrian flash counter */
} Pelican;

static QState Pelican_initial         (Pelican *me);
static QState Pelican_offline         (Pelican *me);
static QState Pelican_operational     (Pelican *me);
static QState Pelican_carsEnabled     (Pelican *me);
static QState Pelican_carsGreen       (Pelican *me);
static QState Pelican_carsGreenNoPed  (Pelican *me);
static QState Pelican_carsGreenPedWait(Pelican *me);
static QState Pelican_carsGreenInt    (Pelican *me);
static QState Pelican_carsYellow      (Pelican *me);
static QState Pelican_pedsEnabled     (Pelican *me);
static QState Pelican_pedsWalk        (Pelican *me);
static QState Pelican_pedsFlash       (Pelican *me);

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
    QActive_ctor((QActive *)&AO_Pelican, (QStateHandler)&Pelican_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Pelican_initial(Pelican *me) {
    return Q_TRAN(&Pelican_operational);
}
/*..........................................................................*/
QState Pelican_operational(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_signalCars(CARS_RED);
            BSP_signalPeds(PEDS_DONT_WALK);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Pelican_carsEnabled);
        }
        case OFF_SIG: {
            return Q_TRAN(&Pelican_offline);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Pelican_carsEnabled(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_EXIT_SIG: {
            BSP_signalCars(CARS_RED);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Pelican_carsGreen);
        }
    }
    return Q_SUPER(&Pelican_operational);
}
/*..........................................................................*/
QState Pelican_carsGreen(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, CARS_GREEN_MIN_TOUT);
            BSP_signalCars(CARS_GREEN);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Pelican_carsGreenNoPed);
        }
    }
    return Q_SUPER(&Pelican_carsEnabled);
}
/*..........................................................................*/
QState Pelican_carsGreenNoPed(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenNoPed  ");
            return Q_HANDLED();
        }
        case PEDS_WAITING_SIG: {
            return Q_TRAN(&Pelican_carsGreenPedWait);
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Pelican_carsGreenInt);
        }
    }
    return Q_SUPER(&Pelican_carsGreen);
}
/*..........................................................................*/
QState Pelican_carsGreenPedWait(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenPedWait");
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Pelican_carsYellow);
        }
    }
    return Q_SUPER(&Pelican_carsGreen);
}
/*..........................................................................*/
QState Pelican_carsGreenInt(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenInt    ");
            return Q_HANDLED();
        }
        case PEDS_WAITING_SIG: {
            return Q_TRAN(&Pelican_carsYellow);
        }
    }
    return Q_SUPER(&Pelican_carsGreen);
}
/*..........................................................................*/
QState Pelican_carsYellow(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsYellow      ");
            QActive_arm((QActive *)me, CARS_YELLOW_TOUT);
            BSP_signalCars(CARS_YELLOW);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Pelican_pedsEnabled);
        }
    }
    return Q_SUPER(&Pelican_carsEnabled);
}
/*..........................................................................*/
QState Pelican_pedsEnabled(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_EXIT_SIG: {
            BSP_signalPeds(PEDS_DONT_WALK);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Pelican_pedsWalk);
        }
    }
    return Q_SUPER(&Pelican_operational);
}
/*..........................................................................*/
QState Pelican_pedsWalk(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "pedsWalk        ");
            QActive_arm((QActive *)me, PEDS_WALK_TOUT);
            BSP_signalPeds(PEDS_WALK);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Pelican_pedsFlash);
        }
    }
    return Q_SUPER(&Pelican_pedsEnabled);
}
/*..........................................................................*/
QState Pelican_pedsFlash(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "pedsWalk        ");
            QActive_arm((QActive *)me, PEDS_FLASH_TOUT);
            me->flashCtr = PEDS_FLASH_NUM*2 + 1;
            return Q_HANDLED();
        }
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
            }
            else {                                         /* done flashing */
                return Q_TRAN(&Pelican_carsEnabled);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Pelican_pedsEnabled);
}
/*..........................................................................*/
QState Pelican_offline(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "offline         ");
            QActive_arm((QActive *)me, OFF_FLASH_TOUT);
            me->flashCtr = 0;
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, OFF_FLASH_TOUT);
            if ((me->flashCtr & 1) == 0) {                 /* even counter? */
                BSP_signalCars(CARS_RED);
                BSP_signalPeds(PEDS_DONT_WALK);
            }
            else {
                BSP_signalCars(CARS_OFF);
                BSP_signalPeds(PEDS_BLANK);
            }
            me->flashCtr ^= 1;                  /* toggle the flash counter */
            return Q_HANDLED();
        }
        case ON_SIG: {
            return Q_TRAN(&Pelican_operational);
        }
    }
    return Q_SUPER(&QHsm_top);
}

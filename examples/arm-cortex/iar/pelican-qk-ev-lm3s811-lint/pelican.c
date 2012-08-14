/*****************************************************************************
* Product: PELICAN crossing example
* Last Updated for Version: 4.5.01
* Date of the Last Update:  Jun 14, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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

static QState Pelican_initial         (Pelican * const me);
static QState Pelican_offline         (Pelican * const me);
static QState Pelican_operational     (Pelican * const me);
static QState Pelican_carsEnabled     (Pelican * const me);
static QState Pelican_carsGreen       (Pelican * const me);
static QState Pelican_carsGreenNoPed  (Pelican * const me);
static QState Pelican_carsGreenPedWait(Pelican * const me);
static QState Pelican_carsGreenInt    (Pelican * const me);
static QState Pelican_carsYellow      (Pelican * const me);
static QState Pelican_pedsEnabled     (Pelican * const me);
static QState Pelican_pedsWalk        (Pelican * const me);
static QState Pelican_pedsFlash       (Pelican * const me);

                                               /* various timeouts in ticks */
#define CARS_GREEN_MIN_TOUT (QTimeEvtCtr)(BSP_TICKS_PER_SEC * 8U)
#define CARS_YELLOW_TOUT    (QTimeEvtCtr)(BSP_TICKS_PER_SEC * 3U)
#define PEDS_WALK_TOUT      (QTimeEvtCtr)(BSP_TICKS_PER_SEC * 3U)
#define PEDS_FLASH_TOUT     (QTimeEvtCtr)(BSP_TICKS_PER_SEC / 5U)
#define PEDS_FLASH_NUM      (uint8_t)(5U * 2U)
#define OFF_FLASH_TOUT      (QTimeEvtCtr)(BSP_TICKS_PER_SEC / 2U)

/* Global objects ----------------------------------------------------------*/
struct PelicanTag AO_Pelican;      /* the single instance of the Pelican AO */

/*..........................................................................*/
void Pelican_ctor(void) {
    QActive_ctor(&AO_Pelican.super, Q_STATE_CAST(&Pelican_initial));
}

/* HSM definition ----------------------------------------------------------*/
static QState Pelican_initial(Pelican * const me) {
    return Q_TRAN(&Pelican_operational);
}
/*..........................................................................*/
static QState Pelican_operational(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_signalCars(CARS_RED);
            BSP_signalPeds(PEDS_DONT_WALK);
            ret = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            ret = Q_TRAN(&Pelican_carsEnabled);
            break;
        }
        case OFF_SIG: {
            ret = Q_TRAN(&Pelican_offline);
            break;
        }
        default: {
            ret = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_carsEnabled(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_EXIT_SIG: {
            BSP_signalCars(CARS_RED);
            ret = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            ret = Q_TRAN(&Pelican_carsGreen);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_operational);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_carsGreen(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm(&me->super, CARS_GREEN_MIN_TOUT);
            BSP_signalCars(CARS_GREEN);
            ret = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            ret = Q_TRAN(&Pelican_carsGreenNoPed);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_carsEnabled);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_carsGreenNoPed(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("carsGreenNoPed  ");
            ret = Q_HANDLED();
            break;
        }
        case PEDS_WAITING_SIG: {
            ret = Q_TRAN(&Pelican_carsGreenPedWait);
            break;
        }
        case Q_TIMEOUT_SIG: {
            ret = Q_TRAN(&Pelican_carsGreenInt);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_carsGreen);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_carsGreenPedWait(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("carsGreenPedWait");
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            ret = Q_TRAN(&Pelican_carsYellow);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_carsGreen);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_carsGreenInt(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("carsGreenInt    ");
            ret = Q_HANDLED();
            break;
        }
        case PEDS_WAITING_SIG: {
            ret = Q_TRAN(&Pelican_carsYellow);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_carsGreen);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_carsYellow(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("carsYellow      ");
            QActive_arm(&me->super, CARS_YELLOW_TOUT);
            BSP_signalCars(CARS_YELLOW);
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            ret = Q_TRAN(&Pelican_pedsEnabled);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_carsEnabled);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_pedsEnabled(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_EXIT_SIG: {
            BSP_signalPeds(PEDS_DONT_WALK);
            ret = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            ret = Q_TRAN(&Pelican_pedsWalk);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_operational);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_pedsWalk(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("pedsWalk        ");
            QActive_arm(&me->super, PEDS_WALK_TOUT);
            BSP_signalPeds(PEDS_WALK);
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            ret = Q_TRAN(&Pelican_pedsFlash);
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_pedsEnabled);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_pedsFlash(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("pedsWalk        ");
            QActive_arm(&me->super, PEDS_FLASH_TOUT);
            me->flashCtr = (PEDS_FLASH_NUM * 2U) + 1U;
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            if (me->flashCtr != 0U) {                    /* still flashing? */
                --me->flashCtr;
                QActive_arm(&me->super, PEDS_FLASH_TOUT);
                if ((me->flashCtr & 1U) == 0U) {           /* even counter? */
                    BSP_signalPeds(PEDS_DONT_WALK);
                    ret = Q_HANDLED();
                }
                else {                               /* must be odd counter */
                    BSP_signalPeds(PEDS_BLANK);
                    ret = Q_HANDLED();
                }
            }
            else {                                         /* done flashing */
                ret = Q_TRAN(&Pelican_carsEnabled);
            }
            break;
        }
        default: {
            ret = Q_SUPER(&Pelican_pedsEnabled);
            break;
        }
    }
    return ret;
}
/*..........................................................................*/
static QState Pelican_offline(Pelican * const me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState("offline         ");
            QActive_arm(&me->super, OFF_FLASH_TOUT);
            me->flashCtr = 0U;
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm(&me->super, OFF_FLASH_TOUT);
            me->flashCtr ^= 1U;                 /* toggle the flash counter */
            if ((me->flashCtr & 1U) == 0U) {               /* even counter? */
                BSP_signalCars(CARS_RED);
                BSP_signalPeds(PEDS_DONT_WALK);
                ret = Q_HANDLED();
            }
            else {
                BSP_signalCars(CARS_OFF);
                BSP_signalPeds(PEDS_BLANK);
                ret = Q_HANDLED();
            }
            break;
        }
        case ON_SIG: {
            ret = Q_TRAN(&Pelican_operational);
            break;
        }
        default: {
            ret = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return ret;
}

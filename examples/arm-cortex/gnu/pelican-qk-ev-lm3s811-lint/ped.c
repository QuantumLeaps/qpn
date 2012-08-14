/*****************************************************************************
* Product: PELICAN crossing example
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Feb 28, 2012
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

/*--------------------------------------------------------------------------*/
typedef struct PedTag {                         /* Pedestrian active object */
    QActive super;                                  /* derived from QActive */
    uint8_t retryCtr;
} Ped;

static QState Ped_initial(Ped *me);          /* initial pseudostate-handler */
static QState Ped_wait   (Ped *me);
static QState Ped_off    (Ped *me);

#define N_ATTEMPTS (uint8_t)10U
#define WAIT_TOUT  (QTimeEvtCtr)(BSP_TICKS_PER_SEC * 3U)
#define OFF_TOUT   (QTimeEvtCtr)(BSP_TICKS_PER_SEC * 8U)

/* Global objects ----------------------------------------------------------*/
struct PedTag AO_Ped;       /* the single instance of the Ped active object */

/*..........................................................................*/
void Ped_ctor(void) {
    QActive_ctor(&AO_Ped.super, Q_STATE_CAST(&Ped_initial));
}

/* HSM definition ----------------------------------------------------------*/
static QState Ped_initial(Ped *me) {
    return Q_TRAN(&Ped_wait);
}
/*..........................................................................*/
static QState Ped_wait(Ped *me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->retryCtr = N_ATTEMPTS;
            QActive_arm(&me->super, WAIT_TOUT);
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            --me->retryCtr;
            if (me->retryCtr != 0U) {
                QActive_arm(&me->super, WAIT_TOUT);
                QActive_post(QF_ACTIVE_CAST(&AO_Pelican),
                             (QSignal)PEDS_WAITING_SIG,
                             (QParam)0);
                ret = Q_HANDLED();
            }
            else {
                ret = Q_TRAN(&Ped_off);
            }
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
static QState Ped_off(Ped *me) {
    QState ret;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm(&me->super, OFF_TOUT);
            QActive_post(QF_ACTIVE_CAST(&AO_Pelican), (QSignal)OFF_SIG, 0U);
            ret = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            QActive_post(QF_ACTIVE_CAST(&AO_Pelican), (QSignal)ON_SIG, 0U);
            ret = Q_TRAN(&Ped_wait);
            break;
        }
        default: {
            ret = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return ret;
}

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

/*--------------------------------------------------------------------------*/
typedef struct PedTag {                         /* Pedestrian active object */
    QActive super;                                  /* derived from QActive */
    uint8_t retryCtr;
} Ped;

static QState Ped_initial(Ped *me);          /* initial pseudostate-handler */
static QState Ped_wait   (Ped *me);
static QState Ped_off    (Ped *me);

enum PedTimeouts {                             /* various timeouts in ticks */
    N_ATTEMPTS = 10,                      /* number of PED_WAITING attempts */
    WAIT_TOUT = BSP_TICKS_PER_SEC * 3,  /* wait between posting PED_WAITING */
    OFF_TOUT  = BSP_TICKS_PER_SEC * 8    /* wait time after posting OFF_SIG */
};

/* Global objects ----------------------------------------------------------*/
Ped AO_Ped;                 /* the single instance of the Ped active object */

/*..........................................................................*/
void Ped_ctor(void) {
    QActive_ctor((QActive *)&AO_Ped, (QStateHandler)&Ped_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Ped_initial(Ped *me) {
    return Q_TRAN(&Ped_wait);
}
/*..........................................................................*/
QState Ped_wait(Ped *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "wait");
            me->retryCtr = N_ATTEMPTS;
            QActive_arm((QActive *)me, WAIT_TOUT);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            if ((--me->retryCtr) != 0) {
                QActive_arm((QActive *)me, WAIT_TOUT);
                QActive_post((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0);
            }
            else {
                return Q_TRAN(&Ped_off);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Ped_off(Ped *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "off");
            QActive_arm((QActive *)me, OFF_TOUT);
            QActive_post((QActive *)&AO_Pelican, OFF_SIG, 0);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            QActive_post((QActive *)&AO_Pelican, ON_SIG, 0);
            return Q_TRAN(&Ped_wait);
        }
    }
    return Q_SUPER(&QHsm_top);
}

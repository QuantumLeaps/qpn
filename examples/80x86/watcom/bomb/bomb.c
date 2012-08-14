/*****************************************************************************
* Product: Time Bomb example
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
#include "bomb.h"

/* Active object class -----------------------------------------------------*/
typedef struct BombTag {
    QActive super;                                  /* derive from QActive */
    uint8_t ctr;                                       /* internal counter */
} Bomb;

static QState Bomb_initial(Bomb *me);
static QState Bomb_off    (Bomb *me);
static QState Bomb_timing (Bomb *me);
static QState Bomb_blast  (Bomb *me);

/* Global objects ----------------------------------------------------------*/
Bomb AO_Bomb;              /* the single instance of the Bomb active object */

/*..........................................................................*/
void Bomb_ctor(void) {
    QActive_ctor(&AO_Bomb.super, (QStateHandler)&Bomb_initial);
}
/*..........................................................................*/
QState Bomb_initial(Bomb *me) {
    return Q_TRAN(&Bomb_off);
}
/*..........................................................................*/
QState Bomb_off(Bomb *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_ledOff();
            return Q_HANDLED();
        }
        case ARM_SIG: {
            me->ctr = Q_PAR(me) * 4;                 /* arm the downcounter */
            return Q_TRAN(&Bomb_timing);
        }
    }
    return Q_IGNORED();
}
/*..........................................................................*/
QState Bomb_timing(Bomb *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/4);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/4);
            if (me->ctr > (uint8_t)0) {
                if ((me->ctr & 0x01) != 0) {
                    BSP_ledOn();
                }
                else {
                    BSP_ledOff();
                }
                --me->ctr;
            }
            else {                                       /* timeout expired */
                return Q_TRAN(&Bomb_blast);
            }
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}
/*..........................................................................*/
QState Bomb_blast(Bomb *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, 2 * BSP_TICKS_PER_SEC);
            BSP_ledOn();
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Bomb_off);
        }
    }
    return Q_IGNORED();
}

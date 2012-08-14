/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 05, 2007
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2007 Quantum Leaps, LLC. All rights reserved.
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
#include "game.h"

/* Q_DEFINE_THIS_FILE */

/* local objects -----------------------------------------------------------*/
typedef struct MissileTag {
    QActive super;                              /* extend the QActive class */
    uint8_t speed;
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
} Missile;                                     /* the Missile active object */

static QState Missile_initial  (Missile *me);
static QState Missile_armed    (Missile *me);
static QState Missile_flying   (Missile *me);
static QState Missile_exploding(Missile *me);

/* global objects ----------------------------------------------------------*/
Missile AO_Missile;

/*..........................................................................*/
void Missile_ctor(uint8_t speed) {
    Missile *me = &AO_Missile;
    QActive_ctor(&me->super, (QStateHandler)&Missile_initial);
    me->speed = speed;
}

/* HSM definition ----------------------------------------------------------*/
/*..........................................................................*/
QState Missile_initial(Missile *me) {
    return Q_TRAN(&Missile_armed);
}
/*..........................................................................*/
QState Missile_armed(Missile *me) {
    switch (Q_SIG(me)) {
        case MISSILE_FIRE_SIG: {
            me->x = (uint8_t)Q_PAR(me);      /* init position from the Ship */
            me->y = (uint8_t)(Q_PAR(me) >> 8);
            return Q_TRAN(&Missile_flying);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Missile_flying(Missile *me) {
    switch (Q_SIG(me)) {
        case TIME_TICK_SIG: {
            if (me->x + me->speed < GAME_SCREEN_WIDTH) {
                me->x += me->speed;
                /*tell the Tunnel to draw the Missile and test for wall hits*/
                QActive_post((QActive *)&AO_Tunnel, MISSILE_IMG_SIG,
                             ((QParam)MISSILE_BMP << 16)
                             | (QParam)me->x
                             | ((QParam)me->y << 8));
            }
            else { /* Missile outside the range, make it ready to fire again*/
                return Q_TRAN(&Missile_armed);
            }
            return Q_HANDLED();
        }
        case HIT_WALL_SIG: {
            return Q_TRAN(&Missile_exploding);
        }
        case DESTROYED_MINE_SIG: {
            /* tell the Ship the score for destroing this Mine */
            QActive_post((QActive *)&AO_Ship, Q_SIG(me), Q_PAR(me));

            /* re-arm immediately & let the destroyed Mine do the exploding */
            return Q_TRAN(&Missile_armed);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Missile_exploding(Missile *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ++me->exp_ctr;             /* advance the explosion counter */
                me->x -= me->speed;       /* move the explosion by one step */

                /* tell the Tunnel to render the current stage of Explosion */
                QActive_post((QActive *)&AO_Tunnel, EXPLOSION_SIG,
                         ((QParam)(EXPLOSION0_BMP + (me->exp_ctr >> 2)) << 16)
                         | (QParam)(me->x + 3)
                         | ((QParam)((int)me->y - 4) << 8));
            }
            else {          /* explosion finished or moved outside the game */
                return Q_TRAN(&Missile_armed);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

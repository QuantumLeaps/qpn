/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.1.00
* Date of the Last Update:  Oct 09, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
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

#define SHIP_WIDTH  5
#define SHIP_HEIGHT 3

/* local objects -----------------------------------------------------------*/
typedef struct ShipTag {
    QActive super;                              /* extend the QActive class */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
    uint16_t score;
} Ship;                                           /* the Ship active object */

static QState Ship_initial  (Ship *me);
static QState Ship_active   (Ship *me);
static QState Ship_parked   (Ship *me);
static QState Ship_flying   (Ship *me);
static QState Ship_exploding(Ship *me);

/* global objects ----------------------------------------------------------*/
Ship AO_Ship;

/*..........................................................................*/
void Ship_ctor(void) {
    Ship *me = &AO_Ship;
    QActive_ctor(&me->super, (QStateHandler)&Ship_initial);
    me->x = GAME_SHIP_X;
    me->y = GAME_SHIP_Y;
}

/* HSM definition ----------------------------------------------------------*/
/*..........................................................................*/
QState Ship_initial(Ship *me) {
    return Q_TRAN(&Ship_active);             /* top-most initial transition */
}
/*..........................................................................*/
QState Ship_active(Ship *me) {
    switch (Q_SIG(me)) {
        case Q_INIT_SIG: {                     /* nested initial transition */
            return Q_TRAN(&Ship_parked);
        }
        case PLAYER_SHIP_MOVE_SIG: {
            me->x = (uint8_t)Q_PAR(me);
            me->y = (uint8_t)(Q_PAR(me) >> 8);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Ship_parked(Ship *me) {
    switch (Q_SIG(me)) {
        case TAKE_OFF_SIG: {               /* permition to take off granted */
            return Q_TRAN(&Ship_flying);
        }
    }
    return Q_SUPER(&Ship_active);
}
/*..........................................................................*/
QState Ship_flying(Ship *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->score = 0;                               /* reset the score */
            QActive_post((QActive *)&AO_Tunnel, SCORE_SIG, me->score);
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            /* tell the Tunnel to draw the Ship and test for hits */
            QActive_post((QActive *)&AO_Tunnel, SHIP_IMG_SIG,
                         ((QParam)SHIP_BMP << 16)
                         | (QParam)me->x
                         | ((QParam)me->y << 8));

            ++me->score;  /* increment the score for surviving another tick */

            if ((me->score % 10) == 0) {           /* is the score "round"? */
                QActive_post((QActive *)&AO_Tunnel, SCORE_SIG, me->score);
            }

            return Q_HANDLED();
        }
        case PLAYER_TRIGGER_SIG: {                   /* trigger the Missile */
            QActive_post((QActive *)&AO_Missile, MISSILE_FIRE_SIG,
                         (QParam)me->x
                         | (((QParam)(me->y - 1 + SHIP_HEIGHT) & 0xFF) << 8));
            return Q_HANDLED();
        }
        case DESTROYED_MINE_SIG: {
            me->score += Q_PAR(me);
            /* the score will be sent to the Tunnel by the next TIME_TICK */
            return Q_HANDLED();
        }
        case HIT_WALL_SIG:
        case HIT_MINE_SIG: {
            return Q_TRAN(&Ship_exploding);
        }
    }
    return Q_SUPER(&Ship_active);
}
/*..........................................................................*/
QState Ship_exploding(Ship *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            if (me->exp_ctr < 15) {
                ++me->exp_ctr;

                  /* tell the Tunnel to draw the current stage of Explosion */
                QActive_post((QActive *)&AO_Tunnel, EXPLOSION_SIG,
                         ((QParam)(EXPLOSION0_BMP + (me->exp_ctr >> 2)) << 16)
                         | (QParam)me->x
                         | (((QParam)(me->y - 4 + SHIP_HEIGHT) & 0xFF) << 8));
            }
            else {
                QActive_post((QActive *)&AO_Tunnel, GAME_OVER_SIG, me->score);
                return Q_TRAN(&Ship_parked);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Ship_active);
}

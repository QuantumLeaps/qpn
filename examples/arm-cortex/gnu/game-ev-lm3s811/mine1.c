/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Feb 29, 2012
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
#include "game.h"

Q_DEFINE_THIS_MODULE("mine1")

/* local objects -----------------------------------------------------------*/
typedef struct Mine1Tag {
    QHsm super;                                    /* extend the QHsm class */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
} Mine1;

static QState Mine1_initial  (Mine1 *me);
static QState Mine1_unused   (Mine1 *me);
static QState Mine1_used     (Mine1 *me);
static QState Mine1_planted  (Mine1 *me);
static QState Mine1_exploding(Mine1 *me);

static Mine1 l_mine1[GAME_MINES_MAX];             /* a pool of type-1 mines */

                             /* helper macro to provide the ID of this mine */
#define MINE_ID(me_)    ((me_) - l_mine1)

/*..........................................................................*/
QHsm *Mine1_ctor(uint8_t id) {
    Mine1 *me;
    Q_REQUIRE(id < GAME_MINES_MAX);

    me = &l_mine1[id];
    QHsm_ctor(&me->super, (QStateHandler)&Mine1_initial);
    return (QHsm *)me;
}
/*..........................................................................*/
QState Mine1_initial(Mine1 *me) {
    return Q_TRAN(&Mine1_unused);
}
/*..........................................................................*/
QState Mine1_unused(Mine1 *me) {
    switch (Q_SIG(me)) {
        case MINE_PLANT_SIG: {
            me->x = (uint8_t)Q_PAR(me);
            me->y = (uint8_t)(Q_PAR(me) >> 8);
            return Q_TRAN(&Mine1_planted);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Mine1_used(Mine1 *me) {
    switch (Q_SIG(me)) {
        case Q_EXIT_SIG: {
            /* tell the Tunnel that this mine is becoming disabled */
            QActive_post((QActive *)&AO_Tunnel,
                         MINE_DISABLED_SIG, MINE_ID(me));
            return Q_HANDLED();
        }
        case MINE_RECYCLE_SIG: {
            return Q_TRAN(&Mine1_unused);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Mine1_planted(Mine1 *me) {
    uint8_t x;
    uint8_t y;
    uint8_t bmp;

    switch (Q_SIG(me)) {
        case TIME_TICK_SIG: {
            if (me->x >= GAME_SPEED_X) {
                me->x -= GAME_SPEED_X;              /* move the mine 1 step */

                /* tell the Tunnel to draw the Mine */
                QActive_post((QActive *)&AO_Tunnel, MINE_IMG_SIG,
                             ((QParam)MINE1_BMP << 16)
                             | (QParam)me->x
                             | ((QParam)me->y << 8));
            }
            else {
                return Q_TRAN(&Mine1_unused);
            }
            return Q_HANDLED();
        }
        case SHIP_IMG_SIG: {
            x   = (uint8_t)Q_PAR(me);
            y   = (uint8_t)(Q_PAR(me) >> 8);
            bmp = (uint8_t)(Q_PAR(me) >> 16);

            /* test for incoming Ship hitting this mine */
            if (do_bitmaps_overlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                QActive_post((QActive *)&AO_Ship, HIT_MINE_SIG,  1);

                /* go straight to 'disabled' and let the Ship do
                * the exploding
                */
                return Q_TRAN(&Mine1_unused);
            }
            return Q_HANDLED();
        }
        case MISSILE_IMG_SIG: {
            x   = (uint8_t)Q_PAR(me);
            y   = (uint8_t)(Q_PAR(me) >> 8);
            bmp = (uint8_t)(Q_PAR(me) >> 16);

            /* test for incoming Missile hitting this mine */
            if (do_bitmaps_overlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                               /* post the score for destroying Mine type-1 */
                QActive_post((QActive *)&AO_Missile, DESTROYED_MINE_SIG, 25);
                return Q_TRAN(&Mine1_exploding);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Mine1_used);
}
/*..........................................................................*/
QState Mine1_exploding(Mine1 *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ++me->exp_ctr;             /* advance the explosion counter */
                me->x -= GAME_SPEED_X;          /* move explosion by 1 step */

                /* tell the Game to render the current stage of Explosion */
                QActive_post((QActive *)&AO_Tunnel, EXPLOSION_SIG,
                         ((QParam)(EXPLOSION0_BMP + (me->exp_ctr >> 2)) << 16)
                         | ((QParam)(me->x + 1))
                         | (((QParam)((int)me->y - 4 + 2)) << 8));
            }
            else {
                return Q_TRAN(&Mine1_unused);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Mine1_used);
}

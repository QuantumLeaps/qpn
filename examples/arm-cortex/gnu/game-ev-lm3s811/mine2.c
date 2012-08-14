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

Q_DEFINE_THIS_MODULE("mine2")

/* local objects -----------------------------------------------------------*/
typedef struct Mine2Tag {
    QHsm super;                                    /* extend the QHsm class */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
} Mine2;

static QState Mine2_initial  (Mine2 *me);
static QState Mine2_unused   (Mine2 *me);
static QState Mine2_used     (Mine2 *me);
static QState Mine2_planted  (Mine2 *me);
static QState Mine2_exploding(Mine2 *me);

static Mine2 l_mine2[GAME_MINES_MAX];             /* a pool of type-2 mines */

                             /* helper macro to provide the ID of this mine */
#define MINE_ID(me_)    ((me_) - l_mine2)

/*..........................................................................*/
QHsm *Mine2_ctor(uint8_t id) {
    Mine2 *me;
    Q_REQUIRE(id < GAME_MINES_MAX);

    me = &l_mine2[id];
    QHsm_ctor(&me->super, (QStateHandler)&Mine2_initial);
    return (QHsm *)me;
}
/*..........................................................................*/
QState Mine2_initial(Mine2 *me) {
    return Q_TRAN(&Mine2_unused);
}
/*..........................................................................*/
QState Mine2_unused(Mine2 *me) {
    switch (Q_SIG(me)) {
        case MINE_PLANT_SIG: {
            me->x = (uint8_t)Q_PAR(me);
            me->y = (uint8_t)(Q_PAR(me) >> 8);
            return Q_TRAN(&Mine2_planted);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Mine2_used(Mine2 *me) {
    switch (Q_SIG(me)) {
        case Q_EXIT_SIG: {
            /* tell the Tunnel that this mine is becoming disabled */
            QActive_post((QActive *)&AO_Tunnel,
                         MINE_DISABLED_SIG, MINE_ID(me));
            return Q_HANDLED();
        }
        case MINE_RECYCLE_SIG: {
            return Q_TRAN(&Mine2_unused);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Mine2_planted(Mine2 *me) {
    uint8_t x;
    uint8_t y;
    uint8_t bmp;

    switch (Q_SIG(me)) {
        case TIME_TICK_SIG: {
            if (me->x >= GAME_SPEED_X) {
                me->x -= GAME_SPEED_X;              /* move the mine 1 step */

                /* tell the Tunnel to draw the Mine */
                QActive_post((QActive *)&AO_Tunnel, MINE_IMG_SIG,
                             ((QParam)MINE2_BMP << 16)
                             | (QParam)me->x
                             | ((QParam)me->y << 8));
            }
            else {
                return Q_TRAN(&Mine2_unused);
            }
            return Q_HANDLED();
        }
        case SHIP_IMG_SIG: {
            x   = (uint8_t)Q_PAR(me);
            y   = (uint8_t)(Q_PAR(me) >> 8);
            bmp = (uint8_t)(Q_PAR(me) >> 16);

            /* test for incoming Ship hitting this mine */
            if (do_bitmaps_overlap(MINE2_BMP, me->x, me->y, bmp, x, y)) {
                QActive_post((QActive *)&AO_Ship, HIT_MINE_SIG,  2);

                /* go straight to 'disabled' and let the Ship do
                * the exploding
                */
                return Q_TRAN(&Mine2_unused);
            }
            return Q_HANDLED();
        }
        case MISSILE_IMG_SIG: {
            x   = (uint8_t)Q_PAR(me);
            y   = (uint8_t)(Q_PAR(me) >> 8);
            bmp = (uint8_t)(Q_PAR(me) >> 16);

            /* test for incoming Missile hitting this mine */
            /* NOTE: Mine type-2 is nastier than Mine type-1.
            * The type-2 mine can hit the Ship with any of its
            * "tentacles". However, it can be destroyed by the
            * Missile only by hitting its center, defined as
            * a smaller bitmap MINE2_MISSILE_BMP.
            */
            if (do_bitmaps_overlap(MINE2_MISSILE_BMP,
                                   me->x, me->y, bmp, x, y))
            {
                               /* post the score for destroying Mine type-2 */
                QActive_post((QActive *)&AO_Missile, DESTROYED_MINE_SIG, 45);
                return Q_TRAN(&Mine2_exploding);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Mine2_used);
}
/*..........................................................................*/
QState Mine2_exploding(Mine2 *me) {
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
                return Q_TRAN(&Mine2_unused);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Mine2_used);
}

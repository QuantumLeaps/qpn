/*$file${.::mine1.c} #######################################################*/
/*
* Model: game.qm
* File:  ${.::mine1.c}
*
* This code has been generated by QM tool (https://state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*$endhead${.::mine1.c} ####################################################*/
#include "qpn.h"
#include "game.h"
#include "bsp.h"

Q_DEFINE_THIS_MODULE("mine1")

/* encapsulated delcaration of the Mine1 HSM -------------------------------*/
/*$declare${AOs::Mine1} ####################################################*/
/*${AOs::Mine1} ............................................................*/
typedef struct Mine1 {
/* protected: */
    QHsm super;

/* private: */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
} Mine1;

/* protected: */
static QState Mine1_initial(Mine1 * const me);
static QState Mine1_unused(Mine1 * const me);
static QState Mine1_used(Mine1 * const me);
static QState Mine1_planted(Mine1 * const me);
static QState Mine1_exploding(Mine1 * const me);
/*$enddecl${AOs::Mine1} ####################################################*/

/* local objects -----------------------------------------------------------*/
static Mine1 l_mine1[GAME_MINES_MAX]; /* a pool of type-1 mines */

/* helper macro to provide the ID of this mine */
#define MINE_ID(me_)    ((me_) - l_mine1)

/* Mine1 class definition --------------------------------------------------*/
/*$define${AOs::Mine1_ctor} ################################################*/
/* Check for the minimum required QP version */
#if ((QP_VERSION < 601) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8)))
#error qpn version 6.0.1 or higher required
#endif
/*${AOs::Mine1_ctor} .......................................................*/
QHsm * Mine1_ctor(uint8_t id) {
    Mine1 *me;
    Q_REQUIRE(id < GAME_MINES_MAX);

    me = &l_mine1[id];
    QHsm_ctor(&me->super, Q_STATE_CAST(&Mine1_initial));
    return &me->super;
}
/*$enddef${AOs::Mine1_ctor} ################################################*/
/*$define${AOs::Mine1} #####################################################*/
/*${AOs::Mine1} ............................................................*/
/*${AOs::Mine1::SM} ........................................................*/
static QState Mine1_initial(Mine1 * const me) {
    /*${AOs::Mine1::SM::initial} */
    return Q_TRAN(&Mine1_unused);
}
/*${AOs::Mine1::SM::unused} ................................................*/
static QState Mine1_unused(Mine1 * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Mine1::SM::unused::MINE_PLANT} */
        case MINE_PLANT_SIG: {
            me->x = (uint8_t)Q_PAR(me);
            me->y = (uint8_t)(Q_PAR(me) >> 8);
            status_ = Q_TRAN(&Mine1_planted);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Mine1::SM::used} ..................................................*/
static QState Mine1_used(Mine1 * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Mine1::SM::used} */
        case Q_EXIT_SIG: {
            /* tell the Tunnel that this mine is becoming disabled */
            QACTIVE_POST(&AO_Tunnel, MINE_DISABLED_SIG, MINE_ID(me));
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Mine1::SM::used::MINE_RECYCLE} */
        case MINE_RECYCLE_SIG: {
            status_ = Q_TRAN(&Mine1_unused);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Mine1::SM::used::planted} .........................................*/
static QState Mine1_planted(Mine1 * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Mine1::SM::used::planted::TIME_TICK} */
        case TIME_TICK_SIG: {
            /*${AOs::Mine1::SM::used::planted::TIME_TICK::[me->x>=GAME_SPEED_X]} */
            if (me->x >= GAME_SPEED_X) {
                me->x -= GAME_SPEED_X;  /* move the mine 1 step */

                /* tell the Tunnel to draw the Mine */
                QACTIVE_POST(&AO_Tunnel, MINE_IMG_SIG,
                             (MINE1_BMP << 16)
                             | me->x
                             | (me->y << 8));
                status_ = Q_HANDLED();
            }
            /*${AOs::Mine1::SM::used::planted::TIME_TICK::[else]} */
            else {
                status_ = Q_TRAN(&Mine1_unused);
            }
            break;
        }
        /*${AOs::Mine1::SM::used::planted::SHIP_IMG} */
        case SHIP_IMG_SIG: {
            uint8_t x   = (uint8_t)Q_PAR(me);
            uint8_t y   = (uint8_t)(Q_PAR(me) >> 8);
            uint8_t bmp = (uint8_t)(Q_PAR(me) >> 16);
            /*${AOs::Mine1::SM::used::planted::SHIP_IMG::[BSP_doBitmapsOverlap(MINE1_BMP,~} */
            if (BSP_doBitmapsOverlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                QACTIVE_POST(&AO_Ship, HIT_MINE_SIG,  1);

                /* go straight to 'disabled' and let the Ship do
                * the exploding
                */
                status_ = Q_TRAN(&Mine1_unused);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /*${AOs::Mine1::SM::used::planted::MISSILE_IMG} */
        case MISSILE_IMG_SIG: {
            uint8_t x   = (uint8_t)Q_PAR(me);
            uint8_t y   = (uint8_t)(Q_PAR(me) >> 8);
            uint8_t bmp = (uint8_t)(Q_PAR(me) >> 16);
            /*${AOs::Mine1::SM::used::planted::MISSILE_IMG::[BSP_doBitmapsOverlap(MINE1_BMP,~} */
            if (BSP_doBitmapsOverlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                /* post the score for destroying Mine type-1 */
                QACTIVE_POST(&AO_Missile, DESTROYED_MINE_SIG, 25);
                status_ = Q_TRAN(&Mine1_exploding);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Mine1_used);
            break;
        }
    }
    return status_;
}
/*${AOs::Mine1::SM::used::exploding} .......................................*/
static QState Mine1_exploding(Mine1 * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Mine1::SM::used::exploding} */
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Mine1::SM::used::exploding::TIME_TICK} */
        case TIME_TICK_SIG: {
            /*${AOs::Mine1::SM::used::exploding::TIME_TICK::[(me->x>=GAME_SPEED_X)&&(me->exp~} */
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ++me->exp_ctr;  /* advance the explosion counter */
                me->x -= GAME_SPEED_X; /* move explosion by 1 step */

                /* tell the Game to render the current stage of Explosion */
                QACTIVE_POST(&AO_Tunnel, EXPLOSION_SIG,
                         ((EXPLOSION0_BMP + (me->exp_ctr >> 2)) << 16)
                         | (me->x + 1)
                         | (((int)me->y - 4 + 2)) << 8);
                status_ = Q_HANDLED();
            }
            /*${AOs::Mine1::SM::used::exploding::TIME_TICK::[else]} */
            else {
                status_ = Q_TRAN(&Mine1_unused);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Mine1_used);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Mine1} #####################################################*/

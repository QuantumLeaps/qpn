/*$file${.::missile.c} #####################################################*/
/*
* Model: game.qm
* File:  ${.::missile.c}
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
/*$endhead${.::missile.c} ##################################################*/
#include "qpn.h"
#include "game.h"
#include "bsp.h"

/* Q_DEFINE_THIS_MODULE("missile") */

/* encapsulated delcaration of the Missile active object -------------------*/
/*$declare${AOs::Missile} ##################################################*/
/*${AOs::Missile} ..........................................................*/
typedef struct Missile {
/* protected: */
    QActive super;

/* private: */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;

/* public: */
    uint8_t speed;
} Missile;

/* protected: */
static QState Missile_initial(Missile * const me);
static QState Missile_armed(Missile * const me);
static QState Missile_flying(Missile * const me);
static QState Missile_exploding(Missile * const me);
/*$enddecl${AOs::Missile} ##################################################*/

/* global objects ----------------------------------------------------------*/
Missile AO_Missile;

/* Active object definition ------------------------------------------------*/
/*$define${AOs::Missile_ctor} ##############################################*/
/* Check for the minimum required QP version */
#if ((QP_VERSION < 601) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8)))
#error qpn version 6.0.1 or higher required
#endif
/*${AOs::Missile_ctor} .....................................................*/
void Missile_ctor(uint8_t speed) {
    Missile *me = &AO_Missile;
    QActive_ctor(&me->super, Q_STATE_CAST(&Missile_initial));
    me->speed = speed;
}
/*$enddef${AOs::Missile_ctor} ##############################################*/
/*$define${AOs::Missile} ###################################################*/
/*${AOs::Missile} ..........................................................*/
/*${AOs::Missile::SM} ......................................................*/
static QState Missile_initial(Missile * const me) {
    /*${AOs::Missile::SM::initial} */
    return Q_TRAN(&Missile_armed);
}
/*${AOs::Missile::SM::armed} ...............................................*/
static QState Missile_armed(Missile * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Missile::SM::armed::MISSILE_FIRE} */
        case MISSILE_FIRE_SIG: {
            me->x = (uint8_t)Q_PAR(me); /* init position from the Ship */
            me->y = (uint8_t)(Q_PAR(me) >> 8);
            status_ = Q_TRAN(&Missile_flying);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Missile::SM::flying} ..............................................*/
static QState Missile_flying(Missile * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Missile::SM::flying::TIME_TICK} */
        case TIME_TICK_SIG: {
            /*${AOs::Missile::SM::flying::TIME_TICK::[me->x+GAME_MISSILE_SPEED_X<GAME~} */
            if (me->x + GAME_MISSILE_SPEED_X < GAME_TUNNEL_WIDTH) {
                me->x += me->speed;
                /*tell the Tunnel to draw the Missile and test for wall hits*/
                QACTIVE_POST(&AO_Tunnel, MISSILE_IMG_SIG,
                             (MISSILE_BMP << 16)
                             | me->x
                             | (me->y << 8));
                status_ = Q_HANDLED();
            }
            /*${AOs::Missile::SM::flying::TIME_TICK::[else]} */
            else {
                status_ = Q_TRAN(&Missile_armed);
            }
            break;
        }
        /*${AOs::Missile::SM::flying::HIT_WALL} */
        case HIT_WALL_SIG: {
            status_ = Q_TRAN(&Missile_exploding);
            break;
        }
        /*${AOs::Missile::SM::flying::DESTROYED_MINE} */
        case DESTROYED_MINE_SIG: {
            /* tell the Ship the score for destroing this Mine */
            QACTIVE_POST(&AO_Ship, Q_SIG(me), Q_PAR(me));
            /* re-arm immediately & let the destroyed Mine do the exploding */
            status_ = Q_TRAN(&Missile_armed);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Missile::SM::exploding} ...........................................*/
static QState Missile_exploding(Missile * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::Missile::SM::exploding} */
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Missile::SM::exploding::TIME_TICK} */
        case TIME_TICK_SIG: {
            /*${AOs::Missile::SM::exploding::TIME_TICK::[(me->x>=GAME_SPEED_X)&&(me->exp~} */
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ++me->exp_ctr;             /* advance the explosion counter */
                me->x -= me->speed;       /* move the explosion by one step */

                /* tell the Tunnel to render the current stage of Explosion */
                QACTIVE_POST(&AO_Tunnel, EXPLOSION_SIG,
                         ((EXPLOSION0_BMP + (me->exp_ctr >> 2)) << 16)
                         | (me->x + 3)
                         | ((int)me->y - 4) << 8);
                status_ = Q_HANDLED();
            }
            /*${AOs::Missile::SM::exploding::TIME_TICK::[else]} */
            else {
                /* explosion finished or moved outside the game */
                status_ = Q_TRAN(&Missile_armed);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Missile} ###################################################*/

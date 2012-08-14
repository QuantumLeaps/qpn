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
#include "game.h"
#include "bsp.h"

#include <string.h>                           /* for memmove() and memcpy() */

Q_DEFINE_THIS_MODULE("tunnel")

/* Tunnel Active Object ....................................................*/
typedef struct TunnelTag {
    QActive super;                              /* extend the QActive class */

    QHsm *mines[GAME_MINES_MAX];                            /* active mines */
    QHsm *mine1_pool[GAME_MINES_MAX];
    QHsm *mine2_pool[GAME_MINES_MAX];

    uint8_t blink_ctr;                                     /* blink counter */

    uint8_t last_mine_x;
    uint8_t last_mine_y;

    uint8_t wall_thickness_top;
    uint8_t wall_thickness_bottom;
    uint8_t minimal_gap;
} Tunnel;                                       /* the Tunnel active object */

static QState Tunnel_initial          (Tunnel *me);
static QState Tunnel_final            (Tunnel *me);
static QState Tunnel_active           (Tunnel *me);
static QState Tunnel_playing          (Tunnel *me);
static QState Tunnel_demo             (Tunnel *me);
static QState Tunnel_game_over        (Tunnel *me);
static QState Tunnel_screen_saver     (Tunnel *me);
static QState Tunnel_screen_saver_hide(Tunnel *me);
static QState Tunnel_screen_saver_show(Tunnel *me);

/* Helper functions ........................................................*/
static void Tunnel_advance(Tunnel *me);
static void Tunnel_plantMine(Tunnel *me);
static void Tunnel_addImageAt(Tunnel *me, uint8_t bmp,
                              uint8_t x, int8_t y);
static void Tunnel_dispatchToAllMines(Tunnel *me, QSignal sig, QParam par);
static uint8_t Tunnel_isWallHit(Tunnel *me, uint8_t bmp,
                                uint8_t x_pos, uint8_t y_pos);

static void randomSeed(uint32_t seed);
static uint32_t random(void);

/* global objects ----------------------------------------------------------*/
Tunnel AO_Tunnel;

/* local objects -----------------------------------------------------------*/
static uint32_t l_rnd;                                       /* random seed */
static uint8_t l_walls[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8];
static uint8_t l_frame[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8];


/*..........................................................................*/
void Tunnel_ctor(void) {
    uint8_t n;
    Tunnel *me = &AO_Tunnel;
    QActive_ctor(&me->super, (QStateHandler)&Tunnel_initial);
    for (n = 0; n < GAME_MINES_MAX; ++n) {
        me->mine1_pool[n] = Mine1_ctor(n); /* instantiate Mine1 in the pool */
        me->mine2_pool[n] = Mine2_ctor(n); /* instantiate Mine2 in the pool */
        me->mines[n] = (QHsm *)0;                     /* mine 'n' is unused */
    }
    me->last_mine_x = 0;   /* the last mine at the right edge of the tunnel */
    me->last_mine_y = 0;
}

/* HSM definition ----------------------------------------------------------*/
/*..........................................................................*/
QState Tunnel_initial(Tunnel *me) {
    uint8_t n;
    for (n = 0; n < GAME_MINES_MAX; ++n) {
        QHsm_init(me->mine1_pool[n]);            /* initial tran. for Mine1 */
        QHsm_init(me->mine2_pool[n]);            /* initial tran. for Mine2 */
    }

    randomSeed(1234);                   /* seed the pseudo-random generator */
    return Q_TRAN(&Tunnel_demo);
}
/*..........................................................................*/
QState Tunnel_final(Tunnel *me) {
    (void)me;                                     /* avoid compiler warning */
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* clear the screen */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
            BSP_drawBitmap(l_frame);
            QF_stop();                               /* stop QF and cleanup */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Tunnel_active(Tunnel *me) {
    switch (Q_SIG(me)) {
        case MINE_DISABLED_SIG: {
            Q_ASSERT((Q_PAR(me) < GAME_MINES_MAX)
                     && (me->mines[Q_PAR(me)] != (QHsm *)0));
            me->mines[Q_PAR(me)] = (QHsm *)0;
            return Q_HANDLED();
        }
        case PLAYER_QUIT_SIG: {
            return Q_TRAN(&Tunnel_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Tunnel_demo(Tunnel *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->last_mine_x = 0;   /* last mine at right edge of the tunnel */
            me->last_mine_y = 0;
                                            /* set the tunnel properties... */
            me->wall_thickness_top = 0;
            me->wall_thickness_bottom = 0;
            me->minimal_gap = GAME_SCREEN_HEIGHT - 3;

            /* erase the tunnel walls */
            memset(l_walls, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));


            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2);     /* 1/2 sec */
            me->blink_ctr = 20*2;                            /* 20s timeout */
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm((QActive *)me);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2);     /* 1/2 sec */
            if ((--me->blink_ctr) == 0) {
                return Q_TRAN(&Tunnel_screen_saver);
            }
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            Tunnel_advance(me);
            if ((me->blink_ctr & 1) != 0) {
                /* add the text bitmap into the frame buffer */
                Tunnel_addImageAt(me,
                    PRESS_BUTTON_BMP,
                    (GAME_SCREEN_WIDTH - 55)/2,
                    (GAME_SCREEN_HEIGHT - 8)/2);
            }
            BSP_drawBitmap(l_frame);
            return Q_HANDLED();
        }
        case PLAYER_TRIGGER_SIG: {
            return Q_TRAN(&Tunnel_playing);
        }
    }
    return Q_SUPER(&Tunnel_active);
}
/*..........................................................................*/
QState Tunnel_game_over(Tunnel *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2);     /* 1/2 sec */
            me->blink_ctr = 5*2;                              /* 5s timeout */
            BSP_drawNString((GAME_SCREEN_WIDTH - 6*9)/2, 0, "Game Over");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm((QActive *)me);
            BSP_updateScore(0);          /* update the score on the display */
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2);     /* 1/2 sec */
            BSP_drawNString((GAME_SCREEN_WIDTH - 6*9)/2, 0,
                            (((me->blink_ctr & 1) != 0)
                             ? "Game Over"
                             : "         "));
            if ((--me->blink_ctr) == 0) {
                return Q_TRAN(&Tunnel_demo);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tunnel_active);
}
/*..........................................................................*/
QState Tunnel_playing(Tunnel *me) {
    uint8_t x;
    int8_t  y;
    uint8_t bmp;

    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            me->minimal_gap = GAME_SCREEN_HEIGHT - 3;

            /* erase the walls */
            memset(l_walls, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));

            QActive_post((QActive *)&AO_Ship, TAKE_OFF_SIG, 0);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            Tunnel_dispatchToAllMines(me, MINE_RECYCLE_SIG, 0);
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            /* render this frame on the display */
            BSP_drawBitmap(l_frame);

            Tunnel_advance(me);
            Tunnel_plantMine(me);
            Tunnel_dispatchToAllMines(me, Q_SIG(me), Q_PAR(me));

            return Q_HANDLED();
        }
        case SHIP_IMG_SIG:
        case MISSILE_IMG_SIG: {
            x   = (uint8_t)Q_PAR(me);
            y   = (uint8_t)(Q_PAR(me) >> 8);
            bmp = (uint8_t)(Q_PAR(me) >> 16);

                               /* did the Ship/Missile hit the tunnel wall? */
            if (Tunnel_isWallHit(me, bmp, x, y)) {
                if (Q_SIG(me) == SHIP_IMG_SIG) {
                    QActive_post((QActive *)&AO_Ship, HIT_WALL_SIG, 0);
                }
                else {
                    QActive_post((QActive *)&AO_Missile, HIT_WALL_SIG, 0);
                }
            }
            Tunnel_addImageAt(me, bmp, x, y);
            Tunnel_dispatchToAllMines(me, Q_SIG(me), Q_PAR(me));
            return Q_HANDLED();
        }
        case MINE_IMG_SIG:
        case EXPLOSION_SIG: {
            x   = (uint8_t)Q_PAR(me);
            y   = (int8_t) (Q_PAR(me) >> 8);
            bmp = (uint8_t)(Q_PAR(me) >> 16);

            Tunnel_addImageAt(me, bmp, x, y);
            return Q_HANDLED();
        }
        case SCORE_SIG: {
            BSP_updateScore((uint16_t)Q_PAR(me));

            /* increase difficulty of the game:
            * the tunnel gets narrower as the score goes up
            */
            me->minimal_gap = (uint8_t)(GAME_SCREEN_HEIGHT - 3U
                                        - (Q_PAR(me)/2000U));
            return Q_HANDLED();
        }
        case GAME_OVER_SIG: {
            uint16_t score = (uint16_t)Q_PAR(me);
            char str[5];

            BSP_updateScore(score);

            /* clear the screen */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
            BSP_drawBitmap(l_frame);

            /* Output the final score to the screen */
            BSP_drawNString((GAME_SCREEN_WIDTH - 6*10)/2, 1, "Score:");
            str[4] = '\0';                     /* zero-terminate the string */
            str[3] = '0' + (score % 10); score /= 10;
            str[2] = '0' + (score % 10); score /= 10;
            str[1] = '0' + (score % 10); score /= 10;
            str[0] = '0' + (score % 10);
            BSP_drawNString((GAME_SCREEN_WIDTH - 6*10)/2 + 6*6, 1, str);

            return Q_TRAN(&Tunnel_game_over);
        }
    }
    return Q_SUPER(&Tunnel_active);
}
/*..........................................................................*/
/* A random-pixel screen saver to avoid damage to the display */
QState Tunnel_screen_saver(Tunnel *me) {
    switch (Q_SIG(me)) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Tunnel_screen_saver_hide);
        }
        case PLAYER_TRIGGER_SIG: {
            return Q_TRAN(&Tunnel_demo);
        }
    }
    return Q_SUPER(&Tunnel_active);
}
/*..........................................................................*/
QState Tunnel_screen_saver_hide(Tunnel *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_displayOff();                     /* power down the display */
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC*3);  /* 3s timeout */
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm((QActive *)me);
            BSP_displayOn();                        /* power up the display */
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Tunnel_screen_saver_show);
        }
    }
    return Q_SUPER(&Tunnel_screen_saver);
}
/*..........................................................................*/
QState Tunnel_screen_saver_show(Tunnel *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            uint32_t rnd = random();
            /* clear the screen frame buffer */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
            Tunnel_addImageAt(me,
                    PRESS_BUTTON_BMP,
                    (uint8_t)(rnd % (GAME_SCREEN_WIDTH - 55)),
                    (int8_t) (rnd % (GAME_SCREEN_HEIGHT - 8)));
            BSP_drawBitmap(l_frame);
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/3);/* 1/3s timeout */
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QActive_disarm((QActive *)me);
            /* clear the screen frame buffer */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
            BSP_drawBitmap(l_frame);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Tunnel_screen_saver_hide);
        }
    }
    return Q_SUPER(&Tunnel_screen_saver);
}

/* helper functions --------------------------------------------------------*/
/*
* The bitmap for the "Press Button" text:
*
*     xxx.........................xxx........x...x...........
*     x..x........................x..x.......x...x...........
*     x..x.x.xx..xx...xxx..xxx....x..x.x..x.xxx.xxx..xx..xxx.
*     xxx..xx...x..x.x....x.......xxx..x..x..x...x..x..x.x..x
*     x....x....xxxx..xx...xx.....x..x.x..x..x...x..x..x.x..x
*     x....x....x.......x....x....x..x.x..x..x...x..x..x.x..x
*     x....x.....xxx.xxx..xxx.....xxx...xxx...x...x..xx..x..x
*     .......................................................
*/
static uint8_t const press_button_bits[] = {
    0x7F, 0x09, 0x09, 0x06, 0x00, 0x7C, 0x08, 0x04, 0x04, 0x00,
    0x38, 0x54, 0x54, 0x58, 0x00, 0x48, 0x54, 0x54, 0x24, 0x00,
    0x48, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x49,
    0x49, 0x36, 0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x04, 0x3F,
    0x44, 0x00, 0x04, 0x3F, 0x44, 0x00, 0x38, 0x44, 0x44, 0x38,
    0x00, 0x7C, 0x04, 0x04, 0x78
};

/* bitmap of the Ship:
*
*     x....
*     xxx..
*     xxxxx
*/
static uint8_t const ship_bits[] = {
    0x07, 0x06, 0x06, 0x04, 0x04
};

/* bitmap of the Missile:
*
*     xxx
*/
static uint8_t const missile_bits[] = {
    0x01, 0x01, 0x01
};

/* bitmap of the Mine type-1:
*
*     .x.
*     xxx
*     .x.
*/
static uint8_t const mine1_bits[] = {
    0x02, 0x07, 0x02
};

/* bitmap of the Mine type-2:
*
*     x..x
*     .xx.
*     .xx.
*     x..x
*/
static uint8_t const mine2_bits[] = {
    0x09, 0x06, 0x06, 0x09
};

/* Mine type-2 is nastier than Mine type-1. The type-2 mine can
* hit the Ship with any of its "tentacles". However, it can be
* destroyed by the Missile only by hitting its center, defined as
* the following bitmap:
*
*     ....
*     .xx.
*     .xx.
*     ....
*/
static uint8_t const mine2_missile_bits[] = {
    0x00, 0x06, 0x06, 0x00
};

/*
* The bitmap of the explosion stage 0:
*
*     .......
*     .......
*     ...x...
*     ..x.x..
*     ...x...
*     .......
*     .......
*/
static uint8_t const explosion0_bits[] = {
    0x00, 0x00, 0x08, 0x14, 0x08, 0x00, 0x00
};

/*
* The bitmap of the explosion stage 1:
*
*     .......
*     .......
*     ..x.x..
*     ...x...
*     ..x.x..
*     .......
*     .......
*/
static uint8_t const explosion1_bits[] = {
    0x00, 0x00, 0x14, 0x08, 0x14, 0x00, 0x00
};

/*
* The bitmap of the explosion stage 2:
*
*     .......
*     .x...x.
*     ..x.x..
*     ...x...
*     ..x.x..
*     .x...x.
*     .......
*/
static uint8_t const explosion2_bits[] = {
    0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00
};

/*
* The bitmap of the explosion stage 3:
*
*     x..x..x
*     .x.x.x.
*     ..x.x..
*     xx.x.xx
*     ..x.x..
*     .x.x.x.
*     x..x..x
*/
static uint8_t const explosion3_bits[] = {
    0x49, 0x2A, 0x14, 0x6B, 0x14, 0x2A, 0x49
};

typedef struct BitmapTag { /* the auxiliary structure to hold const bitmaps */
    uint8_t const *bits;                          /* the bits in the bitmap */
    uint8_t width;                               /* the width of the bitmap */
} Bitmap;

static Bitmap const l_bitmap[MAX_BMP] = {
    { press_button_bits,  Q_DIM(press_button_bits)  },
    { ship_bits,          Q_DIM(ship_bits)          },
    { missile_bits,       Q_DIM(missile_bits)       },
    { mine1_bits,         Q_DIM(mine1_bits)         },
    { mine2_bits,         Q_DIM(mine2_bits)         },
    { mine2_missile_bits, Q_DIM(mine2_missile_bits) },
    { explosion0_bits,    Q_DIM(explosion0_bits)    },
    { explosion1_bits,    Q_DIM(explosion1_bits)    },
    { explosion2_bits,    Q_DIM(explosion2_bits)    },
    { explosion3_bits,    Q_DIM(explosion3_bits)    }
};

/*..........................................................................*/
uint32_t random(void) {      /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}
/*..........................................................................*/
void randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void Tunnel_advance(Tunnel *me) {
    uint32_t rnd;
    uint32_t bmp1;              /* bimap representing 1 column of the image */

    rnd = (random() & 0xFF);

    /* reduce the top wall thickness 18.75% of the time */
    if ((rnd < 48) && (me->wall_thickness_top > 0)) {
        --me->wall_thickness_top;
    }

    /* reduce the bottom wall thickness 18.75% of the time */
    if ((rnd > 208) && (me->wall_thickness_bottom > 0)) {
        --me->wall_thickness_bottom;
    }

    rnd = (random() & 0xFF);

    /* grow the top wall thickness 18.75% of the time */
    if ((rnd < 48)
        && ((GAME_SCREEN_HEIGHT
             - me->wall_thickness_top
             - me->wall_thickness_bottom) > me->minimal_gap)
        && ((me->last_mine_x < (GAME_SCREEN_WIDTH - 5))
            || (me->last_mine_y > (me->wall_thickness_top + 1))))
    {
        ++me->wall_thickness_top;
    }

    /* grow the bottom wall thickness 18.75% of the time */
    if ((rnd > 208)
        && ((GAME_SCREEN_HEIGHT
             - me->wall_thickness_top
             - me->wall_thickness_bottom) > me->minimal_gap)
        && ((me->last_mine_x < (GAME_SCREEN_WIDTH - 5))
             || (me->last_mine_y + 1
                < (GAME_SCREEN_HEIGHT - me->wall_thickness_bottom))))
    {
        ++me->wall_thickness_bottom;
    }

    /* advance the Tunnel by 1 game step to the left */
    memmove(l_walls, l_walls + GAME_SPEED_X,
            (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8) - GAME_SPEED_X);

    bmp1 = (~(~0 << me->wall_thickness_top))
            | (~0 << (GAME_SCREEN_HEIGHT
                        - me->wall_thickness_bottom));

    l_walls[GAME_SCREEN_WIDTH - 1] = (uint8_t)bmp1;
    l_walls[GAME_SCREEN_WIDTH + GAME_SCREEN_WIDTH - 1]
          = (uint8_t)(bmp1 >> 8);

    /* copy the Tunnel layer to the main frame buffer */
    memcpy(l_frame, l_walls, (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
}
/*..........................................................................*/
void Tunnel_plantMine(Tunnel *me) {
    uint32_t rnd = (random() & 0xFF);

    if (me->last_mine_x > 0) {
        --me->last_mine_x;    /* shift the last Mine 1 position to the left */
    }
                                                   /* last mine far enough? */
    if ((me->last_mine_x + GAME_MINES_DIST_MIN < GAME_SCREEN_WIDTH)
        && (rnd < 8))                /* place the mines only 5% of the time */
    {
        uint8_t n;
        for (n = 0; n < Q_DIM(me->mines); ++n) { /* look for disabled mines */
            if (me->mines[n] == (QHsm *)0) {
                break;
            }
        }
        if (n < Q_DIM(me->mines)) {               /* a disabled Mine found? */
            rnd = (random() & 0xFFFF);

            if ((rnd & 1) == 0) {            /* choose the type of the mine */
                me->mines[n] = me->mine1_pool[n];
            }
            else {
                me->mines[n] = me->mine2_pool[n];
            }

            /* new Mine is planted in the last column of the tunnel */
            me->last_mine_x = GAME_SCREEN_WIDTH;

            /* choose a random y-position for the Mine in the Tunnel */
            rnd %= (GAME_SCREEN_HEIGHT
                    - me->wall_thickness_top
                    - me->wall_thickness_bottom - 4);
            me->last_mine_y = (uint8_t)(me->wall_thickness_top + 2U + rnd);

            Q_SIG(me->mines[n]) = MINE_PLANT_SIG;
            Q_PAR(me->mines[n]) = (me->last_mine_x | (me->last_mine_y << 8));
            QHsm_dispatch(me->mines[n]);                 /* direct dispatch */
        }
    }
}
/*..........................................................................*/
void Tunnel_dispatchToAllMines(Tunnel *me, QSignal sig, QParam par) {
    uint8_t n;

    for (n = 0; n < GAME_MINES_MAX; ++n) {
        if (me->mines[n] != (QHsm *)0) {               /* is the mine used? */
            Q_SIG(me->mines[n]) = sig;
            Q_PAR(me->mines[n]) = par;
            QHsm_dispatch(me->mines[n]);
        }
    }
    (void)me;                                 /* avoid the compiler warning */
}
/*..........................................................................*/
void Tunnel_addImageAt(Tunnel *me, uint8_t bmp,
                       uint8_t x_pos, int8_t y_pos)
{
    uint8_t x;                                  /* the x-index of the image */
    uint8_t w;                                    /* the width of the image */

    Q_REQUIRE(bmp < Q_DIM(l_bitmap));

    w = l_bitmap[bmp].width;
    if (w > GAME_SCREEN_WIDTH - x_pos) {
        w = GAME_SCREEN_WIDTH - x_pos;
    }
    for (x = 0; x < w; ++x) {
        uint32_t bmp1;
        if (y_pos >= 0) {
            bmp1 = (l_bitmap[bmp].bits[x] << (uint8_t)y_pos);
        }
        else {
            bmp1 = (l_bitmap[bmp].bits[x] >> (uint8_t)(-y_pos));
        }
        l_frame[x_pos + x] |= (uint8_t)bmp1;
        l_frame[x_pos + x + GAME_SCREEN_WIDTH] |= (uint8_t)(bmp1 >> 8);
    }
    (void)me;                                 /* avoid the compiler warning */
}
/*..........................................................................*/
uint8_t Tunnel_isWallHit(Tunnel *me, uint8_t bmp,
                                uint8_t x_pos, uint8_t y_pos)
{
    uint8_t x;
    uint8_t w;                                    /* the width of the image */

    Q_REQUIRE(bmp < Q_DIM(l_bitmap));

    w = l_bitmap[bmp].width;
    if (w > GAME_SCREEN_WIDTH - x_pos) {
        w = GAME_SCREEN_WIDTH - x_pos;
    }
    for (x = 0; x < w; ++x) {
        uint32_t bmp1 = ((uint32_t)l_bitmap[bmp].bits[x] << y_pos);
        if (((l_walls[x_pos + x] & (uint8_t)bmp1) != 0)
            || ((l_walls[x_pos + x + GAME_SCREEN_WIDTH]
                 & (uint8_t)(bmp1 >> 8)) != 0))
        {
            return (uint8_t)1;
        }
    }
    (void)me;                                 /* avoid the compiler warning */
    return (uint8_t)0;
}
/*..........................................................................*/
uint8_t do_bitmaps_overlap(uint8_t bmp_id1, uint8_t x1, uint8_t y1,
                           uint8_t bmp_id2, uint8_t x2, uint8_t y2)
{
    uint8_t x;
    uint8_t x0;
    uint8_t w;
    uint32_t bits1;
    uint32_t bits2;
    Bitmap const *bmp1;
    Bitmap const *bmp2;

    Q_REQUIRE((bmp_id1 < Q_DIM(l_bitmap)) && (bmp_id2 < Q_DIM(l_bitmap)));

    bmp1 = &l_bitmap[bmp_id1];
    bmp2 = &l_bitmap[bmp_id2];

             /* is the incoming object starting to overlap the Mine bitmap? */
    if ((x1 <= x2) && (x1 + bmp2->width > x2)) {
        x0 = x2 - x1;
        w  = x1 + bmp2->width - x2;
        if (w > bmp1->width) {
            w = bmp1->width;
        }
        for (x = 0; x < w; ++x) {      /* scan over the overlapping columns */
            bits1 = ((uint32_t)bmp2->bits[x + x0] << y2);
            bits2 = ((uint32_t)bmp1->bits[x] << y1);
            if ((bits1 & bits2) != 0) {             /* do the bits overlap? */
                return (uint8_t)1;                                  /* yes! */
            }
        }
    }
    else {
        if ((x1 > x2) && (x2 + bmp1->width > x1)) {
            x0 = x1 - x2;
            w  = x2 + bmp1->width - x1;
            if (w > bmp2->width) {
                w = bmp2->width;
            }
            for (x = 0; x < w; ++x) {  /* scan over the overlapping columns */
                bits1 = ((uint32_t)bmp1->bits[x + x0] << y1);
                bits2 = ((uint32_t)bmp2->bits[x] << y2);
                if ((bits1 & bits2) != 0) {         /* do the bits overlap? */
                    return (uint8_t)1;                              /* yes! */
                }
            }
        }
    }
    return (uint8_t)0;                        /* the bitmaps do not overlap */
}


/*****************************************************************************
* Product: User Interface with State-Local Storage Example
* Last Updated for Version: 4.0.02
* Date of the Last Update:  Sep 24, 2008
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
#include "qpn_port.h"                  /* the port of the QP-nano framework */
#include "num_ent.h"
#include "ui.h"
#include "video.h"

#include <stdio.h>
#include <math.h>


typedef struct UI_num_lr_xyTag {
    UI_num_lr super;                               /* derive from UI_num_lr */
} UI_num_lr_xy;
Q_ASSERT_COMPILE(sizeof(UI_num_lr_xy) < sizeof(UI_mem));

static QState UI_num_lr_xy_x_entry(UI_num_lr_xy *me);
static QState UI_num_lr_xy_y_entry(UI_num_lr_xy *me);


/* Local objects -----------------------------------------------------------*/
#define NUM_ENTRY1_X     3
#define NUM_ENTRY1_Y     13

#define NUM_ENTRY2_X     (NUM_ENTRY1_X + NUM_STR_WIDTH + 4)
#define NUM_ENTRY2_Y     13

#define NUM_ENTRY_COLOR  (VIDEO_FGND_YELLOW)

/*..........................................................................*/
QState UI_num_lr_handler(UI_num_lr *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            uint8_t c;
                                     /* instantiate the state-local objects */
            me->n       = 0.0;
            me->xsum    = 0.0;
            me->xsum_sq = 0.0;
            me->ysum    = 0.0;
            me->ysum_sq = 0.0;
            me->xysum   = 0.0;

            Video_printStrAt(2, 10, VIDEO_FGND_BLACK,
                "Screen 2: Linear Regression      ");
            Video_clearRect( 0, 11, 35, 23, VIDEO_BGND_BLUE);
            Video_clearRect(35, 11, 80, 23, VIDEO_BGND_BLACK);

            c = VIDEO_FGND_LIGHT_GRAY;
            Video_printStrAt(36, 12, c,
                "Press '-'        to enter a negative number");
            Video_printStrAt(36, 13, c,
                "Press '0' .. '9' to enter a digit");
            Video_printStrAt(36, 14, c,
                "Press '.'        to enter the decimal point");
            Video_printStrAt(36, 15, c,
                "Press <Enter>    to move from x to y");
            Video_printStrAt(43, 16, c,
                       "and again to enter the (x, y) sample");
            Video_printStrAt(36, 17, c,
                "Press 'e' or 'E' to Cancel last entry");
            Video_printStrAt(36, 18, c,
                "Press 'c' or 'C' to Cancel the data set");

            c = VIDEO_FGND_WHITE;
            Video_printStrAt(36, 20, c,
                "Press UP-arrow   for previous screen");
            Video_printStrAt(36, 21, c,
                "Press DOWN-arrow for next screen");
            Video_printStrAt(36, 22, c,
                "Press F1         for help");

            Video_clearRect(NUM_ENTRY1_X, NUM_ENTRY1_Y,
                NUM_ENTRY1_X + NUM_STR_WIDTH, NUM_ENTRY1_Y + 1,
                VIDEO_BGND_BLACK);

            Video_clearRect(NUM_ENTRY2_X, NUM_ENTRY2_Y,
                NUM_ENTRY2_X + NUM_STR_WIDTH, NUM_ENTRY2_Y + 1,
                VIDEO_BGND_BLACK);


            NumEntry_config(&me->super.num_entry, NUM_ENTRY1_X, NUM_ENTRY1_Y,
                NUM_ENTRY_COLOR);
            Q_SIG(&me->super.num_entry) = C_SIG;
            QHsm_dispatch((QHsm *)&me->super.num_entry);

            NumEntry_config(&me->super.num_entry, NUM_ENTRY2_X, NUM_ENTRY2_Y,
                NUM_ENTRY_COLOR);
            Q_SIG(&me->super.num_entry) = C_SIG;
            QHsm_dispatch((QHsm *)&me->super.num_entry);

            c = VIDEO_FGND_WHITE;                                 /* labels */
            Video_printStrAt(NUM_ENTRY1_X - 2, NUM_ENTRY1_Y, c, "x");
            Video_printStrAt(NUM_ENTRY2_X - 2, NUM_ENTRY2_Y, c, "y");

            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH, NUM_ENTRY1_Y + 3,
                c, "y = a*x + b");

            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH, NUM_ENTRY1_Y + 5,
                c, "n =");

            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH, NUM_ENTRY1_Y + 6,
                c, "a =");
            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH, NUM_ENTRY1_Y + 7,
                c, "b =");

            c = VIDEO_FGND_YELLOW;                                /* values */
            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                NUM_ENTRY1_Y + 5, c, "0           ");
            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                NUM_ENTRY1_Y + 6, c, "N/A         ");
            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                NUM_ENTRY1_Y + 7, c, "N/A         ");

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
                                             /* store this state in history */
            me->super.super.history = (QStateHandler)&UI_num_lr_handler;
                                      /* destroy the state-local objects... */
            /* noting to destroy */
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&UI_num_lr_xy_x_entry);
        }
        case NEG_SIG:
        case DIGIT_0_SIG:
        case DIGIT_1_9_SIG:
        case POINT_SIG: {
                                             /* dispatch to the number comp */
            Q_SIG(&me->super.num_entry) = Q_SIG(me);
            Q_PAR(&me->super.num_entry) = Q_PAR(me);
            QHsm_dispatch((QHsm *)&me->super.num_entry);
            return Q_HANDLED();
        }
        case C_SIG: {
            return Q_TRAN(&UI_num_lr_handler);        /* transition-to-self */
        }
        case UP_SIG: {
            return Q_TRAN(&UI_num_sd_handler); /* Standard Deviation screen */
        }
        case DOWN_SIG: {
            return Q_TRAN(&UI_num_sd_handler); /* Standard Deviation screen */
        }
    }
    return Q_SUPER(&UI_num_handler);
}
/*..........................................................................*/
QState UI_num_lr_xy_x_entry(UI_num_lr_xy *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Video_clearRect(NUM_ENTRY1_X, NUM_ENTRY1_Y,
                NUM_ENTRY1_X + NUM_STR_WIDTH, NUM_ENTRY1_Y + 1,
                VIDEO_BGND_BLACK);
                                             /* indicate the keyboard focus */
            Video_drawRect(NUM_ENTRY1_X - 1, NUM_ENTRY1_Y - 1,
                NUM_ENTRY1_X + NUM_STR_WIDTH + 1, NUM_ENTRY1_Y + 2,
                VIDEO_FGND_WHITE, 2);

            NumEntry_config(&me->super.super.num_entry,
                NUM_ENTRY1_X, NUM_ENTRY1_Y, NUM_ENTRY_COLOR);
            Q_SIG(&me->super.super.num_entry) = C_SIG;
            QHsm_dispatch((QHsm *)&me->super.super.num_entry);

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            me->super.x = NumEntry_get(&me->super.super.num_entry);
                                               /* remove the keyboard focus */
            Video_drawRect(NUM_ENTRY1_X - 1, NUM_ENTRY1_Y - 1,
                NUM_ENTRY1_X + NUM_STR_WIDTH + 1, NUM_ENTRY1_Y + 2,
                VIDEO_FGND_WHITE, 0);
            return Q_HANDLED();
        }
        case ENTER_SIG: {
            return Q_TRAN(&UI_num_lr_xy_y_entry);
        }
    }
    return Q_SUPER(&UI_num_lr_handler);
}
/*..........................................................................*/
QState UI_num_lr_xy_y_entry(UI_num_lr_xy *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Video_clearRect(NUM_ENTRY2_X, NUM_ENTRY2_Y,
                NUM_ENTRY2_X + NUM_STR_WIDTH, NUM_ENTRY2_Y + 1,
                VIDEO_BGND_BLACK);
                                             /* indicate the keyboard focus */
            Video_drawRect(NUM_ENTRY2_X - 1, NUM_ENTRY2_Y - 1,
                NUM_ENTRY2_X + NUM_STR_WIDTH + 1, NUM_ENTRY2_Y + 2,
                VIDEO_FGND_WHITE, 2);

            NumEntry_config(&me->super.super.num_entry,
                NUM_ENTRY2_X, NUM_ENTRY2_Y, NUM_ENTRY_COLOR);
            Q_SIG(&me->super.super.num_entry) = C_SIG;
            QHsm_dispatch((QHsm *)&me->super.super.num_entry);

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            Q_SIG(&me->super.super.num_entry) = C_SIG;
            QHsm_dispatch((QHsm *)&me->super.super.num_entry);
                                               /* remove the keyboard focus */
            Video_drawRect(NUM_ENTRY2_X - 1, NUM_ENTRY2_Y - 1,
                NUM_ENTRY2_X + NUM_STR_WIDTH + 1, NUM_ENTRY2_Y + 2,
                VIDEO_FGND_WHITE, 0);
            return Q_HANDLED();
        }
        case CE_SIG: {
            return Q_TRAN(&UI_num_lr_xy_x_entry);
        }
        case ENTER_SIG: {
            double tmp = NumEntry_get(&me->super.super.num_entry);     /* y */
            char   buf[14];

            me->super.n       += 1.0;
            me->super.xsum    += me->super.x;
            me->super.ysum    += tmp;
            me->super.xsum_sq += me->super.x * me->super.x;
            me->super.ysum_sq += tmp * tmp;
            me->super.xysum   += me->super.x * tmp;

            sprintf(buf, "%-12.6g", me->super.n);                      /* n */
            Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                NUM_ENTRY1_Y + 5, VIDEO_FGND_YELLOW, buf);

            tmp = me->super.xsum_sq - me->super.xsum * me->super.xsum
                  / me->super.n;
            if ((tmp < -0.0000001) || (0.0000001 < tmp)) {
                double a = (me->super.xysum - me->super.xsum * me->super.ysum
                           / me->super.n) / tmp;
                double b = (me->super.ysum - a * me->super.xsum)
                           / me->super.n;
                sprintf(buf, "%-12.6g", a);
                Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                    NUM_ENTRY1_Y + 6, VIDEO_FGND_YELLOW, buf);
                sprintf(buf, "%-12.6g", b);
                Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                    NUM_ENTRY1_Y + 7, VIDEO_FGND_YELLOW, buf);
            }
            else {
                Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                    NUM_ENTRY1_Y + 6, VIDEO_FGND_YELLOW,  "N/A         ");
                Video_printStrAt(NUM_ENTRY1_X + NUM_STR_WIDTH + 5,
                    NUM_ENTRY1_Y + 7, VIDEO_FGND_YELLOW,  "N/A         ");
            }

            return Q_TRAN(&UI_num_lr_xy_x_entry);
        }
    }
    return Q_SUPER(&UI_num_lr_handler);
}

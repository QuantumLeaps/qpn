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

#define HELP_X      3
#define HELP_Y      14
#define HELP_DX     28
#define HELP_DY     5

/* Local objects -----------------------------------------------------------*/
static char const * const l_help_sd[] = {
    "Standard Deviation:         ",
    "Find the mean value and the ",
    " root-mean-square (RMS)     ",
    "deviation of n data samples ",
    "xi, where i = 1..n.         ",
    "Mean value <x> is calculated",
    "as follows:                 ",
    "<x> = Sum(xi)/n;            ",
    "Two RMS estimatators are    ",
    "provided:                   ",
    "sig(n) =                    ",
    "   sqrt(Sum(xi-<x>)**2 / n);",
    "sig(n-1) =                  ",
    "sqrt(Sum(xi-<x>)**2 / (n-1))"
};
static char const * const l_help_lr[] = {
    "Linear Regression:          ",
    "Fit the line y = a*x + b,   ",
    "based on n samples (xi, yi).",
    "The coefficients a and b are",
    "calculated as follows:      ",
    "a = Sxy / Sxx =             ",
    " (Sum(xi*yi)                ",
    "  -Sum(xi)*Sum(yi)/n)       ",
    " /(Sum(xi*xi)-(Sum(xi)**2/n;",
    "b = yave - a*xave =         ",
    " Sum(yi)/n - a*Sum(xi)/n;   "
};
static char const * const l_help_unknown[] = {
    "Unknown Screen Help:        ",
    "                            ",
    "                            ",
    "                            ",
    "                            "
};

static void printHelp(char const * const *txt) {
    uint8_t y;
    for (y = 0; y < HELP_DY; ++y) {
        Video_printStrAt(HELP_X, HELP_Y + y, VIDEO_FGND_YELLOW, txt[y]);
    }
}

/*..........................................................................*/
QState UI_help_handler(UI_help *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
                                     /* instantiate the state-local objects */
            Video_printStrAt(2, 10, VIDEO_FGND_BLACK,
                "Screen 0: Help                   ");
            Video_clearRect( 0, 11, 35, 23, VIDEO_BGND_BLUE);
            Video_clearRect(35, 11, 80, 23, VIDEO_BGND_BLACK);

            Video_printStrAt(36, 12, VIDEO_FGND_LIGHT_GRAY,
                "Press DOWN-Arrow to scroll down");
            Video_printStrAt(36, 13, VIDEO_FGND_LIGHT_GRAY,
                "Press UP-Arrow   to scroll up");

            Video_printStrAt(36, 20, VIDEO_FGND_WHITE,
                "Press F1         to return to last screen");

            Video_clearRect(HELP_X - 1, HELP_Y,
                HELP_X + HELP_DX + 1, HELP_Y + HELP_DY, VIDEO_BGND_BLACK);
            Video_drawRect (HELP_X - 2, HELP_Y - 1,
                HELP_X + HELP_DX + 2, HELP_Y + HELP_DY + 1,
                VIDEO_FGND_WHITE,2);

            if (me->super.history == (QStateHandler)&UI_num_sd_handler) {
                me->help_text = l_help_sd;
                me->help_len  = Q_DIM(l_help_sd);
            }
            else if (me->super.history == (QStateHandler)&UI_num_lr_handler) {
                me->help_text = l_help_lr;
                me->help_len  = Q_DIM(l_help_lr);
            }
            else {
                me->help_text = l_help_unknown;
                me->help_len  = Q_DIM(l_help_unknown);
            }
            me->help_line = 0;
            printHelp(me->help_text + me->help_line);

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
                                      /* destroy the state-local objects... */
            return Q_HANDLED();
        }
        case DOWN_SIG: {
            if (me->help_line + HELP_DY < me->help_len) {
                ++me->help_line;
            }
            printHelp(me->help_text + me->help_line);
            return Q_HANDLED();
        }
        case UP_SIG: {
            if (me->help_line > 0) {
                --me->help_line;
            }
            printHelp(me->help_text + me->help_line);
            return Q_HANDLED();
        }
        case HELP_SIG: {
            return Q_TRAN(me->super.history); /* go back to the last screen */
        }
    }
    return Q_SUPER(&UI_top_handler);
}

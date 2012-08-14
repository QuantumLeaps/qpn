/*****************************************************************************
* Product: User interface with State-Local Storage Example
* Last Updated for Version: 4.1.01
* Date of the Last Update:  Nov 05, 2009
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
#include "num_ent.h"
#include "ui.h"
#include "video.h"


/* Global objects ----------------------------------------------------------*/
UI_mem const AO_UI;                                            /* the UI AO */

void UI_ctor(void) {
    UI_top_ctor((UI_top *)&AO_UI);
}
/*..........................................................................*/
void UI_top_ctor(UI_top *me) {
    QActive_ctor(&me->super, (QStateHandler)&UI_top_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState UI_top_initial(UI_top *me) {
    me->history = (QStateHandler)&UI_num_sd_handler;  /* initialize history */
    return Q_TRAN(&UI_num_handler);
}
/*..........................................................................*/
QState UI_top_handler(UI_top *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            uint8_t c;
            Video_clearScreen(VIDEO_BGND_BLACK);
            Video_clearRect( 0,  0, 80,  7, VIDEO_BGND_LIGHT_GRAY);
            Video_clearRect( 0, 10, 80, 11, VIDEO_BGND_LIGHT_GRAY);
            Video_clearRect( 0, 23, 80, 24, VIDEO_BGND_LIGHT_GRAY);

            c = VIDEO_FGND_BLUE;
            Video_printStrAt(10, 0, c, "  __");
            Video_printStrAt(10, 1, c, " /  |      _   _ -|-     _ _");
            Video_printStrAt(10, 2, c, " \\__| | |  _\\ | \\ | | | | \\ \\");
            Video_printStrAt(10, 3, c, "    | \\_/ |_| | | | \\_| | | |");
            Video_printStrAt(10, 4, c, "    |");
            c = VIDEO_FGND_RED;
            Video_printStrAt(43, 0, c, "    _       __ ");
            Video_printStrAt(43, 1, c, "|  /_\\     |  \\  TM");
            Video_printStrAt(43, 2, c, "|  \\_   _  |__/ _");
            Video_printStrAt(43, 3, c, "|       _\\ |   |_");
            Video_printStrAt(43, 4, c, "|___   |_| |    _|");
            Video_printStrAt(10, 5, VIDEO_FGND_BLUE,
                "_____________________________________________________");
            Video_printStrAt(10, 6, VIDEO_FGND_RED,
                "i n n o v a t i n g   e m b e d d e d   s y s t e m s");
            Video_printStrAt(2,  8, VIDEO_FGND_WHITE,
                "State-Local Storage Example");
            Video_printStrAt(36,  8, VIDEO_FGND_WHITE, "QP-nano");
            Video_printStrAt(45,  8, VIDEO_FGND_YELLOW, QP_getVersion());

            Video_printStrAt(10, 23, VIDEO_FGND_BLUE,
              "* Copyright (c) Quantum Leaps, LLC * www.quantum-leaps.com *");
            Video_printStrAt(28, 24, VIDEO_FGND_LIGHT_RED,
                "<< Press Esc to quit >>");

            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            Video_clearScreen(VIDEO_BGND_BLACK);     /* clear the screen... */
            return Q_HANDLED();
        }
        case QUIT_SIG: {
            return Q_TRAN(&UI_top_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState UI_top_final(UI_top *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QF_stop();                                  /* cleanup and exit */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}


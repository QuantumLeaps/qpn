/*****************************************************************************
* Product: User Interface with State-Local Storage Example
* Last Updated for Version: 4.0.02
* Date of the Last Update:  Sep 22, 2008
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
#ifndef ui_h
#define ui_h

enum UserSignals {
    QUIT_SIG = Q_USER_SIG,
    C_SIG,
    CE_SIG,
    DIGIT_0_SIG,
    DIGIT_1_9_SIG,
    POINT_SIG,
    NEG_SIG,
    ENTER_SIG,
    UP_SIG,
    DOWN_SIG,
    HELP_SIG,

    MAX_SIG                           /* the last signal (keep always last) */
};

/*..........................................................................*/
typedef struct UI_topTag {
    QActive super;                                   /* derive from QActive */
    QStateHandler history;
} UI_top;

void   UI_top_ctor   (UI_top *me);
QState UI_top_initial(UI_top *me);                   /* initial pseudostate */
QState UI_top_handler(UI_top *me);                         /* state handler */
QState UI_top_final  (UI_top *me);                         /* state handler */

typedef struct UI_memTag {
    UI_top super;                                     /* derive from UI_top */
    uint8_t mem[100];  /* maximum size of any substate (subclass) of UI_top */
} UI_mem;

/*..........................................................................*/
typedef struct UI_numTag {
    UI_top super;                                     /* derive from UI_top */
    NumEntry num_entry;
} UI_num;

QState UI_num_handler(UI_num *me);
Q_ASSERT_COMPILE(sizeof(UI_num) < sizeof(UI_mem));

/*..........................................................................*/
typedef struct UI_num_sdTag {                         /* standard deviation */
    UI_num super;                                     /* derive from UI_num */
    double n;
    double sum;
    double sum_sq;
} UI_num_sd;

QState UI_num_sd_handler(UI_num_sd *me);
Q_ASSERT_COMPILE(sizeof(UI_num_sd) < sizeof(UI_mem));

/*..........................................................................*/
typedef struct UI_num_lrTag {                          /* linear regression */
    UI_num super;                                     /* derive from UI_num */
    double x;
    double n;
    double xsum;
    double xsum_sq;
    double ysum;
    double ysum_sq;
    double xysum;
} UI_num_lr;

QState UI_num_lr_handler(UI_num_lr *me);
Q_ASSERT_COMPILE(sizeof(UI_num_lr) < sizeof(UI_mem));

/*..........................................................................*/
typedef struct UI_helpTag {
    UI_top super;                                     /* derive from UI_top */
    char const * const *help_text;
    uint16_t help_len;
    uint16_t help_line;
} UI_help;

QState UI_help_handler(UI_help *me);
Q_ASSERT_COMPILE(sizeof(UI_help) < sizeof(UI_mem));

/*--------------------------------------------------------------------------*/
extern UI_mem const AO_UI;                              /* UI Active Object */

void UI_ctor(void);                 /* global ctor for the UI active object */

#endif

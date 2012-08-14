/*****************************************************************************
* Product: Number data entry
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
#include <string.h>
#include <stdlib.h>

static QState NumEntry_initial (NumEntry *me);
static QState NumEntry_top     (NumEntry *me);
static QState NumEntry_negative(NumEntry *me);
static QState NumEntry_zero    (NumEntry *me);
static QState NumEntry_integer (NumEntry *me);
static QState NumEntry_fraction(NumEntry *me);

/*..........................................................................*/
void NumEntry_ctor(NumEntry *me) {                                  /* ctor */
    QHsm_ctor(&me->super, (QStateHandler)&NumEntry_initial);
}
/*..........................................................................*/
void NumEntry_xtor(NumEntry *me) {                                  /* xtor */
    (void)me;          /* avoid the compiler warning about unused parameter */
}
/*..........................................................................*/
void NumEntry_config(NumEntry *me, uint8_t x, uint8_t y, uint8_t color) {
    me->x     = x;
    me->y     = y;
    me->color = color;
}
/*..........................................................................*/
void NumEntry_insert(NumEntry *me, uint8_t keyId) {
    if (me->len == 0) {
        me->str[NUM_STR_WIDTH - 1] = (char)keyId;
    }
    else if (me->len < (NUM_STR_WIDTH - 1)) {
        memmove(&me->str[0], &me->str[1], NUM_STR_WIDTH - 1);
        me->str[NUM_STR_WIDTH - 1] = (char)keyId;
    }
    ++me->len;
    Video_printStrAt(me->x, me->y, me->color, me->str);
}
/*..........................................................................*/
double NumEntry_get(NumEntry *me) {
    return strtod(me->str, (char **)0);
}

/* HSM definition ----------------------------------------------------------*/
QState NumEntry_initial(NumEntry *me) {
    return Q_TRAN(&NumEntry_zero);
}
/*..........................................................................*/
QState NumEntry_top(NumEntry *me) {
    switch (Q_SIG(me)) {
        case C_SIG: {
            memset(me->str, ' ', NUM_STR_WIDTH - 1);
            me->str[NUM_STR_WIDTH - 1] = '0';
            me->str[NUM_STR_WIDTH] = '\0';
            me->len = 0;
            Video_printStrAt(me->x, me->y, me->color, me->str);
            return Q_TRAN(&NumEntry_zero);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState NumEntry_zero(NumEntry *me) {
    switch (Q_SIG(me)) {
        case DIGIT_0_SIG: {
            ;                                          /* explicitly ignore */
            return Q_HANDLED();
        }
        case NEG_SIG: {
            me->str[NUM_STR_WIDTH - 2] = '-';
            Video_printStrAt(me->x, me->y, me->color, me->str);
            return Q_TRAN(&NumEntry_negative);
        }
        case DIGIT_1_9_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_TRAN(&NumEntry_integer);
        }
        case POINT_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_TRAN(&NumEntry_fraction);
        }
    }
    return Q_SUPER(&NumEntry_top);
}
/*..........................................................................*/
QState NumEntry_negative(NumEntry *me) {
    switch (Q_SIG(me)) {
        case DIGIT_0_SIG: {
            ;                                          /* explicitly ignore */
            return Q_HANDLED();
        }
        case DIGIT_1_9_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_TRAN(&NumEntry_integer);
        }
        case POINT_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_TRAN(&NumEntry_fraction);
        }
    }
    return Q_SUPER(&NumEntry_top);
}
/*..........................................................................*/
QState NumEntry_integer(NumEntry *me) {
    switch (Q_SIG(me)) {
        case DIGIT_0_SIG:                     /* intentionally fall through */
        case DIGIT_1_9_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_HANDLED();
        }
        case POINT_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_TRAN(&NumEntry_fraction);
        }
    }
    return Q_SUPER(&NumEntry_top);
}
/*..........................................................................*/
QState NumEntry_fraction(NumEntry *me) {
    switch (Q_SIG(me)) {
        case DIGIT_0_SIG:                     /* intentionally fall through */
        case DIGIT_1_9_SIG: {
            NumEntry_insert(me, Q_PAR(me));
            return Q_HANDLED();
        }
        case POINT_SIG: {
            ;                                          /* explicitly ignore */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&NumEntry_top);
}


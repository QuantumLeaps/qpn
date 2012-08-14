/*****************************************************************************
* Product: Time Bomb example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 05, 2008
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
#include "qpn_port.h"
#include "bsp.h"
#include "bomb.h"

/*--------------------------------------------------------------------------*/
typedef struct OperTag {
    QActive super;                                   /* derive from QActive */
    uint8_t bomb_min;                   /* the minimum timeout for the bomb */
    uint8_t bomb_max;                   /* the maximum timeout for the bomb */
    uint8_t bomb_prev;                 /* the previous timeout for the bomb */
    uint8_t interval;               /* the interval between arming the bomb */
} Oper;

static QState Oper_initial(Oper *me);
static QState Oper_run    (Oper *me);

/* Global objects ----------------------------------------------------------*/
Oper AO_Oper;              /* the single instance of the Oper active object */

/*..........................................................................*/
void Oper_ctor(uint8_t min, uint8_t max, uint8_t every) {
    Oper *me = &AO_Oper;

    QActive_ctor(&me->super, (QStateHandler)&Oper_initial);
    me->bomb_min  = min;
    me->bomb_max  = max;
    me->bomb_prev = min;
    me->interval  = every;
}
/*..........................................................................*/
QState Oper_initial(Oper *me) {
    return Q_TRAN(&Oper_run);
}
/*..........................................................................*/
QState Oper_run(Oper *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, me->interval);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            uint8_t timeout = me->bomb_prev + 1;
            if (timeout > me->bomb_max) {
                timeout = me->bomb_min;
            }
            me->bomb_prev = timeout;
            QActive_post((QActive *)&AO_Bomb, ARM_SIG, timeout);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Oper_run);                 /* transition to self */
        }
        case TERMINATE_SIG: {
            QF_stop();
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}

/*..........................................................................*/
void BSP_onKeyboardInputISR(uint8_t key) {
    if (key == 129) {                                       /* ESC pressed? */
        QActive_postISR((QActive *)&AO_Oper, TERMINATE_SIG, 0);
    }
}

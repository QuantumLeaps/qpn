/*****************************************************************************
* Product: PELICAN crossing example
* Last Updated for Version: 4.0.01
* Date of the Last Update:  May 03, 2008
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
#include "pelican.h"

/*--------------------------------------------------------------------------*/
typedef struct OperTag {
    QActive super;                                  /* derived from QActive */
} Oper;

QState Oper_initial       (Oper *me);
QState Oper_in_service    (Oper *me);
QState Oper_out_of_service(Oper *me);

static Oper l_oper;  /* the encapsulated instance of Operator active object */

/* global objects ----------------------------------------------------------*/
Oper AO_Oper;              /* the single instance of the Oper active object */

/*--------------------------------------------------------------------------*/
enum OperTimeouts {                            /* various timeouts in ticks */
    IN_SERVICE_TOUT      = BSP_TICKS_PER_SEC * 30,       /* in-service time */
    OUT_OF_SERVICE_TOUT  = BSP_TICKS_PER_SEC * 8,    /* out-of-service time */
};

/*..........................................................................*/
void Oper_ctor(void) {
    QActive_ctor((QActive *)&AO_Oper, (QStateHandler)&Oper_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Oper_initial(Oper *me) {
    return Q_TRAN(&Oper_in_service);
}
/*..........................................................................*/
QState Oper_in_service(Oper *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, IN_SERVICE_TOUT);
            QActive_post((QActive *)&AO_Pelican, ON_SIG, 0);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Oper_out_of_service);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Oper_out_of_service(Oper *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, OUT_OF_SERVICE_TOUT);
            QActive_post((QActive *)&AO_Pelican, OFF_SIG, 0);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Oper_in_service);
        }
    }
    return Q_SUPER(&QHsm_top);
}

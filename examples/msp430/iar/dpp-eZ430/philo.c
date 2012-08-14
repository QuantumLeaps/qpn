/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
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
#include "qpn_port.h"                                       /* QP-nano port */
#include "dpp.h"                                   /* application interface */
#include "bsp.h"                             /* Board Support Package (BSP) */

//Q_DEFINE_THIS_MODULE("philo")

/* Active object class -----------------------------------------------------*/
typedef struct PhiloTag {
    QActive super;
} Philo;

static QState Philo_initial (Philo *me);
static QState Philo_thinking(Philo *me);
static QState Philo_hungry  (Philo *me);
static QState Philo_eating  (Philo *me);

#define THINK_TIME  7
#define EAT_TIME    5

/* Global objects ----------------------------------------------------------*/
Philo AO_Philo0;
Philo AO_Philo1;
Philo AO_Philo2;
//Philo AO_Philo3;
//Philo AO_Philo4;

/*..........................................................................*/
void Philo_ctor(void) {                    /* instantiate all Philo objects */
    QActive_ctor(&AO_Philo0.super, (QStateHandler)&Philo_initial);
    QActive_ctor(&AO_Philo1.super, (QStateHandler)&Philo_initial);
    QActive_ctor(&AO_Philo2.super, (QStateHandler)&Philo_initial);
    //QActive_ctor(&AO_Philo3.super, (QStateHandler)&Philo_initial);
    //QActive_ctor(&AO_Philo4.super, (QStateHandler)&Philo_initial);
}
/*..........................................................................*/
QState Philo_initial(Philo *me) {
    return Q_TRAN(&Philo_thinking);
}
/*..........................................................................*/
QState Philo_thinking(Philo *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, THINK_TIME);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            BSP_busyDelay();
            return Q_TRAN(&Philo_hungry);
        }
//        case EAT_SIG:
//        case DONE_SIG: {
//            Q_ERROR();    /* these events should never arrive in this state */
//            return Q_HANDLED();
//        }
    }
    return Q_IGNORED();
}
/*..........................................................................*/
QState Philo_hungry(Philo *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_post((QActive *)&AO_Table, HUNGRY_SIG, me->super.prio);
            return Q_HANDLED();
        }
        case EAT_SIG: {
            BSP_busyDelay();
            return Q_TRAN(&Philo_eating);
        }
//        case DONE_SIG: {
//            Q_ERROR();      /* thes event should never arrive in this state */
//            return Q_HANDLED();
//        }
    }
    return Q_IGNORED();
}
/*..........................................................................*/
QState Philo_eating(Philo *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, EAT_TIME);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_busyDelay();
            QActive_post((QActive *)&AO_Table, DONE_SIG, me->super.prio);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Philo_thinking);
        }
//        case EAT_SIG:
//        case DONE_SIG: {
//            Q_ERROR();    /* these events should never arrive in this state */
//            return Q_HANDLED();
//        }
    }
    return Q_IGNORED();
}


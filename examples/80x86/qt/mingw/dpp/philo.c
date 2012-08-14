/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 29, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"                                       /* QP-nano port */
#include "dpp.h"                                   /* application interface */
#include "bsp.h"

Q_DEFINE_THIS_MODULE("philo")

/* Active object class -----------------------------------------------------*/
typedef struct PhiloTag {
    QActive super;
} Philo;

static QState Philo_initial (Philo *me);
static QState Philo_thinking(Philo *me);
static QState Philo_hungry  (Philo *me);
static QState Philo_eating  (Philo *me);

#define THINK_TIME  ((BSP_random() & 0x7FU) + 20U)
#define EAT_TIME    ((BSP_random() & 0x7FU) + 30U)

/* Global objects ----------------------------------------------------------*/
Philo AO_Philo0;
Philo AO_Philo1;
Philo AO_Philo2;
Philo AO_Philo3;
Philo AO_Philo4;

/*..........................................................................*/
void Philo_ctor(void) {                    /* instantiate all Philo objects */
    QActive_ctor(&AO_Philo0.super, (QStateHandler)&Philo_initial);
    QActive_ctor(&AO_Philo1.super, (QStateHandler)&Philo_initial);
    QActive_ctor(&AO_Philo2.super, (QStateHandler)&Philo_initial);
    QActive_ctor(&AO_Philo3.super, (QStateHandler)&Philo_initial);
    QActive_ctor(&AO_Philo4.super, (QStateHandler)&Philo_initial);
}
/*..........................................................................*/
QState Philo_initial(Philo *me) {
    BSP_randomSeed(123U);
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
             return Q_TRAN(&Philo_hungry);
        }
        case EAT_SIG:
        case DONE_SIG: {
            Q_ERROR();    /* these events should never arrive in this state */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Philo_hungry(Philo *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_post((QActive *)&AO_Table, HUNGRY_SIG, me->super.prio);
            return Q_HANDLED();
        }
        case EAT_SIG: {
            return Q_TRAN(&Philo_eating);
        }
        case DONE_SIG: {
            Q_ERROR();      /* thes event should never arrive in this state */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Philo_eating(Philo *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, EAT_TIME);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
             QActive_post((QActive *)&AO_Table, DONE_SIG, me->super.prio);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Philo_thinking);
        }
        case EAT_SIG:
        case DONE_SIG: {
            Q_ERROR();    /* these events should never arrive in this state */
        }
    }
    return Q_SUPER(&QHsm_top);
}


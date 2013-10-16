/*****************************************************************************
* Product: Simple Blinky example
* Last Updated for Version: 5.1.0
* Date of the Last Update:  Oct 09, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
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
#include "qpn_port.h"
#include "bsp.h"

//Q_DEFINE_THIS_FILE

/*..........................................................................*/
typedef struct BlinkyTag {                      /* the Blinky active object */
    QActive super;                                   /* derive from QActive */
} Blinky;


void Blinky_ctor(Blinky * const me);
                                          /* hierarchical state machine ... */
static QState Blinky_initial(Blinky * const me);
static QState Blinky_off    (Blinky * const me);
static QState Blinky_on     (Blinky * const me);

/*..........................................................................*/
void Blinky_ctor(Blinky * const me) {
    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky_initial));
}

/* HSM definition ----------------------------------------------------------*/
QState Blinky_initial(Blinky * const me) {
    QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2U);
    return Q_TRAN(&Blinky_off);
}
/*..........................................................................*/
QState Blinky_off(Blinky * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_ledOff();
            status = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2U);
            status = Q_TRAN(&Blinky_on);
            break;
        }
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}
/*..........................................................................*/
QState Blinky_on(Blinky * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_ledOn();
            status = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            QActive_arm((QActive *)me, BSP_TICKS_PER_SEC/2U);
            status = Q_TRAN(&Blinky_off);
            break;
        }
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

/* test harness ============================================================*/

/* Local-scope objects -----------------------------------------------------*/
static Blinky l_blinky;                         /* the Blinky active object */
static QEvt l_blinkyQSto[10];             /* Event queue storage for Blinky */

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                      },
    { (QActive *)&l_blinky,   l_blinkyQSto,     Q_DIM(l_blinkyQSto)     }
};

/* make sure that the QF_active[] array matches QF_MAX_ACTIVE in qpn_port.h */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

/*..........................................................................*/
int main(void) {
    Blinky_ctor(&l_blinky);                    /* instantiate all Blinky AO */

    BSP_init();

    return QF_run();                         /* transfer control to QF-nano */
}

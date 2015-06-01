/*****************************************************************************
* Product: "Deferred Event" state pattern example
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-05-24
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
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
* Web  : http://www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#include "qpn.h"     /* QP-nano API */
#include "bsp.h"     /* Board Support Package */
#include "defer.h"   /* Application interface */

#include <stdio.h>   /* this example uses printf() to report status */

/*..........................................................................*/
typedef struct TServerTag { /* Transaction Server active object */
    QActive super;          /* inherit QActive */

    QEvt deferredRequest;   /* deferred request event */
    /* optionally other storage for deferring other event types... */
} TServer;

/* hierarchical state machine ... */
static QState TServer_initial    (TServer *me);
static QState TServer_operational(TServer *me);
static QState TServer_idle       (TServer *me);
static QState TServer_receiving  (TServer *me);
static QState TServer_authorizing(TServer *me);
static QState TServer_final      (TServer *me);

/* helper functions */
void TServer_deferRequest(TServer *me);
void TServer_recallRequest(TServer *me);

/* Global objects ----------------------------------------------------------*/
TServer AO_TServer; /* the single instance of the TServer active object */

/*..........................................................................*/
void TServer_ctor(void) {
    QActive_ctor((QActive *)&AO_TServer, Q_STATE_CAST(&TServer_initial));
}
/* HSM definition ----------------------------------------------------------*/
QState TServer_initial(TServer *me) {
    me->deferredRequest.sig = 0; /* no deferred requests */
    return Q_TRAN(&TServer_operational);
}
/*..........................................................................*/
QState TServer_final(TServer *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QF_stop(); /* terminate the application */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState TServer_operational(TServer *me) {
    switch (Q_SIG(me)) {
        case Q_INIT_SIG: {
            return Q_TRAN(&TServer_idle);
        }
        case NEW_REQUEST_SIG: {
            TServer_deferRequest(me); /* defer the request */
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            return Q_TRAN(&TServer_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState TServer_idle(TServer *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> idle\n");
            TServer_recallRequest(me); /* recall the request */
            return Q_HANDLED();
        }
        case NEW_REQUEST_SIG: {
            printf("Processing request #%d\n", (int)Q_PAR(me));
            return Q_TRAN(&TServer_receiving);
        }
    }
    return Q_SUPER(&TServer_operational);
}
/*..........................................................................*/
QState TServer_receiving(TServer *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> receiving\n");

            /* one-shot timeout in 1 second */
            QActive_armX(&me->super, 0U, BSP_TICKS_PER_SEC, 0U);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&TServer_authorizing);
        }
    }
    return Q_SUPER(&TServer_operational);
}
/*..........................................................................*/
QState TServer_authorizing(TServer *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("-> authorizing\n");
            /* one-shot timeout in 2 seconds */
            QActive_armX(&me->super, 0U, 2U*BSP_TICKS_PER_SEC, 0U);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&TServer_idle);
        }
    }
    return Q_SUPER(&TServer_operational);
}

/* helper functions ........................................................*/
void TServer_deferRequest(TServer *me) {
    if (me->deferredRequest.sig == 0U) { /* the request NOT deferred yet? */
        me->deferredRequest.sig = Q_SIG(me);
        me->deferredRequest.par = Q_PAR(me);
        printf("deferring request #%d\n", (int)me->deferredRequest.par);
    }
    else {
        printf("!!! cannot defer request #%d\n", (int)Q_PAR(me));
    }
}
void TServer_recallRequest(TServer *me) {
    if (me->deferredRequest.sig != 0U) { /* the request already deferred? */
        printf("recalling request #%d\n", (int)me->deferredRequest.par);
        QActive_post((QActive *)me,
                     me->deferredRequest.sig, me->deferredRequest.par);

        me->deferredRequest.sig = 0U; /* request no longer deferred */
    }
    else {
        printf("No requests to recall\n");
    }
}

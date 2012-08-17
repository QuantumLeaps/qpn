//////////////////////////////////////////////////////////////////////////////
// Product: QP-nano Qt Application (Qt-1T port single thread)
// Last Updated for Version: 4.5.02
// Date of the Last Update:  Aug 16, 2012
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// Quantum Leaps Web sites: http://www.quantum-leaps.com
//                          http://www.state-machine.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include <QTimer>
#include <QTime>
#include "qpn_app.h"                   // Qt Application for emulating QP-nano
//------------------
extern "C" {
    #include "qpn_port.h"
}

Q_DEFINE_THIS_MODULE("qpn_app")

/**
* \file
* \ingroup qfn
* QF-nano emulation for Qt.
*/

static QEvent::Type qp_event_type = QEvent::MaxUser;
static QTimer       *qp_tick_timer;

//////////////////////////////////////////////////////////////////////////////
class QP_Event : public QEvent {
public:
    QP_Event(QActive *a, QSignal sig, QParam par = 0U)
      : QEvent(qp_event_type),
        m_act(a),
        m_sig(sig),
        m_par(par)
    {}

private:
    QActive *m_act;                // QP active object pointer (the recipient)
    QSignal m_sig;
    QParam  m_par;

    friend class QPnApp;
};

//............................................................................
QPnApp::QPnApp(int &argc, char **argv)
  : QApplication(argc, argv)
{
    qp_event_type =
        static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::MaxUser));
}
//............................................................................
bool QPnApp::event(QEvent *e) {
    if (e->type() == qp_event_type) {

        QActive *a = (static_cast<QP_Event *>(e))->m_act;

        Q_SIG(a) = (static_cast<QP_Event *>(e))->m_sig;
#if (Q_PARAM_SIZE != 0)
        Q_PAR(a) = (static_cast<QP_Event *>(e))->m_par;
#endif

#ifndef QF_FSM_ACTIVE
        QHsm_dispatch(&a->super);                           // dispatch to HSM
#else
        QFsm_dispatch(&a->super);                           // dispatch to FSM
#endif

        return true;                           // event recognized and handled
    }
    else {
        return QApplication::event(e);           // delegate to the superclass
    }
}

//////////////////////////////////////////////////////////////////////////////
extern "C" {

static int qp_tick_interval_ms = 10;             // default 10ms tick interval

//............................................................................
void QF_init(void) {
}
//............................................................................
int16_t QF_run(void) {
    uint8_t p;
    QActive *a;

    /* set priorities all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
        Q_ASSERT(a != (QActive *)0);    /* QF_active[p] must be initialized */
        a->prio = p;               /* set the priority of the active object */
    }
         /* trigger initial transitions in all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = QF_ROM_ACTIVE_GET_(p);
#ifndef QF_FSM_ACTIVE
        QHsm_init(&a->super);         /* take the initial transition in HSM */
#else
        QFsm_init(&a->super);         /* take the initial transition in FSM */
#endif
    }

    QF_onStartup();                             // invoke the startup callback

    qp_tick_timer = new QTimer(qApp);
    QObject::connect(qp_tick_timer, SIGNAL(timeout()),
                     qApp,          SLOT(onClockTick()));
    qp_tick_timer->setSingleShot(false);                      // periodic timer
    qp_tick_timer->setInterval(qp_tick_interval_ms);   // set system clock tick
    qp_tick_timer->start();

    return static_cast<int16_t>(qApp->exec());        // run the Qt event loop
}
//............................................................................
void QF_stop(void) {
    qp_tick_timer->stop();
    delete qp_tick_timer;
}
//............................................................................
void QF_setTickRate(uint32_t ticksPerSec) {
    qp_tick_interval_ms = 1000 / ticksPerSec;           // tick interval in ms
}

//............................................................................
#if (Q_PARAM_SIZE != 0)
void QActive_post(QActive * const me, QSignal sig, QParam par) {
    QCoreApplication::postEvent(qApp, new QP_Event(me, sig, par), me->prio);
}
#else
void QActive_post(QActive * const me, QSignal sig) {
    QCoreApplication::postEvent(qApp, new QP_Event(me, sig), me->prio);
}
#endif

//----------------------------------------------------------------------------
#if (QF_TIMEEVT_CTR_SIZE != 0)

void QF_tickISR(void) {
    uint8_t p = (uint8_t)QF_MAX_ACTIVE;
    do {
        QActive *a = QF_ROM_ACTIVE_GET_(p);
        if (a->tickCtr != (QTimeEvtCtr)0) {
            --a->tickCtr;
            if (a->tickCtr == (QTimeEvtCtr)0) {
#if (Q_PARAM_SIZE != 0)
                QActive_post(a, (QSignal)Q_TIMEOUT_SIG, (QParam)0);
#else
                QActive_post(a, (QSignal)Q_TIMEOUT_SIG);
#endif
            }
        }
        --p;
    } while (p != (uint8_t)0);
}

#if (QF_TIMEEVT_CTR_SIZE > 1)
//............................................................................
void QActive_arm(QActive * const me, QTimeEvtCtr tout) {
    me->tickCtr = tout;
}
//............................................................................
void QActive_disarm(QActive * const me) {
    me->tickCtr = (QTimeEvtCtr)0;
}
#endif                                        // #if (QF_TIMEEVT_CTR_SIZE > 1)

#endif                                       // #if (QF_TIMEEVT_CTR_SIZE != 0)

}                                                                // extern "C"


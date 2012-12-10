//////////////////////////////////////////////////////////////////////////////
// Product: QP-nano, BSP for PELICAN crossing example
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
#include <QtGui>
#include "gui.h"
#include "qpn_app.h"                   // Qt Application for emulating QP-nano
//-----------------
extern "C" {

#include "qpn_port.h"
#include "pelican.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

//............................................................................
static uint8_t const l_time_tick = 0U;                               // for QS

//............................................................................
void QPnApp::onClockTick(void) {                                       // slot
    QF_tickISR();
}
//............................................................................
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC);
}
//............................................................................
void Q_onAssert(char const * const file, int line) {
    QMessageBox::critical(0, "PROBLEM",
        QString("<p>Assertion failed in module <b>%1</b>,"
                "line <b>%2</b></p>")
            .arg(file)
            .arg(line));
    qFatal("Assertion failed in module %s, line %d", file, line);
}

//............................................................................
void BSP_init(void) {
}
//............................................................................
void BSP_terminate(int16_t result) {
    (void)result;
    qDebug("terminate");
    QF_stop();                                     // stop the QF_run() thread
    qApp->quit();  // quit the Qt application *after* the QF_run() has stopped
}
//............................................................................
void BSP_showState(char_t const *state) {
    qDebug("state:%s", state);
    Gui::instance()->m_stateLabel->setText(state);
}
//............................................................................
void BSP_signalCars(enum BSP_CarsSignal  sig) {
    qDebug("cars:%d", (int)sig);
    switch (sig) {
    case CARS_RED:
        Gui::instance()->m_carsLabel
                ->setPixmap(QPixmap(":/res/cars_red.png"));
        break;
    case CARS_YELLOW:
        Gui::instance()->m_carsLabel
                ->setPixmap(QPixmap(":/res/cars_ylw.png"));
        break;
    case CARS_GREEN:
        Gui::instance()->m_carsLabel
                ->setPixmap(QPixmap(":/res/cars_grn.png"));
        break;
    case CARS_BLANK:
        Gui::instance()->m_carsLabel
                ->setPixmap(QPixmap(":/res/cars_blank.png"));
        break;
    }
}
//............................................................................
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    qDebug("peds:%d", (int)sig);
    switch (sig) {
    case PEDS_DONT_WALK:
        Gui::instance()->m_pedsLabel
                ->setPixmap(QPixmap(":/res/peds_dont.png"));
        break;
    case PEDS_WALK:
        Gui::instance()->m_pedsLabel
                ->setPixmap(QPixmap(":/res/peds_walk.png"));
        break;
    case PEDS_BLANK:
        Gui::instance()->m_pedsLabel
                ->setPixmap(QPixmap(":/res/peds_blank.png"));
        break;
    }
}

}                                                                // extern "C"



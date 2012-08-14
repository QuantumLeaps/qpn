//////////////////////////////////////////////////////////////////////////////
// Product: QP/C example
// Last Updated for Version: 4.5.01
// Date of the Last Update:  Jun 14, 2012
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
    #include "dpp.h"
    #include "bsp.h"

Q_DEFINE_THIS_FILE

//............................................................................
static unsigned l_rnd;                                          // random seed

//............................................................................
void QPnApp::onClockTick(void) {                                       // slot
    QF_tickISR();
}
//............................................................................
void QF_onStartup(void) {
    QF_setClockTick(BSP_TICKS_PER_SEC);
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
void BSP_displayPhilStat(uint8_t n, char_t const *stat) {
    Q_ASSERT(n < Q_DIM(Gui::instance()->m_philoLabel));

    qDebug("philo[%d] is %s", n, stat);

    switch (stat[0]) {
    case 't':
        Gui::instance()->m_philoLabel[n]
                       ->setPixmap(QPixmap(":/res/thinking.png"));
        break;
    case 'h':
        Gui::instance()->m_philoLabel[n]
                        ->setPixmap(QPixmap(":/res/hungry.png"));
        break;
    case 'e':
        Gui::instance()->m_philoLabel[n]
                        ->setPixmap(QPixmap(":/res/eating.png"));
        break;
    }
}
//............................................................................
void BSP_displayPaused(uint8_t paused) {
    if (paused != 0U) {
        qDebug("PAUSED");
        Gui::instance()->m_pauseButton->setText("PAUSED");
    }
    else {
        qDebug("SERVING");
        Gui::instance()->m_pauseButton->setText("SERVING");
    }
}
//............................................................................
void BSP_terminate(void) {
    qDebug("terminate");
    qApp->quit();
}
//............................................................................
uint32_t BSP_random(void) {     // a very cheap pseudo-random-number generator
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}
//............................................................................
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}

}                                                                // extern "C"

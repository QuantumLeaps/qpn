//////////////////////////////////////////////////////////////////////////////
// Product: QP/C example
// Last Updated for Version: 4.5.00
// Date of the Last Update:  May 29, 2012
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
}

Q_DEFINE_THIS_FILE

//............................................................................
static Gui *l_instance;

//............................................................................
Gui::Gui(QWidget *parent)
    : QDialog(parent)
{
    l_instance = this;   // initialize the instance (Singleton)

    for (unsigned i = 0U; i < Q_DIM(m_philoLabel); ++i) {
         m_philoLabel[i] = new QLabel(this);
         //m_philoLabel[i]->setPixmap(QPixmap(":/res/thinking.png"));
    }
    m_philoLabel[0]->setGeometry( 34,  13, 80, 74);
    m_philoLabel[1]->setGeometry(128,  13, 80, 74);
    m_philoLabel[2]->setGeometry(158, 102, 80, 74);
    m_philoLabel[3]->setGeometry( 82, 158, 80, 74);
    m_philoLabel[4]->setGeometry(  4, 102, 80, 74);


    m_pauseButton = new QPushButton(this);
    m_pauseButton->setText("SERVING");
    m_pauseButton->setDefault(true);
    m_pauseButton->setGeometry(2, 275, 237, 25);

    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setGeometry(2, 301, 237, 25);

    connect(m_pauseButton, SIGNAL(pressed()),
            this,          SLOT(onPausePressed()));
    connect(m_pauseButton, SIGNAL(released()),
            this,          SLOT(onPauseReleased()));
    connect(m_closeButton, SIGNAL(clicked()),
            this,          SLOT(onCloseClicked()));

    setWindowTitle(tr("Dining Philosophers"));
}
//............................................................................
Gui *Gui::instance() {
    return l_instance;
}
//............................................................................
void Gui::onPausePressed() {                                           // slot
    QActive_post((QActive *)&AO_Table, PAUSE_SIG, 0U);
    qDebug("onPausePressed");
}
//............................................................................
void Gui::onPauseReleased() {                                          // slot
    QActive_post((QActive *)&AO_Table, PAUSE_SIG, 0U);
    qDebug("onPauseReleased");
}
//............................................................................
void Gui::onCloseClicked() {                                           // slot
    QActive_post((QActive *)&AO_Table, TERMINATE_SIG, 0U);
    qDebug("onCloseClicked");
}

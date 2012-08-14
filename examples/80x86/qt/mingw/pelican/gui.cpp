//////////////////////////////////////////////////////////////////////////////
// Product: QP-nano, PELICAN example
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
    #include "pelican.h"
    #include "bsp.h"
}

//............................................................................
static Gui *l_instance;

//............................................................................
Gui::Gui(QWidget *parent)
    : QDialog(parent)
{
    l_instance = this;   // initialize the instance (Singleton)

    m_stateLabel = new QLabel("               ", this);
    m_carsLabel  = new QLabel(this);
    m_pedsLabel  = new QLabel(this);

    m_carsLabel->setPixmap(QPixmap(":/res/cars_blank.png"));
    m_pedsLabel->setPixmap(QPixmap(":/res/peds_blank.png"));

    m_pedsButton = new QPushButton(tr(" PEDS_WAITING "), this);
    m_pedsButton->setDefault(true);

    m_closeButton = new QPushButton(tr("Close"), this);
    m_onOffButton = new QPushButton(tr("ON"), this);
    m_onOffButton->setDefault(true);

    connect(m_pedsButton,  SIGNAL(clicked()),
            this,          SLOT(onPedsClicked()));
    connect(m_onOffButton, SIGNAL(pressed()),
            this,          SLOT(onOnPressed()));
    connect(m_onOffButton, SIGNAL(released()),
            this,          SLOT(onOnReleased()));
    connect(m_closeButton, SIGNAL(clicked()),
            this,          SLOT(onCloseClicked()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_stateLabel);
    mainLayout->addWidget(m_carsLabel);
    mainLayout->addWidget(m_pedsLabel);
    mainLayout->addWidget(m_pedsButton);
    mainLayout->addWidget(m_onOffButton);
    mainLayout->addWidget(m_closeButton);

    setLayout(mainLayout);

    setWindowTitle(tr("PELICAN crossing"));
    setFixedHeight(sizeHint().height());
}
//............................................................................
Gui *Gui::instance() {                                           // static
    return l_instance;
}
//............................................................................
void Gui::onPedsClicked() {                                            // slot
    QActive_post((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0U);
    qDebug("onPedsClicked");
}
//............................................................................
void Gui::onOnPressed() {                                              // slot
    m_onOffButton->setText("OFF");
    QActive_post((QActive *)&AO_Pelican, OFF_SIG, 0U);
    qDebug("onOnPressed");
}
//............................................................................
void Gui::onOnReleased() {                                             // slot
    m_onOffButton->setText("ON");
    QActive_post((QActive *)&AO_Pelican, ON_SIG, 0U);
    qDebug("onOnReleased");
}
//............................................................................
void Gui::onCloseClicked() {                                           // slot
    QActive_post((QActive *)&AO_Pelican, TERMINATE_SIG, 0U);
    qDebug("onCloseClicked");
}

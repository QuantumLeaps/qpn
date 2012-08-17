//////////////////////////////////////////////////////////////////////////////
// Product: GUI for QP-nano "Fly 'n' Shoot" game example
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
//-----------------
extern "C" {
    #include "qpn_port.h"
    #include "game.h"
    #include "bsp.h"
}

Q_DEFINE_THIS_FILE

//............................................................................
Gui *Gui::instance;

//............................................................................
Gui::Gui(QWidget *parent)
  : QDialog(parent)
{
    instance = this;  // only one instance for the Gui

    setupUi(this);

    static quint8 const c_offColor[] = {  15U,  15U,  15U };
    m_display->init(BSP_SCREEN_WIDTH, 2U,
                    BSP_SCREEN_HEIGHT, 2U,
                    c_offColor);
}
//............................................................................
void Gui::onBtnPressed() {                                             // slot
    static QPixmap const btnDWN(":/res/EK-BTN_DWN.png");
    m_button->setIcon(btnDWN);
    QActive_post((QActive *)&AO_Ship,   PLAYER_TRIGGER_SIG, 0U);
    QActive_post((QActive *)&AO_Tunnel, PLAYER_TRIGGER_SIG, 0U);
}
//............................................................................
void Gui::onBtnReleased() {                                            // slot
    static QPixmap const btnUP(":/res/EK-BTN_UP.png");
    m_button->setIcon(btnUP);
}
//............................................................................
void Gui::onQuit() {                                                   // slot
    BSP_terminate(0);
}
//............................................................................
void Gui::wheelEvent(QWheelEvent *e) {
    if (e->delta() >= 0) {
        BSP_moveShipUp();
    }
    else {
        BSP_moveShipDown();
    }
}

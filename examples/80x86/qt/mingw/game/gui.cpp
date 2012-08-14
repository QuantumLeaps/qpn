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

    m_background = new QLabel(this);
    m_background->setPixmap(QPixmap(":/res/EK-LM3S811.png"));
    m_background->setGeometry( 0,  0, 799, 294);

    m_display = new QLabel(this);
    m_display->setGeometry(427, 130, 192, 32);

    m_button = new QPushButton(this);
    m_button->setIconSize (QSize(43, 45));
    m_button->setIcon(QPixmap(":/res/EK-BTN_UP.png"));
    m_button->setGeometry(557, 210, 43, 45);

    connect(m_button, SIGNAL(pressed()),
            this,     SLOT(onBtnPressed()));
    connect(m_button, SIGNAL(released()),
            this,     SLOT(onBtnReleased()));

    m_scoreLbl = new QLabel(tr("Score:"), this);
    m_scoreLbl->setGeometry(5, 300, 60, 20);

    m_score = new QLabel("0", this);
    m_score->setGeometry(65, 300, 40, 20);

    setWindowTitle(tr("Fly 'n' Shoot game on EK-LM3S811"));
}
//............................................................................
void Gui::onBtnPressed() {                                             // slot
    m_button->setIcon(QPixmap(":/res/EK-BTN_DWN.png"));
    QActive_post((QActive *)&AO_Ship,   PLAYER_TRIGGER_SIG, 0U);
    QActive_post((QActive *)&AO_Tunnel, PLAYER_TRIGGER_SIG, 0U);
}
//............................................................................
void Gui::onBtnReleased() {                                            // slot
    m_button->setIcon(QPixmap(":/res/EK-BTN_UP.png"));
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

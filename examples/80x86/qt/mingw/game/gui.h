#ifndef GUI_H
#define GUI_H

#include <QDialog>

class QLabel;
class QPushButton;

class Gui : public QDialog {
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = 0);
    static Gui *instance;

private slots:
    void onBtnPressed();
    void onBtnReleased();

protected:
    virtual void wheelEvent(QWheelEvent *e);
    //virtual void keyPressEvent(QKeyEvent *e);

public:
    QLabel      *m_background;
    QLabel      *m_display;
    QPushButton *m_button;

    QLabel      *m_scoreLbl;
    QLabel      *m_score;
};

#endif // GUI_H

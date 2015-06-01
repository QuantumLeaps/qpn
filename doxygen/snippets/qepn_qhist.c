typedef struct  {
    QHsm super; /* inherit QHsm */

    QStateHandler hist_doorClosed; /* history of doorClosed */
} ToastOven;

/*..........................................................*/
static QState ToastOven_doorClosed(ToastOven * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        ~ ~ ~
        case Q_EXIT_SIG: {
            me->hist_doorClosed = QHsm_state(&me->super);
            status_ = Q_HANDLED();
            break;
        }
    }
    return status_;
}
/*..........................................................*/
static QState ToastOven_doorOpen(ToastOven * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        ~ ~ ~
        case CLOSE_SIG: {
            status_ = Q_TRAN_HIST(hist_doorClosed); /*<== */
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

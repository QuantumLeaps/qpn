/* HSM definition ----------------------------------------------------------*/
QState Pelican_carsGreenNoPed(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenNoPed");
            status_ = Q_HANDLED();
            break;
        }
        case PEDS_WAITING_SIG: {
            status_ = Q_TRAN(&Pelican_carsGreenPedWait);
            break;
        }
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Pelican_carsGreenInt);
            break;
        }
        default: {
            status_ = Q_SUPER(&Pelican_carsGreen);
            break;
        }
    }
    return status_;
}
/*..........................................................................*/
QState Pelican_carsGreenPedWait(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenPedWait");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Pelican_carsYellow);
            break;
        }
        default: {
            status_ = Q_SUPER(&Pelican_carsGreen);
            break;
        }
    }
    return status_;
}


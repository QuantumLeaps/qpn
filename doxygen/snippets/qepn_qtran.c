/* HSM definition ----------------------------------------------------------*/
QState Pelican_carsGreenNoPed(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenNoPed");
            return Q_HANDLED();
        }
        case PEDS_WAITING_SIG: {
            return Q_TRAN(&Pelican_carsGreenPedWait);
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Pelican_carsGreenInt);
        }
    }
    return Q_SUPER(&Pelican_carsGreen);
}
/*..........................................................................*/
QState Pelican_carsGreenPedWait(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "carsGreenPedWait");
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            return Q_TRAN(&Pelican_carsYellow);
        }
    }
    return Q_SUPER(&Pelican_carsGreen);
}


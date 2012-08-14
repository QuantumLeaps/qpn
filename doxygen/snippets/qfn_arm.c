QState Pelican_carsGreen(Pelican *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QActive_arm((QActive *)me, CARS_GREEN_MIN_TOUT);  /* arm timer */
            BSP_signalCars(CARS_GREEN);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Pelican_carsGreenNoPed);
        }
    }
    return Q_SUPER(&Pelican_carsEnabled);
}

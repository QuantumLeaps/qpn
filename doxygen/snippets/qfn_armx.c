/* when #QF_TIMEEVT_PERIODIC is NOT defined... */
QState Pelican_carsGreen(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* arm timer (one-shot) at tick rate 0 */
            QActive_armX(&me->super, 0U,
                         CARS_GREEN_MIN_TOUT);
            BSP_signalCars(CARS_GREEN);
            status_ = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            QActive_disarm(&me->super)
            status_ = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Pelican_carsGreenNoPed);
            break;
        }
        default: {
            status_ = Q_SUPER(&Pelican_carsEnabled);
            break;
        }
    }
    return status_;
}

/* when #QF_TIMEEVT_PERIODIC is defined... */
QState Pelican_carsGreen(Pelican * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            /* arm timer (one-shot) at tick rate 0 */
            QActive_armX(&me->super, 0U,
                         CARS_GREEN_MIN_TOUT, 0U);
            BSP_signalCars(CARS_GREEN);
            status_ = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            QActive_disarm(&me->super)
            status_ = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Pelican_carsGreenNoPed);
            break;
        }
        default: {
            status_ = Q_SUPER(&Pelican_carsEnabled);
            break;
        }
    }
    return status_;
}

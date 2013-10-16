/*..........................................................................*/
void Pelican_ctor(Pelican * const me) {
    QActive_ctor(me, Q_STATE_CAST(&Pelican_initial));
}

/* HSM definition ----------------------------------------------------------*/
QState Pelican_initial(Pelican * const me) { /* the initial pseudo-state */
    return Q_TRAN(&Pelican_operational);
}
/*..........................................................................*/
QState Pelican_operational(Pelican * const me) { /* a regular state handler */
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_signalCars(CARS_RED);
            BSP_signalPeds(PEDS_DONT_WALK);
            status_ = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Pelican_carsEnabled); /* regular transition */
            break;
        }
        case OFF_SIG: {
            status_ = Q_TRAN(&Pelican_offline);     /* regular transition */
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);            /* the superstate */
            break;
        }
    }
    return status_;
}


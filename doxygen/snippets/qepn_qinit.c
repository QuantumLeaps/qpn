/*..........................................................................*/
void Pelican_ctor(Pelican *me) {
    QActive_ctor(me, (QStateHandler)&Pelican_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Pelican_initial(Pelican *me) {   /* the initial pseudo-state handler */
    return Q_TRAN(&Pelican_operational);
}
/*..........................................................................*/
QState Pelican_operational(Pelican *me) {        /* a regular state handler */
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_signalCars(CARS_RED);
            BSP_signalPeds(PEDS_DONT_WALK);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Pelican_carsEnabled);       /* return transition */
        }
        case OFF_SIG: {
            return Q_TRAN(&Pelican_offline);           /* return transition */
        }
    }
    return Q_SUPER(&QHsm_top);                     /* return the superstate */
}


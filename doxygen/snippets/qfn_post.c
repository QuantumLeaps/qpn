QState Ped_wait(Ped *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_showState(me->super.prio, "wait");
            me->retryCtr = N_ATTEMPTS;
            QActive_arm((QActive *)me, WAIT_TOUT);
            return Q_HANDLED();
        }
        case Q_TIMEOUT_SIG: {
            if ((--me->retryCtr) != 0) {
                QActive_arm((QActive *)me, WAIT_TOUT);
                QActive_post((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0);
            }
            else {
                return Q_TRAN(&Ped_off);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

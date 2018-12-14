void Alarm_ctor(Alarm * const me) {
    QHsm_ctor(&me->super, Q_STATE_CAST(&Alarm_initial));
}

void QBomb_ctor(QBomb *me, uint8_t timeout) {
    QActive_ctor((&me->super,   /* superclass ctor */
                 Q_STATE_CAS(&QBomb_initial));
    me->timeout = timeout * 2U; /* set the timeout */
}

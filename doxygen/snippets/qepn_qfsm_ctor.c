void Bomb_ctor(Bomb * const me, uint8_t timeout) {
    QFsm_ctor(&me->super,   /* superclass ctor */
              Q_STATE_CAS(&Bomb_initial));
    me->timeout = timeout * 2U; /* set the timeout */
}

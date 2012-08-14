void Pelican_ctor(Pelican *me, uint8_t timeout) {
    QActive_ctor(&me->super, (QStateHandler)&Pelican_initial);
    me->pedFlashCtr = timeout;
}

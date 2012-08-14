void QBomb_ctor(QBomb *me, uint8_t timeout) {
    QActive_ctor((QActive *)me,                          /* superclass ctor */
                 (QStateHandler)&QBomb_initial);
    me->timeout = timeout * 2;                           /* set the timeout */
}

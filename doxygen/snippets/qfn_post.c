void BSP_onKeyboardInputISR(uint8_t key) {
    switch (key) {
        case '\33': {                                 /* ESC pressed? */
            QACTIVE_POST_ISR((QActive *)&AO_Cruncher, TERMINATE_SIG, 0);
            break;
        }
        case 'e': {                                     /* echo event */
            QACTIVE_POST_ISR((QActive *)&AO_Cruncher, ECHO_SIG, 0);
            break;
        }
    }
}
/*....................................................................*/
QState Cruncher_processing(Cruncher * const me) {
    QState status;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            QACTIVE_POST((QActive *)me, CRUNCH_SIG, 0);
            me->sum = 0.0;
            status = Q_HANDLED();
            break;
        }
        case CRUNCH_SIG: {
            . . .
            if (i < 0x07000000) {
                QACTIVE_POST((QActive *)me, CRUNCH_SIG, i);
                status = Q_HANDLED();
            }
            break;
        }
        . . .
    }
    return status;
}

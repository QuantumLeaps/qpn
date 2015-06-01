/* "extended" event posting from the interrupt context (QACTIVE_POST_X_ISR) */
void SysTick_Handler(void) {
    . . .
    if (!QACTIVE_POST_X_ISR((QMActive *)&AO_Cruncher,
             5U,            /* margin of free slots in the queue */
             ECHO_SIG, 0U)) /* signal and parameter */
    {
        /* event posting failed... */
    }
}

/* "extended" event posting from the task context (QACTIVE_POST_X())... */
static QState Ship_flying(Ship * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case TIME_TICK_SIG: {
            . . .
            if ((me->score % 10) == 0) { /* is the score "round"? */
                if (!QACTIVE_POST_X((QMActive *)&AO_Tunnel,
                        4U, /* margin of free slots in the queue */
                        SCORE_SIG, me->score)) /* signal and parameter */
                {
                    /* event posting failed... */
                }
            }
            status_ = QM_HANDLED();
            break;
        }
        . . .
    }
    return status_;
}

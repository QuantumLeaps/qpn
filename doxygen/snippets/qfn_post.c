/* event posting from the interrupt context (QACTIVE_POST_ISR()) ... */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
    QK_ISR_ENTRY();  /* infrom QK-nano about entering an ISR */

    QF_tickXISR(0U); /* process time events for rate 0 */

    /* post TIME_TICK events to all interested active objects... */
    QACTIVE_POST_ISR((QActive *)&AO_Tunnel,  TIME_TICK_SIG, 0U);
    QACTIVE_POST_ISR((QActive *)&AO_Ship,    TIME_TICK_SIG, 0U);
    QACTIVE_POST_ISR((QActive *)&AO_Missile, TIME_TICK_SIG, 0U);

    QK_ISR_EXIT();   /* infrom QK-nano about exiting an ISR */
}

/* event posting from the task context (QACTIVE_POST())... */
static QState Ship_flying(Ship * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case TIME_TICK_SIG: {
            . . .
            if ((me->score % 10) == 0) { /* is the score "round"? */
                QACTIVE_POST((QActive *)&AO_Tunnel,
                    SCORE_SIG, me->score); /* signal and parameter */
            }
            status_ = Q_HANDLED();
            break;
        }
        . . .
    }
    return status_;
}

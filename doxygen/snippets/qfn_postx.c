/* event posting from the ISR level ...................................*/
void SysTick_Handler(void) {
    . . .
    if (!QACTIVE_POST_X_ISR((QActive *)&AO_Cruncher,
                            5U,   /* margin of free slots in the queue */
                            ECHO_SIG, 0))      /* signal and parameter */
    {
        /* event posting failed... */
    }
}

/* event posting from the task level ..................................*/
QState Cruncher_processing(Cruncher * const me) {
    QState status;
    switch (Q_SIG(me)) {
        . . .
        case CRUNCH_SIG: {
            . . .
            if (!QACTIVE_POST((QActive *)me,
                              2U, /* margin of free slots in the queue */
                              CRUNCH_SIG, i))  /* signal and parameter */
            {
                /* event posting failed... */
            }
            status = Q_HANDLED();
            break;
        }
        . . .
    }
    return status;
}

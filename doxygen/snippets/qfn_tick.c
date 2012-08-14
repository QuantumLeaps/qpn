/* the system time tick ISR for C8051 from Silicon Labs ....................*/
#pragma vector=0x2B
__interrupt void timer2_ISR(void) {
   TMR2CN &= ~(1 << 7);                      /* Clear Timer2 interrupt flag */
   QF_tick();                    /* handle all armed time events in QF-nano */
}

/* the system time tick ISR for MSP430 from TI (non-preemptive case) .......*/
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
    __low_power_mode_off_on_exit();
   QF_tick();                    /* handle all armed time events in QF-nano */
}

/* the system time tick ISR for MSP430 from TI (preemptive case QK-nano) ...*/
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
    QK_ISR_ENTRY();                /* inform QK-nano about entering the ISR */
    QF_tick();                   /* handle all armed time events in QF-nano */
    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}

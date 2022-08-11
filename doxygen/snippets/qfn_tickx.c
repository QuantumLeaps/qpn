/* system clock tick ISR for MSP430 from TI (cooperative QV-nano kernel) */
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
    __low_power_mode_off_on_exit();
   QF_tickXISR(0U);  /* handle all armed time events at tick rate 0 */
}

/* system clock tick ISR for MSP430 from TI (preemptive QK-nano kernel) */
#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_ISR(void) {
    QK_ISR_ENTRY();  /* inform QK-nano about entering the ISR */
    QF_tickXISR(0U); /* handle all armed time events  at tick rate 0 */
    QK_ISR_EXIT();   /* inform QK-nano about exiting the ISR */
}

/*****************************************************************************
* Product: BSP for the PSoCEVAL1 board
* Last Updated for Version: 4.0.02
* Date of the Last Update:  Nov 04, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*
*-----------------------------------------------------------------------------
* PSoCEVAL1 board custom wiring:
*     P00 -> LED1
*     P01 -> LED2
*     P02 -> LED3
*     P03 -> LED4
*     P10 -> SW
*     P16 -> RX = Serial RX
*     P27 -> TX = Serial TX
*****************************************************************************/
#include "qpn_port.h"
#include "pelican.h"
#include "bsp.h"

#define SW_1 0x01

/*..........................................................................*/
static void delay(uint16_t count) {
    while ((--count) != 0) {
        asm("nop");
    }
}
/*..........................................................................*/
#pragma interrupt_handler BSP_tick
void BSP_tick(void) {
    static uint8_t sw_debounced;
    static uint8_t debounce_state;
    uint8_t sw;

    LED_3_Invert();                                       /* toggle the LED */

    QF_tick();                                      /* process armed timers */

    sw = (PRT1DR & SW_1);                                /* read the switch */
    switch (debounce_state) {              /* switch debounce state machine */
        case 0:
            if (sw != sw_debounced) {
                debounce_state = 1;         /* transition to the next state */
            }
            break;
        case 1:
            if (sw != sw_debounced) {
                debounce_state = 2;         /* transition to the next state */
            }
            else {
                debounce_state = 0;           /* transition back to state 0 */
            }
            break;
        case 2:
            if (sw != sw_debounced) {
                sw_debounced = sw;       /* save the debounced switch value */

                if (sw == SW_1) {               /* is the button depressed? */
                    QActive_postISR((QActive *)&AO_Pelican,
                                    PEDS_WAITING_SIG, 0);
                }
            }
            debounce_state = 0;               /* transition back to state 0 */
            break;
    }
}
/*..........................................................................*/
void BSP_init(void) {
    LED_1_Start();
    LED_2_Start();
    LED_3_Start();
    LED_4_Start();

    PRT1DR &= ~SW_1;             /* set the output direction for the SW pin */

    UART_1_Start(UART_PARITY_NONE);                          /* enable UART */
    UART_1_PutCRLF();
    UART_1_CPutString("PELICAN example, QP-nano ");
    UART_1_CPutString(QP_getVersion());                  /* QP-nano version */
    UART_1_PutCRLF();

    SleepTimer_1_Start();
    SleepTimer_1_SetInterval(SleepTimer_1_64_HZ);     /* set interrupt rate */
}
/*..........................................................................*/
void BSP_showState(uint8_t prio, char const Q_ROM *state) {
    if (prio == 1) {                                /* PELICAN crossing AO? */
        UART_1_CPutString(state);                     /* output the message */
        UART_1_PutCRLF();
        delay(500);    /* wait for the transmission to complete, see NOTE01 */
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    if (sig == PEDS_DONT_WALK) {
        LED_1_On();
    }
    else {
        LED_1_Off();
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    if (sig == CARS_GREEN) {
        LED_2_On();
    }
    else {
        LED_2_Off();
    }
}
/*..........................................................................*/
void QF_onStartup(void) {
    SleepTimer_1_EnableInt();
    QF_INT_UNLOCK();                                   /* unlock interrupts */
}
/*..........................................................................*/
void QF_onIdle(void) {        /* entered with interrupts LOCKED, see NOTE02 */

    LED_4_On();                                  /* blink LED_4, see NOTE03 */
    LED_4_Off();

#ifdef NDEBUG
    M8C_Sleep;       /* tread-safe transition to the sleep mode, see NOTE04 */
#endif

    QF_INT_UNLOCK();
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();            /* make sure that all interrupts are disabled */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
        LED_4_On();
        delay(10000);
        LED_4_Off();
        delay(10000);
    }
}

/*****************************************************************************
* NOTE01:
* Waiting for the UART transmission to complete is important when low-power
* sleep mode is used. If the CPU goes to sleep before the UART completes
* transmitting the last byte corrupts the state of the UART.
*
* NOTE02:
* The QF_onIdle() callback is called with interrupts locked, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally unlock interrupts, ideally atomically
* with putting the CPU to the power-saving mode.
*
* NOTE03:
* The LED_4 is switched on and off inside a critical section to prevent
* preemptions. Therefore the intensity of the LED corresponds only to the
* idle loop activity and does not include preemptions by the ISRs.
*
* NOTE04:
* As described in Section 12.2 in the "PSoC Technical Reference Manual", the
* M8C sleep mode can be entered with the global interrupts disabled. This
* guarantees that the transition to the sleep mode occurs atomically. After
* the external interrupt (such as Sleep Timer interrupt) wakes the CPU up,
* the interrupt is not taken because interrupts are still disabled.
* The interrupt is only taken after the background loop unlocks interrupts,
* which happens immediately after the wakeup from the sleep mode.
*/

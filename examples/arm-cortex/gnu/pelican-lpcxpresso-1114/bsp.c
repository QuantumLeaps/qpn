/*****************************************************************************
* Product: PELICAN crossing example, Vanilla kernel, LPCXpresso-1114 board
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Feb 29, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"
#include "pelican.h"

#include "LPC11xx.h"                                 /* LPC11xx definitions */
#include "timer16.h"
#include "clkconfig.h"
#include "gpio.h"

#define LED_PORT    0
#define LED_BIT     7
#define LED_ON      1
#define LED_OFF     0

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    SYSTICK_PRIO,
    /* ... */
};

/*..........................................................................*/
void SysTick_Handler(void) __attribute__((__interrupt__));
void SysTick_Handler(void) {
    QF_tickISR();
}
/*..........................................................................*/
void BSP_init(void) {
    SystemInit();                         /* initialize the clocking system */
    GPIOInit();                                          /* initialize GPIO */
    GPIOSetDir(LED_PORT, LED_BIT, 1);         /* set port for LED to output */
}
/*..........................................................................*/
void QF_onStartup(void) {
    /* Set up and enable the SysTick timer. It will be used as a reference
    * for delay loops in the interrupt handlers. The SysTick timer period
    * will be set up for BSP_TICKS_PER_SEC.
    */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIO);
}
/*..........................................................................*/
void QF_onIdle(void) {      /* entered with interrupts DISABLED, see NOTE01 */

    /* toggle the User LED on and then off, see NOTE02 */
    //GPIOSetValue(LED_PORT, LED_BIT, LED_ON);                   /* LED on  */
    //GPIOSetValue(LED_PORT, LED_BIT, LED_OFF);                  /* LED off */

#ifdef NDEBUG
    /* put the CPU and peripherals to the low-power mode */
    __WFI();                /* stop clocking the CPU and wait for interrupt */
#endif
    QF_INT_ENABLE();                            /* always enable interrupts */
}
/*..........................................................................*/
/* error routine that is called if the STM32 library encounters an error    */
void assert_failed(char const *file, int line) {
    Q_onAssert(file, line);
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}
/*..........................................................................*/
void BSP_showState(uint8_t prio, char const *state) {
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    if (sig == PEDS_DONT_WALK) {
        GPIOSetValue(LED_PORT, LED_BIT, LED_ON);                 /* LED on  */
    }
    else {
        GPIOSetValue(LED_PORT, LED_BIT, LED_OFF);                /* LED off */
    }
}
/*****************************************************************************
* NOTE01:
* The QF_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally enable interrupts, ideally atomically
* with putting the CPU to the power-saving mode.
*
* NOTE02:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*
*/

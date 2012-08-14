/*****************************************************************************
* Product: PELICAN crossing example, preemptive QK-nano kerenel
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Feb 27, 2012
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

#include "lm3s_cmsis.h"
#include "display96x16x1.h"

Q_DEFINE_THIS_FILE

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    GPIOPORTA_PRIO,
    SYSTICK_PRIO
    /* ... */
};

            /* The Push Button pin and User LED pin on the EK-LM3S811 board */
/*#define PUSH_BUTTON             (uint8_t)(1U << 4)*/
#define USER_LED                (uint8_t)(1U << 5)

/* ISR prototypes */
void SysTick_Handler(void);
void GPIOPortA_IRQHandler(void);

/*..........................................................................*/
void SysTick_Handler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    QF_tickISR();
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void GPIOPortA_IRQHandler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    QActive_postISR(QF_ACTIVE_CAST(&AO_Ped),
                    (QSignal)PEDS_WAITING_SIG, (QParam)0);
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}

/*..........................................................................*/
void BSP_init(void) {
    SystemInit();                         /* initialize the system clocking */

    QK_init();                             /* initialize the QK-nano kernel */

                      /* enable the peripherals used by this application... */
    SYSCTL->RCGC2 |= (1U <<  2);                  /* enable clock to GPIOC  */
    SYSCTL->RCGC1 |= (1U << 16);                  /* enable clock to TIMER0 */
    __NOP();                                  /* wait after enabling clocks */
    __NOP();
    __NOP();
                                  /* configure the User LED (PortC pin5)... */
    GPIOC->DIR |= USER_LED;                        /* set direction: output */
    GPIOC->DEN |= USER_LED;                               /* digital enable */
    GPIOC->DATA_Bits[USER_LED] = 0U;               /* turn the User LED off */

    Display96x16x1Init(1U);                  /* initialize the OLED display */
}
/*..........................................................................*/
void QF_onStartup(void) {
              /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    (void)SysTick_Config(SystemFrequency / BSP_TICKS_PER_SEC);

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn,   (uint32_t)SYSTICK_PRIO);
    NVIC_SetPriority(GPIOPortA_IRQn, (uint32_t)GPIOPORTA_PRIO);

    NVIC_EnableIRQ(GPIOPortA_IRQn);                 /* IRQ used for testing */
}
/*..........................................................................*/
void QK_onIdle(void) {

    /* toggle the User LED on and then off, see NOTE01 */
    QF_INT_DISABLE();
    GPIOC->DATA_Bits[USER_LED] = USER_LED;         /* turn the User LED on  */
    GPIOC->DATA_Bits[USER_LED] = 0U;               /* turn the User LED off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* put the CPU and peripherals to the low-power mode, see NOTE02 */
    __WFI();
#endif
}
/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const Q_ROM_VAR file, int_t line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}
/*..........................................................................*/
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char const *file, int line) {
    Q_onAssert(file, line);
}

/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            Display96x16x1StringDraw("RED", 78U, 1U);
            break;
        }
        case CARS_YELLOW: {
            Display96x16x1StringDraw("YLW", 78U, 1U);
            break;
        }
        case CARS_GREEN: {
            Display96x16x1StringDraw("GRN", 78U, 1U);
            break;
        }
        case CARS_OFF: {
            Display96x16x1StringDraw("   ", 78U, 1U);
            break;
        }
        default: {
            Q_ERROR();
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            Display96x16x1StringDraw("DON'T WALK", 0U, 1U);
            break;
        }
        case PEDS_BLANK: {
            Display96x16x1StringDraw("          ", 0U, 1U);
            break;
        }
        case PEDS_WALK: {
            Display96x16x1StringDraw("** WALK **", 0U, 1U);
            break;
        }
        default: {
            Q_ERROR();
            break;
        }
    }
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    Display96x16x1StringDraw(state, 0U, 0U);
}

/*****************************************************************************
* NOTE01:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

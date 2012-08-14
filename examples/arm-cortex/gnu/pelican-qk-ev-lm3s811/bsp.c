/*****************************************************************************
* Product: PELICAN crossing example, EV-LM3S811 board, QK-nano
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

#include "lm3s_cmsis.h"
#include "display96x16x1.h"     /* RITEK 128x96x1 OLED used in Rev C boards */

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    GPIOPORTA_PRIO,
    SYSTICK_PRIO,
    /* ... */
};

/* Local-scope objects -----------------------------------------------------*/
#define PUSH_BUTTON             (1 << 4)
#define USER_LED                (1 << 5)

/*..........................................................................*/
void SysTick_Handler(void) __attribute__((__interrupt__));
void SysTick_Handler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */

    QF_tickISR();

    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void GPIOPortA_IRQHandler(void) __attribute__((__interrupt__));
void GPIOPortA_IRQHandler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */

    QActive_postISR((QActive *)&AO_Ped, PEDS_WAITING_SIG, 0);

    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}

/*..........................................................................*/
void BSP_init(void) {

    SystemInit();                         /* initialize the system clocking */

    QK_init();              /* initialize the QK-nano kernel for ARM Cortex */

    /* enable clock to the peripherals used by the application */
    SYSCTL->RCGC2 |= (1 <<  0) | (1 <<  2);   /* enable clock to GPIOA & C  */
    __NOP();                                  /* wait after enabling clocks */
    __NOP();
    __NOP();

    /* configure the LED and push button */
    GPIOC->DIR |= USER_LED;                        /* set direction: output */
    GPIOC->DEN |= USER_LED;                               /* digital enable */
    GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */

    GPIOC->DIR &= ~PUSH_BUTTON;                    /*  set direction: input */
    GPIOC->DEN |= PUSH_BUTTON;                            /* digital enable */

    Display96x16x1Init(1);                   /* initialize the OLED display */
}
/*..........................................................................*/
void QF_onStartup(void) {
              /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemFrequency / BSP_TICKS_PER_SEC);

                 /* enable GPIOPortA interrupt used for testing preemptions */
    NVIC_EnableIRQ(GPIOPortA_IRQn);

                                /* set all interrupt priorities in the NVIC */
    NVIC_SetPriority(GPIOPortA_IRQn, GPIOPORTA_PRIO);
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
}
/*..........................................................................*/
void QK_onIdle(void) {

    /* toggle the User LED on and then off, see NOTE01 */
    QF_INT_DISABLE();
    GPIOC->DATA_Bits[USER_LED] = USER_LED;         /* turn the User LED on  */
    GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* put the CPU and peripherals to the low-power mode, see NOTE02 */
    __WFI();
#endif
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
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char const *file, int line) {
    Q_onAssert(file, line);
}

/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            Display96x16x1StringDraw("RED", 78, 1);
            break;
        }
        case CARS_YELLOW: {
            Display96x16x1StringDraw("YLW", 78, 1);
            break;
        }
        case CARS_GREEN: {
            Display96x16x1StringDraw("GRN", 78, 1);
            break;
        }
        case CARS_OFF: {
            Display96x16x1StringDraw("   ", 78, 1);
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            Display96x16x1StringDraw("DON'T WALK", 0, 1);
            break;
        }
        case PEDS_BLANK: {
            Display96x16x1StringDraw("          ", 0, 1);
            break;
        }
        case PEDS_WALK: {
            Display96x16x1StringDraw("** WALK **", 0, 1);
            break;
        }
    }
}
/*..........................................................................*/
void BSP_showState(uint8_t prio, char const *state) {
    if (QF_active[prio].act == (QActive *)&AO_Pelican) {
        Display96x16x1StringDraw(state, 0, 0);
    }
}

/*****************************************************************************
* NOTE01:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

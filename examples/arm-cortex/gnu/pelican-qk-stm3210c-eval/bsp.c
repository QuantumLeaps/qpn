/*****************************************************************************
* Product: PELICAN crossing example, STM3210C-EVAL board, QK-nano, GNU
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

#include "stm32f10x.h"
#include "stm32_eval.h"
#include "stm3210c_eval_lcd.h"

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    EXTI0_PRIO,                                         /* highest priority */
    SYSTICK_PRIO,
    /* ... */
};

/*..........................................................................*/
void SysTick_Handler(void) __attribute__((__interrupt__));
void SysTick_Handler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    QF_tickISR();
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void EXTI0_IRQHandler(void) __attribute__((__interrupt__));
void EXTI0_IRQHandler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    EXTI->PR = 0x1;   /* set the EXTI->PR[0] to clear the EXTI_SWIER[0] bit */
    QActive_postISR((QActive *)&AO_Ped, PEDS_WAITING_SIG, 0);
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}

/*..........................................................................*/
void BSP_init(void) {
    EXTI_InitTypeDef exti_init;

    SystemInit();         /* initialize STM32 system (clock, PLL and Flash) */

    QK_init();                             /* initialize the QK-nano kernel */

             /* initialize LEDs, Key Button, and LCD on STM3210X-EVAL board */
    STM_EVAL_LEDInit(LED1);
    STM_EVAL_LEDInit(LED2);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);

                    /* initialize the EXTI Line0 interrupt used for testing */
    exti_init.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
    exti_init.EXTI_Line    = EXTI_Line0;
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init);

    STM3210C_LCD_Init();                              /* initialize the LCD */
    LCD_Clear(White);                                      /* clear the LCD */
    LCD_SetBackColor(Grey);
    LCD_SetTextColor(Black);
    LCD_DisplayString(Line0, 0, "   Quantum Leaps    ");
    LCD_DisplayString(Line1, 0, "  PELICAN Crossing  ");
    LCD_DisplayString(Line2, 0, "  QP-nano (QK-nano) ");
    LCD_SetBackColor(White);
    LCD_DisplayString(Line4, 0, "stat");
    LCD_DisplayString(Line5, 0, "cars");
    LCD_DisplayString(Line6, 0, "peds");
    LCD_SetBackColor(Black);
    LCD_SetTextColor(Yellow);
    LCD_DisplayString(Line9, 0, "  state-machine.com ");
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayString(Line4, 4*16, "                ");
    LCD_DisplayString(Line5, 4*16, "                ");
    LCD_DisplayString(Line6, 4*16, "                ");
}
/*..........................................................................*/
void QF_onStartup(void) {
    NVIC_InitTypeDef nvic_init;

    /* Set up and enable the SysTick timer.  It will be used as a reference
    * for delay loops in the interrupt handlers.  The SysTick timer period
    * will be set up for BSP_TICKS_PER_SEC.
    */
    SysTick_Config(SystemFrequency_SysClk / BSP_TICKS_PER_SEC);

    /* Enable the EXTI0 Interrupt used for testing preemptions */
    nvic_init.NVIC_IRQChannel                   = EXTI0_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_init.NVIC_IRQChannelSubPriority        = 0;
    nvic_init.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic_init);/* enables the device and sets interrupt priority */

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIO);
    NVIC_SetPriority(EXTI0_IRQn,   EXTI0_PRIO);
    /* ... */
}
/*..........................................................................*/
void QK_onIdle(void) {
                         /* toggle the blue LED on and then off, see NOTE01 */
    QF_INT_DISABLE();
    STM_EVAL_LEDOn (LED4);                                  /* blue LED on  */
    STM_EVAL_LEDOff(LED4);                                  /* blue LED off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    __WFI();
#endif
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
    if (QF_active[prio].act == (QActive *)&AO_Pelican) {
        LCD_DisplayString(Line4, 4*16, state);
    }
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            LCD_DisplayString(Line5, 4*16, "RED   ");
            STM_EVAL_LEDOn (LED3);
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOff(LED1);
            break;
        }
        case CARS_YELLOW: {
            LCD_DisplayString(Line5, 4*16, "YELLOW");
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOn (LED2);
            STM_EVAL_LEDOff(LED1);
            break;
        }
        case CARS_GREEN: {
            LCD_DisplayString(Line5, 4*16, "GREEN ");
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOn (LED1);
            break;
        }
        case CARS_OFF: {
            LCD_DisplayString(Line5, 4*16, "      ");
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOff(LED1);
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            LCD_DisplayString(Line6, 4*16, "DON'T WALK");
            break;
        }
        case PEDS_BLANK: {
            LCD_DisplayString(Line6, 4*16, "          ");
            break;
        }
        case PEDS_WALK: {
            LCD_DisplayString(Line6, 4*16, "***WALK***");
            break;
        }
    }
}
/*****************************************************************************
* NOTE01:
* The blue LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*/

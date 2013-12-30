/*****************************************************************************
* Product: "Fly 'n' Shoot" game example
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 11, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "game.h"
#include "bsp.h"

#include "lm3s_cmsis.h"
#include "display96x16x1.h"

Q_DEFINE_THIS_FILE

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    GPIOPORTA_PRIO,
    ADCSEQ3_PRIO,
    SYSTICK_PRIO,
    /* ... */
};

#define ADC_TRIGGER_TIMER       0x00000005U
#define ADC_CTL_IE              0x00000040U
#define ADC_CTL_END             0x00000020U
#define ADC_CTL_CH0             0x00000000U
#define ADC_SSFSTAT0_EMPTY      0x00000100U
#define UART_FR_TXFE            0x00000080U

/* ISR prototypes */
void SysTick_Handler(void);
void ADCSeq3_IRQHandler(void);
void GPIOPortA_IRQHandler(void);


/* Local-scope objects -----------------------------------------------------*/
#define PUSH_BUTTON             (1U << 4)
#define USER_LED                (1U << 5)

/*..........................................................................*/
void SysTick_Handler(void) {
    QF_tickISR();                             /* process all armed time events */

               /* post TIME_TICK events to all interested active objects... */
    QActive_postISR((QActive *)&AO_Tunnel,  TIME_TICK_SIG, 0U);
    QActive_postISR((QActive *)&AO_Ship,    TIME_TICK_SIG, 0U);
    QActive_postISR((QActive *)&AO_Missile, TIME_TICK_SIG, 0U);
}
/*..........................................................................*/
void ADCSeq3_IRQHandler(void) {
    static uint32_t adcLPS = 0U;           /* Low-Pass-Filtered ADC reading */
    static uint32_t wheel = 0U;                  /* the last wheel position */

    static uint32_t btn_debounced  = 0U;
    static uint8_t  debounce_state = 0U;

    uint32_t tmp;

    ADC->ISC = (1U << 3);                    /* clear the ADCSeq3 interrupt */
                              /* the ADC Sequence 3 FIFO must have a sample */
    Q_ASSERT((ADC->SSFSTAT3 & ADC_SSFSTAT0_EMPTY) == 0U);
    tmp = ADC->SSFIFO3;                       /* read the data from the ADC */

    /* 1st order low-pass filter: time constant ~= 2^n samples
    * TF = (1/2^n)/(z-((2^n - 1)/2^n)),
    * eg, n=3, y(k+1) = y(k) - y(k)/8 + x(k)/8 => y += (x - y)/8
    */
    adcLPS += (((int)tmp - (int)adcLPS + 4) >> 3);       /* Low-Pass-Filter */

    /* compute the next position of the wheel */
    tmp = (((1U << 10) - adcLPS)*(BSP_SCREEN_HEIGHT - 2)) >> 10;

    if (tmp != wheel) {                   /* did the wheel position change? */
        QActive_postISR((QActive *)&AO_Ship, PLAYER_SHIP_MOVE_SIG,
                        ((tmp << 8) | GAME_SHIP_X));
        wheel = tmp;                 /* save the last position of the wheel */
    }

    tmp = GPIOC->DATA_Bits[PUSH_BUTTON];      /* read the push button state */
    switch (debounce_state) {
        case 0:
            if (tmp != btn_debounced) {
                debounce_state = 1;         /* transition to the next state */
            }
            break;
        case 1:
            if (tmp != btn_debounced) {
                debounce_state = 2;         /* transition to the next state */
            }
            else {
                debounce_state = 0;           /* transition back to state 0 */
            }
            break;
        case 2:
            if (tmp != btn_debounced) {
                debounce_state = 3;         /* transition to the next state */
            }
            else {
                debounce_state = 0;           /* transition back to state 0 */
            }
            break;
        case 3:
            if (tmp != btn_debounced) {
                btn_debounced = tmp;     /* save the debounced button value */

                if (tmp == 0) {                 /* is the button depressed? */
                    QActive_postISR((QActive *)&AO_Ship,
                                    PLAYER_TRIGGER_SIG, 0);
                    QActive_postISR((QActive *)&AO_Tunnel,
                                    PLAYER_TRIGGER_SIG, 0);
                }
            }
            debounce_state = 0;               /* transition back to state 0 */
            break;
    }
}
/*..........................................................................*/
void GPIOPortA_IRQHandler(void) {
    QActive_postISR((QActive *)&AO_Tunnel, TAKE_OFF_SIG, 0); /* for testing */
}

/*..........................................................................*/
void BSP_init(void) {
    SystemInit();                         /* initialize the system clock(s) */

    /* enable clock to the peripherals used by the application */
    SYSCTL->RCGC0 |= (1 << 16);               /* enable clock to ADC        */
    SYSCTL->RCGC1 |= (1 << 16) | (1 << 17);   /* enable clock to TIMER0 & 1 */
    SYSCTL->RCGC2 |= (1 <<  0) | (1 <<  2);   /* enable clock to GPIOA & C  */

    asm(" MOV R0,R0");                        /* wait after enabling clocks */
    asm(" MOV R0,R0");
    asm(" MOV R0,R0");

    /* Configure the ADC Sequence 3 to sample the potentiometer when the
    * timer expires. Set the sequence priority to 0 (highest).
    */
    ADC->EMUX   = (ADC->EMUX   & ~(0xF << (3*4)))
                  | (ADC_TRIGGER_TIMER << (3*4));
    ADC->SSPRI  = (ADC->SSPRI  & ~(0xF << (3*4)))
                  | (0 << (3*4));
    /* set ADC Sequence 3 step to 0 */
    ADC->SSMUX3 = (ADC->SSMUX3 & ~(0xF << (0*4)))
                  | ((ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END) << (0*4));
    ADC->SSCTL3 = (ADC->SSCTL3 & ~(0xF << (0*4)))
                  | (((ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END) >> 4) <<(0*4));
    ADC->ACTSS |= (1 << 3);

    /* configure TIMER1 to trigger the ADC to sample the potentiometer. */
    TIMER1->CTL  &= ~((1 << 0) | (1 << 16));
    TIMER1->CFG   = 0;
    TIMER1->TAMR  = 0x02;
    TIMER1->TAILR = SystemFrequency / 120;
    TIMER1->CTL  |= 0x02;
    TIMER1->CTL  |= 0x20;

    /* configure the LED and push button */
    GPIOC->DIR |= USER_LED;                        /* set direction: output */
    GPIOC->DEN |= USER_LED;                               /* digital enable */
    GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */

    GPIOC->DIR &= ~PUSH_BUTTON;                    /*  set direction: input */
    GPIOC->DEN |= PUSH_BUTTON;                            /* digital enable */

    Display96x16x1Init(1);                   /* initialize the OLED display */
}
/*..........................................................................*/
void BSP_drawBitmap(uint8_t const *bitmap) {
    Display96x16x1ImageDraw(bitmap, 0U, 0U,
                            BSP_SCREEN_WIDTH, (BSP_SCREEN_HEIGHT >> 3));
}
/*..........................................................................*/
void BSP_drawNString(uint8_t x, uint8_t y, char const *str) {
    Display96x16x1StringDraw(str, x, y);
}
/*..........................................................................*/
void BSP_updateScore(uint16_t score) {
    /* no room on the OLED display of the EV-LM3S811 board for the score */
}
/*..........................................................................*/
void BSP_displayOn(void) {
    Display96x16x1DisplayOn();
}
/*..........................................................................*/
void BSP_displayOff(void) {
    Display96x16x1DisplayOff();
}

/*..........................................................................*/
void QF_onStartup(void) {               /* enable the configured interrupts */
              /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemFrequency / BSP_TICKS_PER_SEC);

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(ADCSeq3_IRQn,   ADCSEQ3_PRIO);
    NVIC_SetPriority(GPIOPortA_IRQn, GPIOPORTA_PRIO);

    NVIC_EnableIRQ(ADCSeq3_IRQn);
    NVIC_EnableIRQ(GPIOPortA_IRQn);

    ADC->ISC = (1 << 3);
    ADC->IM |= (1 << 3);

    TIMER1->CTL |= ((1 << 0) | (1 << 16));                 /* enable TIMER1 */
}
/*..........................................................................*/
void QF_stop(void) {
}
/*..........................................................................*/
void QF_onIdle(void) {      /* entered with interrupts DISABLED, see NOTE01 */

    /* toggle the User LED on and then off, see NOTE02 */
    GPIOC->DATA_Bits[USER_LED] = USER_LED;         /* turn the User LED on  */
    GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M3 MCU.
    */
    QF_CPU_SLEEP();         /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE();                              /* just enable interrupts */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}
/*..........................................................................*/
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char_t const *file, int_t line) {
    Q_onAssert(file, line);
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
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*
*/

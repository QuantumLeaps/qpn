/*****************************************************************************
* Product: PELICAN crossing example, cooperative "vanilla" kerenel
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Aug 16, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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
#include "bsp.h"
#include "pelican.h"

#include "lm3s_cmsis.h"
#include "display96x16x1.h"

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    GPIOPORTA_PRIO,
    SYSTICK_PRIO,
    /* ... */
};

            /* The Push Button pin and User LED pin on the EK-LM3S811 board */
#define PUSH_BUTTON             (1U << 4)
#define USER_LED                (1U << 5)

/* ISR prototypes */
void SysTick_Handler(void);
void GPIOPortA_IRQHandler(void);


/*..........................................................................*/
void SysTick_Handler(void) {
    static uint32_t btn_debounced  = PUSH_BUTTON;
    static uint8_t  debounce_state = 0U;
    uint32_t btn;

    QF_tickISR();                          /* process all armed time events */

    btn = GPIOC->DATA_Bits[PUSH_BUTTON];               /* read the push btn */
    switch (debounce_state) {
        case 0:
            if (btn != btn_debounced) {
                debounce_state = 1U;        /* transition to the next state */
            }
            break;
        case 1:
            if (btn != btn_debounced) {
                debounce_state = 2U;        /* transition to the next state */
            }
            else {
                debounce_state = 0U;          /* transition back to state 0 */
            }
            break;
        case 2:
            if (btn != btn_debounced) {
                debounce_state = 3U;        /* transition to the next state */
            }
            else {
                debounce_state = 0U;          /* transition back to state 0 */
            }
            break;
        case 3:
            if (btn != btn_debounced) {
                btn_debounced = btn;     /* save the debounced button value */

                if (btn == 0U) {                /* is the button depressed? */
                    QActive_postISR((QActive *)&AO_Pelican,
                                    PEDS_WAITING_SIG, 0U);
                }
                else {                                   /* button released */
                }
            }
            debounce_state = 0U;              /* transition back to state 0 */
            break;
    }
}
/*..........................................................................*/
void GPIOPortA_IRQHandler(void) {
    QActive_postISR((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0);/* testing */
}
/*..........................................................................*/
void QF_onStartup(void) {
              /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemFrequency / BSP_TICKS_PER_SEC);

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(GPIOPortA_IRQn, GPIOPORTA_PRIO);

    NVIC_EnableIRQ(GPIOPortA_IRQn);                 /* IRQ used for testing */
}
/*..........................................................................*/
void QF_onIdle(void) {      /* entered with interrupts DISABLED, see NOTE01 */

    /* toggle the User LED on and then off, see NOTE02 */
    GPIOC->DATA_Bits[USER_LED] = USER_LED;         /* turn the User LED on  */
    GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */

#ifdef NDEBUG
    /* put the CPU and peripherals to the low-power mode, see NOTE02 */
    __WFI();
#endif
    QF_INT_ENABLE();                            /* always enable interrupts */
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
void assert_failed(char_t const *file, int_t line);            /* prototype */
void assert_failed(char_t const *file, int_t line) {
    Q_onAssert(file, line);
}

/*..........................................................................*/
void BSP_init(void) {
    SystemInit();                         /* initialize the system clocking */

                      /* enable the peripherals used by this application... */
    SYSCTL->RCGC2 |= (1 <<  2);                   /* enable clock to GPIOC  */
    SYSCTL->RCGC1 |= (1 << 16);                   /* enable clock to TIMER0 */
    __NOP();                                  /* wait after enabling clocks */
    __NOP();
    __NOP();
                                  /* configure the User LED (PortC pin5)... */
    GPIOC->DIR |= USER_LED;                        /* set direction: output */
    GPIOC->DEN |= USER_LED;                               /* digital enable */
    GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */

    Display96x16x1Init(1);                   /* initialize the OLED display */
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
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
        case CARS_BLANK: {
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
void BSP_showState(char_t const *state) {
    Display96x16x1StringDraw(state, 0, 0);
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
* Please note that the LED is toggled with interrupts disabled, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

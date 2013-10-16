/*****************************************************************************
* Product: PELICAN crossing example, preemptive QK-nano kerenel
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
#include "pelican.h"
#include "bsp.h"

#include "tm4c_cmsis.h"
#include "sysctl.h"
#include "gpio.h"
#include "rom.h"

//Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelUnawareISRs {                                      /* see NOTE00 */
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI                        /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    GPIOPORTA_PRIO = QF_AWARE_ISR_CMSIS_PRI,                  /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI                          /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/* ISRs defined in this BSP ------------------------------------------------*/
void SysTick_Handler(void);
void GPIOPortA_IRQHandler(void);

/* Local-scope objects -----------------------------------------------------*/
#define LED_RED     (1U << 1)
#define LED_GREEN   (1U << 3)
#define LED_BLUE    (1U << 2)

#define USR_SW1     (1U << 4)
#define USR_SW2     (1U << 0)

/*..........................................................................*/
void SysTick_Handler(void) {
    static uint32_t btn_debounced  = USR_SW1;
    static uint8_t  debounce_state = 0U;
    uint32_t btn;

    QK_ISR_ENTRY();                      /* infrom QK about entering an ISR */

    QF_tickISR();                          /* process all armed time events */

                                             /* debounce the USER button... */
    btn = GPIOF->DATA_Bits[USR_SW1];                   /* read the User Sw1 */
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

    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}
/*..........................................................................*/
void GPIOPortA_IRQHandler(void) {
    QK_ISR_ENTRY();                /* inform QK-nano about entering the ISR */
    QActive_postISR((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0);/* testing */
    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
}
/*..........................................................................*/
void BSP_init(void) {
                                          /* Enable the floating-point unit */
    SCB->CPACR |= (0xFU << 20);

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPULazyStackingEnable();

    // Set the clocking to run directly from the crystal
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC
                       | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /* enable clock to the peripherals used by the application */
    SYSCTL->RCGC2 |= (1U << 5);                   /* enable clock to GPIOF  */
    __NOP();                                  /* wait after enabling clocks */
    __NOP();
    __NOP();

    /* configure the LEDs */
    GPIOF->DIR |= (LED_RED | LED_GREEN | LED_BLUE);/* set direction: output */
    GPIOF->DEN |= (LED_RED | LED_GREEN | LED_BLUE);       /* digital enable */
    GPIOF->DATA_Bits[LED_RED]   = 0U;                   /* turn the LED off */
    GPIOF->DATA_Bits[LED_GREEN] = 0U;                   /* turn the LED off */
    GPIOF->DATA_Bits[LED_BLUE]  = 0U;                   /* turn the LED off */

    /* configure the User Switches */
    GPIOF->DIR &= ~(USR_SW1 | USR_SW2);            /*  set direction: input */
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, (USR_SW1 | USR_SW2),
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    /* test code to use the FPU ... */
    float volatile x = 3.1415926F;
    x = x + 2.7182818F;

    (void)state;

}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            //GPIOF->DATA_Bits[LED_RED] = LED_RED;
            break;
        }
        case CARS_YELLOW: {
            //GPIOF->DATA_Bits[LED_RED | LED_GREEN] = (LED_RED | LED_GREEN);
            break;
        }
        case CARS_GREEN: {
            //GPIOF->DATA_Bits[LED_GREEN] = LED_GREEN;
            break;
        }
        case CARS_BLANK: {
            //GPIOF->DATA_Bits[LED_RED | LED_GREEN | LED_BLUE] = 0U;
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            GPIOF->DATA_Bits[LED_RED] = LED_RED;
            break;
        }
        case PEDS_BLANK: {
            GPIOF->DATA_Bits[LED_RED | LED_GREEN] = 0U;
            break;
        }
        case PEDS_WALK: {
            GPIOF->DATA_Bits[LED_RED | LED_GREEN] = LED_GREEN;
            break;
        }
    }
}

/*..........................................................................*/
void QF_onStartup(void) {
              /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(ROM_SysCtlClockGet() / BSP_TICKS_PER_SEC);

    /* assing all priority bits for preemption-prio. and none to sub-prio. */
    NVIC_SetPriorityGrouping(0U);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(GPIOPortA_IRQn, GPIOPORTA_PRIO);
    /* ... */
                                                          /* enable IRQs... */
    NVIC_EnableIRQ(GPIOPortA_IRQn);
}
/*..........................................................................*/
void QK_onIdle(void) {
    /* test code to use the FPU ... */
    float volatile x = 3.1415926F;
    x = x + 2.7182818F;

    /* toggle the Blue LED on and then off, see NOTE01 */
    QF_INT_DISABLE();
    GPIOF->DATA_Bits[LED_BLUE] = LED_BLUE;         /* turn the Blue LED on  */
    GPIOF->DATA_Bits[LED_BLUE] = 0;                /* turn the Blue LED off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    __WFI();                                          /* Wait-For-Interrupt */
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


/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
* are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The Blue LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invocations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the Blue LED.
*/

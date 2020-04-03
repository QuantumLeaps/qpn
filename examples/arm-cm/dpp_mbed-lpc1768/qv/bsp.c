/*****************************************************************************
* Product: DPP on EK-TM4C123GXL board, coopearative QV kernel
* Last Updated for Version: 5.5.1
* Date of the Last Update:  2015-10-05
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"
#include "dpp.h"
#include "bsp.h"

#include "LPC17xx.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelUnawareISRs { /* see NOTE00 */
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI  /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    EINT0_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/* ISRs defined in this BSP ------------------------------------------------*/
void SysTick_Handler(void);
void EINT0_IRQHandler(void);

/* Local-scope objects -----------------------------------------------------*/
/* LEDs available on the board */
#define LED_1    (1U << 18)  /* P1.18 */
#define LED_2    (1U << 20)  /* P1.20 */
#define LED_3    (1U << 21)  /* P1.21 */
#define LED_4    (1U << 23)  /* P1.23 */

/* Push-Button wired externally to DIP8 (P0.6) */
#define BTN_EXT  (1U << 6)    /* P0.6 */

/* random seed */
static uint32_t l_rnd;


/* ISRs used in this project ===============================================*/
void SysTick_Handler(void) {
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { ~0U, ~0U };
    uint32_t current;
    uint32_t tmp;

    QF_tickXISR(0U); /* process time events for rate 0 */


    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = ~LPC_GPIO0->FIOPIN; /* read P0 with the state of the Buttons */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & BTN_EXT) != 0U) {  /* debounced BTN_EXT state changed? */
        if ((buttons.depressed & BTN_EXT) != 0U) { /* is BTN_EXT depressed? */
            QACTIVE_POST_ISR(&AO_Table, PAUSE_SIG, 0U);
        }
        else {            /* the button is released */
            QACTIVE_POST_ISR(&AO_Table, SERVE_SIG, 0U);
        }
    }
}
/*..........................................................................*/
void EINT0_IRQHandler(void) {
    QACTIVE_POST_ISR(&AO_Table, MAX_SIG, 0U); /* for testing */
}


/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* NOTE: SystemInit() already called from the startup code
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

    /* turn the GPIO clock on */
    LPC_SC->PCONP |= (1U << 15);

    /* setup the GPIO pin functions for the LEDs... */
    LPC_PINCON->PINSEL3 &= ~(3U <<  4); /* LED_1: function P1.18 to GPIO */
    LPC_PINCON->PINSEL3 &= ~(3U <<  8); /* LED_2: function P1.20 to GPIO */
    LPC_PINCON->PINSEL3 &= ~(3U << 10); /* LED_3: function P1.21 to GPIO */
    LPC_PINCON->PINSEL3 &= ~(3U << 14); /* LED_4: function P1.23 to GPIO */

    /* Set GPIO-P1 LED pins to output */
    LPC_GPIO1->FIODIR |= (LED_1 | LED_2 | LED_3 | LED_4);


    /* setup the GPIO pin function for the Button... */
    LPC_PINCON->PINSEL0 &= ~(3U << 12); /* function P0.6 to GPIO, pull-up */

    /* Set GPIO-P0 Button pin as input */
    LPC_GPIO0->FIODIR &= ~BTN_EXT;

    BSP_randomSeed(1234U);
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
    if (stat[0] == 'h') {
        LPC_GPIO1->FIOSET = LED_1;  /* turn LED on  */
    }
    else {
        LPC_GPIO1->FIOCLR = LED_1;  /* turn LED off */
    }
    if (stat[0] == 'e') {
        LPC_GPIO1->FIOSET = LED_2;  /* turn LED on  */
    }
    else {
        LPC_GPIO1->FIOCLR = LED_2;  /* turn LED off */
    }
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    if (paused != (uint8_t)0) {
        LPC_GPIO1->FIOSET = LED_3;  /* turn LED on  */
    }
    else {
        LPC_GPIO1->FIOCLR = LED_3;  /* turn LED off */
    }
}
/*..........................................................................*/
uint32_t BSP_random(void) { /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);

    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}


/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* assing all priority bits for preemption-prio. and none to sub-prio. */
    NVIC_SetPriorityGrouping(0U);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(EINT0_IRQn,     EINT0_PRIO);
    /* ... */

    /* enable IRQs in the NVIC... */
    NVIC_EnableIRQ(EINT0_IRQn);
}
/*..........................................................................*/
void QV_onIdle(void) { /* called with interrupts disabled, see NOTE01 */

    /* toggle the User LED on and then off, see NOTE01 */
    LPC_GPIO1->FIOSET = LED_4;  /* turn LED on  */
    __NOP();   /* a couple of NOPs to actually see the LED glow */
    __NOP();
    __NOP();
    __NOP();
    LPC_GPIO1->FIOCLR = LED_4;  /* turn LED off */

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */
#endif
}

/*..........................................................................*/
Q_NORETURN Q_onAssert(char const Q_ROM * const module, int loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)loc;

    NVIC_SystemReset();
}

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/QK_ISR_ENTRY
* macros or any other QF/QK  services. These ISRs are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF/QK services. In particular they
* can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The QV_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QV_onIdle() must internally enable interrupts, ideally
* atomically with putting the CPU to the power-saving mode.
*
* NOTE02:
* The User LED is used to visualize the idle loop activity. The brightness
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

/*****************************************************************************
* Product: DPP on STM32 NUCLEO-L053R8 board, preemptive QK kernel
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* http://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpn.h"
#include "dpp.h"
#include "bsp.h"

#include "stm32l0xx.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelAwareISRs {
    GPIOPORTA_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/* ISRs defined in this BSP ------------------------------------------------*/
void SysTick_Handler(void);

/* Local-scope objects -----------------------------------------------------*/
/* LED pins available on the board (just one user LED LD2--Green on PA.5) */
#define LED_LD2  (1U << 5)

/* Button pins available on the board (just one user Button B1 on PC.13) */
#define BTN_B1   (1U << 13)

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

    QK_ISR_ENTRY();  /* inform QK about entering an ISR */

    QF_tickXISR(0U); /* process time events for rate 0 */


    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = ~GPIOC->IDR; /* read Port C with the state of Button B1 */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & BTN_B1) != 0U) {  /* debounced B1 state changed? */
        if ((buttons.depressed & BTN_B1) != 0U) { /* is B1 depressed? */
            QACTIVE_POST_ISR(&AO_Table, PAUSE_SIG, 0U);
        }
        else {            /* the button is released */
            QACTIVE_POST_ISR(&AO_Table, SERVE_SIG, 0U);
        }
    }

    QK_ISR_EXIT();  /* inform QK about exiting an ISR */
}


/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* NOTE: SystemInit() has been already called from the startup code
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

    /* enable GPIOA clock port for the LED LD2 */
    RCC->IOPENR |= (1U << 0);

    /* configure LED (PA.5) pin as push-pull output, no pull-up, pull-down */
    GPIOA->MODER   &= ~((3U << 2*5));
    GPIOA->MODER   |=  ((1U << 2*5));
    GPIOA->OTYPER  &= ~((1U <<   5));
    GPIOA->OSPEEDR &= ~((3U << 2*5));
    GPIOA->OSPEEDR |=  ((1U << 2*5));
    GPIOA->PUPDR   &= ~((3U << 2*5));

    /* enable GPIOC clock port for the Button B1 */
    RCC->IOPENR |=  (1U << 2);

    /* configure Button (PC.13) pins as input, no pull-up, pull-down */
    GPIOC->MODER   &= ~(3U << 2*13);
    GPIOC->OSPEEDR &= ~(3U << 2*13);
    GPIOC->OSPEEDR |=  (1U << 2*13);
    GPIOC->PUPDR   &= ~(3U << 2*13);

    BSP_randomSeed(1234U); /* seed the random number generator */
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char const *stat) {
    if (stat[0] == 'h') {
        GPIOA->BSRR |= LED_LD2;  /* turn LED on  */
    }
    else {
        GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED off */
    }
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    /* not enough LEDs to implement this feature */
    if (paused != (uint8_t)0) {
        //GPIOA->BSRR |= (LED_LD2);  /* turn LED[n] on  */
    }
    else {
        //GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED[n] off */
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

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    /* ... */

    /* enable IRQs... */
}
/*..........................................................................*/
void QK_onIdle(void) {

    /* toggle an LED on and then off (not enough LEDs, see NOTE01) */
    QF_INT_DISABLE();
    //GPIOA->BSRR |= (LED_LD2);        /* turn LED[n] on  */
    //GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED[n] off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M3 MCU.
    */
    /* !!!CAUTION!!!
    * The WFI instruction stops the CPU clock, which unfortunately disables
    * the JTAG port, so the ST-Link debugger can no longer connect to the
    * board. For that reason, the call to __WFI() has to be used with CAUTION.
    *
    * NOTE: If you find your board "frozen" like this, strap BOOT0 to VDD and
    * reset the board, then connect with ST-Link Utilities and erase the part.
    * The trick with BOOT(0) is it gets the part to run the System Loader
    * instead of your broken code. When done disconnect BOOT0, and start over.
    */
    //__WFI(); /* Wait-For-Interrupt */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR module, int loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)loc;

#ifndef NDEBUG
    GPIOA->BSRR |= LED_LD2;  /* turn LED on  */
    for (;;) {
    }
#endif

    NVIC_SystemReset();
}

/*****************************************************************************
* NOTE01:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/

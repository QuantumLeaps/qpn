/*****************************************************************************
* Product: PELICAN crossing example, M16C/Neutrino board, QK-nano kernel
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 02, 2012
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
#include "pelican.h"
#include "bsp.h"

/*..........................................................................*/
uint8_t l_led_mask = 0x00;    /* the current state of LEDs for HEW debugger */
uint8_t l_cars_sig = 0xFF;  /* the current signal for Cars for HEW debugger */
uint8_t l_peds_sig = 0xFF;  /* the current signal for Peds for HEW debugger */

enum LedBits {
    RED_LED_BIT = (1 << 0),
    AMB_LED_BIT = (1 << 1),
    GRN_LED_BIT = (1 << 2),
    ORG_LED_BIT = (1 << 3)
};

                 /* priority of the time-tick ISR, (1-lowest ... 7-highest) */
#define TICK_ISR_PRIO       1

/*..........................................................................*/
#pragma INTERRUPT ta0_isr (vect = 21)              /* system clock tick ISR */
void ta0_isr(void) {
    static uint8_t btn_debounced  = 0;
    static uint8_t debounce_state = 0;
    uint8_t btn;

    QK_ISR_ENTRY();                 /* inform QK-nano about entering in ISR */

    QF_tick();                             /* process all armed time events */

    btn = SW1;                                  /* read the user switch SW1 */
    switch (debounce_state) {
        case 0:
            if (btn != btn_debounced) {
                debounce_state = 1;         /* transition to the next state */
            }
            break;
        case 1:
            if (btn != btn_debounced) {
                debounce_state = 2;         /* transition to the next state */
            }
            else {
                debounce_state = 0;           /* transition back to state 0 */
            }
            break;
        case 2:
            if (btn != btn_debounced) {
                btn_debounced = btn;     /* save the debounced button value */

                if (btn == 0) {                 /* is the button depressed? */
                    QActive_postISR((QActive *)&AO_Pelican,
                                    PEDS_WAITING_SIG, 0);
                }
            }
            debounce_state = 0;               /* transition back to state 0 */
            break;
    }

    QK_ISR_EXIT();                   /* inform QK-nano about exiting in ISR */
}
/*..........................................................................*/
void BSP_init(void) {
    uint16_t volatile delay;
                                                 /* initialize the clock... */
    prc1 = 1;                  /* enable access to processor mode registers */
    pm20 = 0;       /* 2 wait states for SFR access for >16MHz operation... */
    prc1 = 0;                 /* disable access to processor mode registers */

                      /* configure and switch main clock to PLL at 20MHz... */
    prc0 = 1;                           /* enable access to clock registers */
    cm1  = 0x20;                        /* set to hi-drive Xin, divide by 1 */
    cm0  = 0x08;                     /* set to main clock using divide by 1 */
    cm21 = 0;                                              /* switch to Xin */
    plc0 = 0x12;                                     /* configure PLL to x4 */
    plc0 = 0x92;                                              /* enable PLL */
    for (delay = 20000; delay != 0; --delay) {       /* wait for stable PLL */
    }
    cm11 = 1;                                              /* switch to PLL */
    prc0 = 0;                          /* disable access to clock registers */


                                            /* general device configuration */
    prcr = 0x04;                    /* enables next instruction to set pacr */
    pacr = 0x04;                                      /* set to 48 pin chip */

                                                  /* enable the User Button */
    SW1_DDR = 0;

                                               /* LED port configuration... */
    LED0_DDR = 1;
    LED1_DDR = 1;
    LED2_DDR = 1;
    LED3_DDR = 1;
    LED0     = LED_OFF;
    LED1     = LED_OFF;
    LED2     = LED_OFF;
    LED3     = LED_OFF;

                                /* configure 32kHz sub clock..., see NOTE02 */
    pd8_7 = 0;                           /* set GPIO to inputs (XCin/XCout) */
    pd8_6 = 0;
    prc0 = 1;                                         /* unlock CM0 and CM1 */
    cm04 = 1;                                    /* start the 32KHz crystal */
    prc0 = 0;                     /* lock the System Clock Control Register */

                                      /* setup Timer A running from fc32... */
    ta0mr = 0xC0;                      /* Timer mode, fc32, no pulse output */
    ta0   = (int)((fc_CLK_SPEED/32 + BSP_TICKS_PER_SEC/2)
                  / BSP_TICKS_PER_SEC) - 1;                       /* period */
    ta0ic = TICK_ISR_PRIO;   /* set the clock tick interrupt priority level */
}
/*..........................................................................*/
void QF_onStartup(void) {
    ta0s = 1;                                             /* Start timer A0 */
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QK_onIdle(void) {
#ifdef NDEBUG                   /* low-power mode interferes with debugging */
    /* stop all peripheral clocks that you can in your applicaiton ... */
    _asm("WAIT");          /* execute before entering any pending interrupt */
#endif
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    l_cars_sig = (uint8_t)sig;       /* save in the global for HEW debugger */

    switch (sig) {
        case CARS_OFF:
            l_led_mask &= ~(RED_LED_BIT | AMB_LED_BIT | GRN_LED_BIT);
            break;
        case CARS_RED:
            l_led_mask |= RED_LED_BIT;
            l_led_mask &= ~(AMB_LED_BIT | GRN_LED_BIT);
            break;
        case CARS_YELLOW:
            l_led_mask |= AMB_LED_BIT;
            l_led_mask &= ~(RED_LED_BIT | GRN_LED_BIT);
            break;
        case CARS_GREEN:
            l_led_mask |= GRN_LED_BIT;
            l_led_mask &= ~(RED_LED_BIT | AMB_LED_BIT);
            break;
    }
    LED0 = (((l_led_mask & RED_LED_BIT) != 0) ? LED_ON : LED_OFF);
    LED1 = (((l_led_mask & AMB_LED_BIT) != 0) ? LED_ON : LED_OFF);
    LED2 = (((l_led_mask & GRN_LED_BIT) != 0) ? LED_ON : LED_OFF);
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    l_peds_sig = (uint8_t)sig;       /* save in the global for HEW debugger */
    switch (sig) {
        case PEDS_DONT_WALK:
            l_led_mask |= ORG_LED_BIT;
            break;
        case PEDS_BLANK:
            l_led_mask &= ~ORG_LED_BIT;
            break;
        case PEDS_WALK:
            l_led_mask &= ~ORG_LED_BIT;
            break;
    }
    LED3 = (((l_led_mask & ORG_LED_BIT) != 0) ? LED_ON : LED_OFF);
}
/*..........................................................................*/
void BSP_showState(uint8_t prio, char const *state) {
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();                                 /* disable interrupts */
    for (;;) {                                /* hang in this for-ever loop */
    }
}

/*****************************************************************************
* NOTE01:
* The R8C microcontroller supports interrupt prioritization. Therefore it
* is safe to unlock interrupts inside ISRs. By assigning priorities to
* interrupts you have full control over interrupt nesting. In particular,
* you can avoid interrupt nesting by assigning the same priority level to
* all interrupts.
*
* The simple policy of unconditional unlocking of interrupts upon exit from
* a critical section precludes nesting of critical sections. This policy
* means that you *must* unlock interrupts inside every ISR before invoking
* any QF service, such as QActvie_postFIFO(), QF_publish(), or QF_tick().
* The macros QK_ISR_ENTRY() and QK_ISR_EXIT() unlock and lock interrupts,
* respectively.
*/
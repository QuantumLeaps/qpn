/*****************************************************************************
* Product: PELICAN crossing example, H8/Quark board
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

#include "quarkdef.h"                            /* Quark board definitions */

uint8_t l_led_mask = 0x00;    /* the current state of LEDs for HEW debugger */
uint8_t l_swi_mask = 0x00;/* the current state of Switches for HEW debugger */
uint8_t l_cars_sig = 0xFF;  /* the current signal for Cars for HEW debugger */
uint8_t l_peds_sig = 0xFF;  /* the current signal for Peds for HEW debugger */

enum LedBits {
    RED_LED_BIT = (1 << 0),
    AMB_LED_BIT = (1 << 1),
    GRN_LED_BIT = (1 << 2),
    ORG_LED_BIT = (1 << 3)
};

/*..........................................................................*/
__interrupt(vect = 26)
void timer_z_isr(void) {
    static uint8_t btn_debounced  = 0;
    static uint8_t debounce_state = 0;
    uint8_t btn;

    TZ0.TSR.BIT.IMFA = 0;                       /* clear compare match flag */

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
                    QActive_post((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0);
                }
            }
            debounce_state = 0;               /* transition back to state 0 */
            break;
    }
}
/*..........................................................................*/
void BSP_init(void) {
    WDT.TCSRWD.BYTE = 0x10;                             /* disable Watchdog */
    WDT.TCSRWD.BYTE = 0x00;

    MSTCR2.BIT.MSTTZ   = 0;                               /* turn on TimerZ */
    TZ0.TCR.BIT.TPSC   = 3;                         /* internal clock phi/8 */
    TZ0.TCR.BIT.CCLR   = 1;
    TZ0.GRA            = (uint16_t)((f1_CLK_SPEED/8 + BSP_TICKS_PER_SEC/2)
                                    / BSP_TICKS_PER_SEC);
    TZ0.TIER.BIT.IMIEA = 1;               /* compare match interrupt enable */

                                                 /* enable the User LEDs... */
    LED0_DDR_1();                           /* configure LED0 pin as output */
    LED1_DDR_1();                           /* configure LED1 pin as output */
    LED2_DDR_1();                           /* configure LED2 pin as output */
    LED3_DDR_1();                           /* configure LED3 pin as output */
    LED0 = LED_OFF;
    LED1 = LED_OFF;
    LED2 = LED_OFF;
    LED3 = LED_OFF;
                                                    /* enable the Switch... */
    SW1_DDR = 0;
}
/*..........................................................................*/
void QF_onStartup(void) {
    TZ.TSTR.BIT.STR0 = 1;                                   /* start TimerZ */
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QF_onIdle(void) {
#ifdef NDEBUG                   /* low-power mode interferes with debugging */
    /* stop all peripheral clocks that you can in your applicaiton ... */
    set_imask_ccr(0);       /* the following SLEEP instruction will execute */
    sleep();          /*  before entering any pending interrupt, see NOTE01 */
#else
    QF_INT_ENABLE();                              /* just enable interrupts */
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
* The non-preemptive QF port (foreground/background) requires an atomic
* transiton to the low-power WAIT mode of the M16C MCU. As described in
* "M16C Software Manual", Section 5.2.1 "Interrupt Enable Flag", the
* instruction immediately following "FSET I" instruction, will be executed
* before any pending interrupt. This guarantees atomic transition to the
* WAIT mode. CAUTION: The instruction pair (FSET I, WAIT) should never be
* separated by any other instruction.
*/

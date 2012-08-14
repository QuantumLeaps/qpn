/*****************************************************************************
* Product: PELICAN crossing example, H8/SKP36077 board, QK-nano kenrel
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

#include "skp36077def.h"                      /* SKP36077 board definitions */

/*..........................................................................*/
__interrupt(vect = 26)
void timer_z_isr(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    TZ0.TSR.BIT.IMFA = 0;                       /* clear compare match flag */
    QF_tick();
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void BSP_init(void) {
    WDT.TCSRWD.BYTE = 0x10;                             /* disable Watchdog */
    WDT.TCSRWD.BYTE = 0x00;

    LED1_DDR_1();                           /* configure LED1 pin as output */
    LED2_DDR_1();                           /* configure LED2 pin as output */
    LED1 = LED_OFF;
    LED2 = LED_OFF;

    MSTCR2.BIT.MSTTZ   = 0;                               /* turn on TimerZ */
    TZ0.TCR.BIT.TPSC   = 3;                         /* internal clock phi/8 */
    TZ0.TCR.BIT.CCLR   = 1;
    TZ0.GRA            = (uint16_t)((f1_CLK_SPEED/8 + BSP_TICKS_PER_SEC/2)
                                    / BSP_TICKS_PER_SEC);
    TZ0.TIER.BIT.IMIEA = 1;               /* compare match interrupt enable */
}
/*..........................................................................*/
void QF_onStartup(void) {
    TZ.TSTR.BIT.STR0 = 1;                                   /* start TimerZ */
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QK_init(void) {
}
/*..........................................................................*/
void QK_onIdle(void) {
#ifdef NDEBUG                   /* low-power mode interferes with debugging */
    /* stop all peripheral clocks that you can in your applicaiton ... */
    sleep();               /* execute before entering any pending interrupt */
#endif
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_OFF:
            LED1 = LED_OFF;
            break;
        case CARS_RED:
            LED1 = LED_ON;
            break;
        case CARS_YELLOW:
            LED1 = LED_OFF;
            break;
        case CARS_GREEN:
            LED1 = LED_OFF;
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK:
            LED2 = LED_ON;
            break;
        case PEDS_BLANK:
            LED2 = LED_OFF;
            break;
        case PEDS_WALK:
            LED2 = LED_OFF;
            break;
    }
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

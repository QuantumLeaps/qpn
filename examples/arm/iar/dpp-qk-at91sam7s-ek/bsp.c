/*****************************************************************************
* Product: Board Support Package for AT91SAM7S-EK, with QK-nano
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
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
#include "dpp.h"
#include "bsp.h"

/*Q_DEFINE_THIS_FILE*/

#pragma diag_suppress=Ta021  /* call __iar_disable_interrupt from __ramfunc */
#pragma diag_suppress=Ta022     /* possible ROM access <ptr> from __ramfunc */
#pragma diag_suppress=Ta023         /* call to non __ramfunc from __ramfunc */

/* Local objects -----------------------------------------------------------*/
uint32_t const l_led[] = {
    (1 << 0),                                     /* LED D1 on AT91SAM7S-EK */
    (1 << 1),                                     /* LED D2 on AT91SAM7S-EK */
    (1 << 2),                                     /* LED D3 on AT91SAM7S-EK */
    (1 << 3)                                      /* LED D4 on AT91SAM7S-EK */
};

#define LED_ON(num_)       (AT91C_BASE_PIOA->PIO_CODR = l_led[num_])
#define LED_OFF(num_)      (AT91C_BASE_PIOA->PIO_SODR = l_led[num_])

typedef void (*IntVector)(void);           /* IntVector pointer-to-function */


/*..........................................................................*/
__arm __ramfunc void BSP_irq(void) {
    IntVector vect = (IntVector)AT91C_BASE_AIC->AIC_IVR;    /* read the IVR */
    QF_INT_ENABLE_32();                         /* allow nesting interrupts */
    (*vect)();              /* call the IRQ ISR via the pointer to function */
    QF_INT_DISABLE_32();            /* lock interrups for the exit sequence */
    AT91C_BASE_AIC->AIC_EOICR = 0;    /* write AIC_EOICR to clear interrupt */
}
/*..........................................................................*/
__arm __ramfunc void BSP_fiq(void) {
    /* TBD: implement the FIQ handler directly right here, see NOTE02  */
    /* NOTE: Do NOT enable interrupts throughout the whole FIQ processing. */
    /* NOTE: Do NOT write EOI to the AIC */
}
/* ISRs --------------------------------------------------------------------*/
static __arm __ramfunc void ISR_tick(void) {
    uint32_t tmp = AT91C_BASE_PITC->PITC_PIVR;    /* clear interrupt source */
    QF_tick();
}
/*..........................................................................*/
static __arm __ramfunc void ISR_spur(void) {
}
/*..........................................................................*/
__arm void QF_onStartup(void) {
                            /* hook the exception handlers from the QF port */
    *(uint32_t volatile *)0x24 = (uint32_t)&QF_undef;
    *(uint32_t volatile *)0x28 = (uint32_t)&QF_swi;
    *(uint32_t volatile *)0x2C = (uint32_t)&QF_pAbort;
    *(uint32_t volatile *)0x30 = (uint32_t)&QF_dAbort;
    *(uint32_t volatile *)0x34 = (uint32_t)&QF_reserved;

    *(uint32_t volatile *)0x38 = (uint32_t)&QK_irq;
    *(uint32_t volatile *)0x3C = (uint32_t)&QK_fiq;

    AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS] = (uint32_t)&ISR_tick;
    AT91C_BASE_AIC->AIC_SPU = (uint32_t)&ISR_spur;          /* spurious IRQ */

    AT91C_BASE_AIC->AIC_SMR[AT91C_ID_SYS] =
        (AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | AT91C_AIC_PRIOR_LOWEST);
    AT91C_BASE_AIC->AIC_ICCR = (1 << AT91C_ID_SYS);
    AT91C_BASE_AIC->AIC_IECR = (1 << AT91C_ID_SYS);
}
/*..........................................................................*/
__ramfunc void QK_onIdle(void) {
#ifdef NDEBUG     /* only if not debugging (power saving hinders debugging) */
    AT91C_BASE_PMC->PMC_SCDR = 1;/* Power-Management: disable the CPU clock */
    /* NOTE: an interrupt starts the CPU clock again */
#endif
}
/*..........................................................................*/
void QF_stop(void) {
}

/*..........................................................................*/
void BSP_init(void) {
    uint32_t i;

    for (i = 0; i < Q_DIM(l_led); ++i) {          /* initialize the LEDs... */
        AT91C_BASE_PIOA->PIO_PER = l_led[i];                  /* enable pin */
        AT91C_BASE_PIOA->PIO_OER = l_led[i];     /* configure as output pin */
        LED_OFF(i);                                   /* extinguish the LED */
    }
                /* configure Advanced Interrupt Controller (AIC) of AT91... */
    AT91C_BASE_AIC->AIC_IDCR = ~0;                /* disable all interrupts */
    AT91C_BASE_AIC->AIC_ICCR = ~0;                  /* clear all interrupts */
    for (i = 0; i < 8; ++i) {
        AT91C_BASE_AIC->AIC_EOICR = 0;           /* write AIC_EOICR 8 times */
    }

                             /* set the desired ticking rate for the PIT... */
    i = (MCK / 16 / BSP_TICKS_PER_SEC) - 1;
    AT91C_BASE_PITC->PITC_PIMR = (AT91C_PITC_PITEN | AT91C_PITC_PITIEN | i);
}
/*..........................................................................*/
void BSP_busyDelay(void) {
}

/*............................................................................
* __low_level_init() is invoked by the standard IAR startup sequence after
* cstartup, but before initializing the segments in RAM. The function
* gives the application a chance to perform early initializations of
* the hardware. This function cannot use any static variables, because these
* have not yet been initialized in RAM.
*
* The value returned by __low_level_init() determines whether or not data
* segments should be initialized by __seqment_init. If __low_level_init()
* returns 0, the data segments will NOT be initialized. For more information
* see the "IAR ARM C/C++ Compiler Reference Guide".
*/
int __low_level_init(void) {
    AT91PS_PMC pPMC = AT91C_BASE_PMC;

    /* Set flash wait sate FWS and FMCN */
    AT91C_BASE_MC->MC_FMR = ((AT91C_MC_FMCN) & ((MCK + 500000)/1000000 << 16))
                             | AT91C_MC_FWS_1FWS;

    AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;  /* Disable the watchdog */

    /* Enable the Main Oscillator:
    * set OSCOUNT to 6, which gives Start up time = 8 * 6 / SCK = 1.4ms
    * (SCK = 32768Hz)
    */
    pPMC->PMC_MOR = ((6 << 8) & AT91C_CKGR_OSCOUNT) | AT91C_CKGR_MOSCEN;
    while ((pPMC->PMC_SR & AT91C_PMC_MOSCS) == 0) {/* Wait the startup time */
    }

    /* Set the PLL and Divider:
    * - div by 5 Fin = 3,6864 =(18,432 / 5)
    * - Mul 25+1: Fout = 95,8464 =(3,6864 *26)
    * for 96 MHz the error is 0.16%
    * Field out NOT USED = 0
    * PLLCOUNT pll startup time estimate at : 0.844 ms
    * PLLCOUNT 28 = 0.000844 /(1/32768)
    */
    pPMC->PMC_PLLR = ((AT91C_CKGR_DIV & 0x05)
                      | (AT91C_CKGR_PLLCOUNT & (28 << 8))
                      | (AT91C_CKGR_MUL & (25 << 16)));
    while ((pPMC->PMC_SR & AT91C_PMC_LOCK) == 0) { /* Wait the startup time */
    }
    while ((pPMC->PMC_SR & AT91C_PMC_MCKRDY) == 0) {
    }

    /* Select Master Clock and CPU Clock select the PLL clock / 2 */
    pPMC->PMC_MCKR =  AT91C_PMC_PRES_CLK_2;
    while ((pPMC->PMC_SR & AT91C_PMC_MCKRDY) == 0) {
    }

    pPMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;
    while ((pPMC->PMC_SR & AT91C_PMC_MCKRDY) == 0) {
    }

    return 1;                    /* proceed with the segment initialization */
}
/*..........................................................................*/
void BSP_displyPhilStat(uint8_t n, char const *stat) {
    if (n < Q_DIM(l_led)) {       /* do not exceed number of available LEDs */
        if (stat[0] == (uint8_t)'e') {       /* is this Philosopher eating? */
            LED_ON(n);
        }
        else {                            /* this Philosopher is not eating */
            LED_OFF(n);
        }
    }
}
/*..........................................................................*/
__arm void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE_32();      /* make sure that all interrupts are disabled */
    for (;;) {                            /* hang here in the for-ever loop */
    }                  /* NOTE: this must be changed for production code!!! */
}

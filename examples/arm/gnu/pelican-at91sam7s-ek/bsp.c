/*****************************************************************************
* Product: QDK-nano_ARM-GNU_AT91SAM7S-EK
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
#include "pelican.h"
#include "bsp.h"

/* Local objects -----------------------------------------------------------*/
uint32_t const l_led[] = {
    (1 << 0),                                     /* LED D1 on AT91SAM7S-EK */
    (1 << 1),                                     /* LED D2 on AT91SAM7S-EK */
    (1 << 2),                                     /* LED D3 on AT91SAM7S-EK */
    (1 << 3)                                      /* LED D4 on AT91SAM7S-EK */
};

#define LED_ON(num_)       (AT91C_BASE_PIOA->PIO_CODR = l_led[num_])
#define LED_OFF(num_)      (AT91C_BASE_PIOA->PIO_SODR = l_led[num_])

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

    /* set the desired ticking rate for the PIT */
    i = (MCK / 16 / BSP_TICKS_PER_SEC) - 1;
    AT91C_BASE_PITC->PITC_PIMR = (AT91C_PITC_PITEN | AT91C_PITC_PITIEN | i);
}
/*..........................................................................*/
/* NOTE: QF_onIdle() is called with interrupts DISABLED */
void QF_onIdle(void) {

#ifdef NDEBUG     /* only if not debugging (power saving hinders debugging) */
    AT91C_BASE_PMC->PMC_SCDR = 1;/* Power-Management: disable the CPU clock */
    /* NOTE: an interrupt starts the CPU clock again */
#endif

    QF_INT_ENABLE();       /* enable interrupts as soon as CPU clock starts */
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_OFF:
            LED_OFF(1);
            LED_OFF(2);
            LED_OFF(3);
            break;
        case CARS_RED:
            LED_ON(1);
            LED_OFF(2);
            LED_OFF(3);
            break;
        case CARS_YELLOW:
            LED_OFF(1);
            LED_ON(2);
            LED_OFF(3);
            break;
        case CARS_GREEN:
            LED_OFF(1);
            LED_OFF(2);
            LED_ON(3);
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    if (sig == PEDS_DONT_WALK) {
        LED_ON(0);
    }
    else {
        LED_OFF(0);
    }
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();           /* make sure that all interrupts are locked */
    for (;;) {                            /* hang here in the for-ever loop */
    }
}

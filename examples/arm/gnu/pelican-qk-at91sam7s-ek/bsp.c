/*****************************************************************************
* Product: QDK-nano_ARM-GNU_AT91SAM7S-EK with QK-nano
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Oct 11, 2012
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
#include "pelican.h"
#include "bsp.h"

/* Global objects ----------------------------------------------------------*/
uint32_t const BSP_led[4] = {
    (1U << 0),                                    /* LED D1 on AT91SAM7S-EK */
    (1U << 1),                                    /* LED D2 on AT91SAM7S-EK */
    (1U << 2),                                    /* LED D3 on AT91SAM7S-EK */
    (1U << 3)                                     /* LED D4 on AT91SAM7S-EK */
};
uint32_t const BSP_pb[4] = {
    (1U << 19),                          /* Push Button PB1 on AT91SAM7S-EK */
    (1U << 20),                          /* Push Button PB2 on AT91SAM7S-EK */
    (1U << 15),                          /* Push Button PB3 on AT91SAM7S-EK */
    (1U << 14)                           /* Push Button PB4 on AT91SAM7S-EK */
};

/*..........................................................................*/
void BSP_init(void) {
    uint32_t i;

    for (i = 0; i < Q_DIM(BSP_led); ++i) {        /* initialize the LEDs... */
        AT91C_BASE_PIOA->PIO_PER = BSP_led[i];                /* enable pin */
        AT91C_BASE_PIOA->PIO_OER = BSP_led[i];   /* configure as output pin */
        LED_OFF(i);                                   /* extinguish the LED */
    }
    for (i = 0; i < Q_DIM(BSP_pb); ++i) { /* initialize the Push Buttons... */
        AT91C_BASE_PMC->PMC_PCER = (1U << AT91C_ID_PIOA);
        AT91C_BASE_PIOA->PIO_IDR   = BSP_pb[i];       /* disable interrupts */
        AT91C_BASE_PIOA->PIO_PPUER = BSP_pb[i];            /* enable pullup */
        AT91C_BASE_PIOA->PIO_IFER  = BSP_pb[i];   /* enable deglitch filter */
        AT91C_BASE_PIOA->PIO_ODR   = BSP_pb[i];   /* configure pin as input */
        AT91C_BASE_PIOA->PIO_PER   = BSP_pb[i];   /* configure pin as input */
    }
                /* configure Advanced Interrupt Controller (AIC) of AT91... */
    AT91C_BASE_AIC->AIC_IDCR = ~0;                /* disable all interrupts */
    AT91C_BASE_AIC->AIC_ICCR = ~0;                  /* clear all interrupts */
    for (i = 0; i < 8; ++i) {
        AT91C_BASE_AIC->AIC_EOICR = 0;           /* write AIC_EOICR 8 times */
    }

    /* set the desired ticking rate for the PIT */
    i = (MCK / 16U / BSP_TICKS_PER_SEC) - 1U;
    AT91C_BASE_PITC->PITC_PIMR = (AT91C_PITC_PITEN | AT91C_PITC_PITIEN | i);
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_BLANK:
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
void BSP_showState(char_t const *state) {
    (void)state;
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();           /* make sure that all interrupts are locked */
    for (;;) {                            /* hang here in the for-ever loop */
    }
}

/*..........................................................................*/
void QK_onIdle(void) {
#ifdef NDEBUG     /* only if not debugging (power saving hinders debugging) */
    AT91C_BASE_PMC->PMC_SCDR = 1;/* Power-Management: disable the CPU clock */
    /* NOTE: an interrupt starts the CPU clock again */
#endif
}

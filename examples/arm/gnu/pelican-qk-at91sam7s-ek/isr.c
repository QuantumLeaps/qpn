/*****************************************************************************
* Product: QDK-nano_ARM-GNU_AT91SAM7S-EK with QK
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

typedef void (*IntVector)(void);           /* IntVector pointer-to-function */

/*..........................................................................*/
__attribute__ ((section (".text.fastcode")))
static void ISR_tick(void) {
    uint32_t volatile dummy = AT91C_BASE_PITC->PITC_PIVR;/*clear int source */
    (void)dummy;         /* avoid compiler warning "dummy" set but not used */
    QF_tick();                             /* process the system clock tick */
}
/*..........................................................................*/
static void ISR_spur(void) {
}
/*..........................................................................*/
__attribute__ ((section (".text.fastcode")))
void BSP_irq(void) {
    IntVector vect = (IntVector)AT91C_BASE_AIC->AIC_IVR;    /* read the IVR */
    AT91C_BASE_AIC->AIC_IVR = (AT91_REG)vect; /* write AIC_IVR if protected */

    QF_INT_ENABLE();                /* allow nesting interrupts, see NOTE01 */
    (*vect)();              /* call the IRQ ISR via the pointer to function */
    QF_INT_DISABLE();            /* disable interrups for the exit sequence */

    AT91C_BASE_AIC->AIC_EOICR = (AT91_REG)vect;      /* send EOI to the AIC */
}
/*..........................................................................*/
__attribute__ ((section (".text.fastcode")))
void BSP_fiq(void) {
    /* TBD: implement the FIQ handler directly right here, see NOTE02  */
    /* NOTE: Do NOT enable interrupts throughout the whole FIQ processing. */
    /* NOTE: Do NOT write EOI to the AIC */
}
/*..........................................................................*/
void QF_onStartup(void) {
                            /* hook the exception handlers from the QF port */
    *(uint32_t volatile *)0x24 = (uint32_t)&QF_undef;
    *(uint32_t volatile *)0x28 = (uint32_t)&QF_swi;
    *(uint32_t volatile *)0x2C = (uint32_t)&QF_pAbort;
    *(uint32_t volatile *)0x30 = (uint32_t)&QF_dAbort;
    *(uint32_t volatile *)0x34 = (uint32_t)&QF_reserved;

    *(uint32_t volatile *)0x38 = (uint32_t)&QK_irq;
    *(uint32_t volatile *)0x3C = (uint32_t)&QK_fiq;

    AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS] = (uint32_t)&ISR_tick;/* tick ISR */
    AT91C_BASE_AIC->AIC_SPU  = (uint32_t)&ISR_spur;         /* spurious ISR */

    AT91C_BASE_AIC->AIC_SMR[AT91C_ID_SYS] =
        (AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | AT91C_AIC_PRIOR_LOWEST);
    AT91C_BASE_AIC->AIC_ICCR = (1 << AT91C_ID_SYS);
    AT91C_BASE_AIC->AIC_IECR = (1 << AT91C_ID_SYS);
}

/*****************************************************************************
* NOTE01:
* The QF IRQ assembly "wrapper" QF_irq() calls the IRQ handler BSP_irq()
* with interrupts locked at the ARM core level. In the presence of the
* Advanced Interrupt Controller (AIC), we can unlock the interrupts at the
* ARM core level, even for level-sensitive interrupt that require explicit
* clearing. The AIC will prevent the interrupt of the same level from
* interrupting the CPU again until it receives the End-Of-Interrupt command
* (AT91C_BASE_AIC->AIC_EOICR = 0).
*
* NOTE02:
* The QF FIQ assembly "wrapper" QF_fiq() calls the FIQ handler BSP_fiq()
* with interrupts locked at the ARM core level. In contrast to the IRQ line,
* the FIQ line is NOT prioritized by the AIC. Therefore, you must NOT enable
* interrupts while processing FIQ. All FIQs should be the highest-priority
* in the system. All FIQs run at the same (highest) priority level.
*****************************************************************************/

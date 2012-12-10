/*****************************************************************************
* Product: QDK-nano_ARM-GNU_AT91SAM7S-EK, Vanilla kernel
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

typedef void (*IntVector)(void);           /* IntVector pointer-to-function */

/*..........................................................................*/
__attribute__ ((section (".text.fastcode")))
static void ISR_tick(void) {
    static uint32_t btn_debounced  = 0U;
    static uint8_t  debounce_state = 0U;
    uint32_t tmp;

    tmp = AT91C_BASE_PITC->PITC_PIVR;             /* clear interrupt source */
    QF_tickISR();                        /* process all QF-nano time events */

    tmp = AT91C_BASE_PIOA->PIO_PDSR & BSP_pb[0];     /* read the push btn 0 */
    switch (debounce_state) {
        case 0:
            if (tmp != btn_debounced) {
                debounce_state = 1U;        /* transition to the next state */
            }
            break;
        case 1:
            if (tmp != btn_debounced) {
                debounce_state = 2U;        /* transition to the next state */
            }
            else {
                debounce_state = 0U;          /* transition back to state 0 */
            }
            break;
        case 2:
            if (tmp != btn_debounced) {
                debounce_state = 3U;        /* transition to the next state */
            }
            else {
                debounce_state = 0U;          /* transition back to state 0 */
            }
            break;
        case 3:
            if (tmp != btn_debounced) {
                btn_debounced = tmp;     /* save the debounced button value */

                if (tmp == 0U) {                /* is the button depressed? */
                    QActive_post((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0U);
                }
                else {
                }
            }
            debounce_state = 0U;              /* transition back to state 0 */
            break;
    }
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

    *(uint32_t volatile *)0x38 = (uint32_t)&QF_irq;
    *(uint32_t volatile *)0x3C = (uint32_t)&QF_fiq;

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

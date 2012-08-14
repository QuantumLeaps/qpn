/*****************************************************************************
* Product: DPP example
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

#include <dos.h>                       /* for _dos_setvect()/_dos_getvect() */
#include <conio.h>                                      /* for inp()/outp() */
#include <stdlib.h>                                          /* for _exit() */
#include <stdio.h>

Q_DEFINE_THIS_MODULE("bsp")

/* Local-scope objects -----------------------------------------------------*/
static void interrupt (*l_dosTmrISR)();
static void interrupt (*l_dosKbdISR)();

static uint32_t l_delay = 0UL; /* limit for the loop counter in busyDelay() */

#define TMR_VECTOR      0x08
#define KBD_VECTOR      0x09

/*..........................................................................*/
static void interrupt tmrISR(void) {
    QF_tickISR();                          /* process all armed time events */
    outp(0x20, 0x20);                        /* write EOI to the master PIC */
}
/*..........................................................................*/
static void interrupt kbdISR() {
    uint8_t key = inp(0x60);      /* key scan code from 8042 kbd controller */
    uint8_t kcr = inp(0x61);               /* get keyboard control register */

    outp(0x61, (uint8_t)(kcr | 0x80));       /* toggle acknowledge bit high */
    outp(0x61, kcr);                          /* toggle acknowledge bit low */

    if (key == (uint8_t)129) {                          /* ESC key pressed? */
        QActive_postISR((QActive *)&AO_Table, TERMINATE_SIG, 0);
    }
    outp(0x20, 0x20);                        /* write EOI to the master PIC */
}
/*..........................................................................*/
void BSP_init(int argc, char *argv[]) {
    if (argc > 1) {
        l_delay = atol(argv[1]);    /* set the delay counter for busy delay */
    }
    printf("Dining Philosopher Problem example"
           "\nQP-nano %s\n"
           "Press ESC to quit...\n",
           QP_getVersion());
}
/*..........................................................................*/
void BSP_displyPhilStat(uint8_t n, char const *stat) {
    printf("Philosopher %2d is %s\n", (int)n, stat);
}
/*..........................................................................*/
void BSP_busyDelay(void) {
    uint32_t volatile i = l_delay;
    while (i-- > 0UL) {                                   /* busy-wait loop */
    }
}
/*..........................................................................*/
void QF_onStartup(void) {
                                      /* save the origingal DOS vectors ... */
    l_dosTmrISR = _dos_getvect(TMR_VECTOR);
    l_dosKbdISR = _dos_getvect(KBD_VECTOR);

    QF_INT_DISABLE();
    _dos_setvect(TMR_VECTOR, &tmrISR);
    _dos_setvect(KBD_VECTOR, &kbdISR);
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QF_stop(void) {
                                    /* restore the original DOS vectors ... */
    if (l_dosTmrISR != (void interrupt (*)(void))0) { /* DOS vectors saved? */
        QF_INT_DISABLE();
        _dos_setvect(TMR_VECTOR, l_dosTmrISR);
        _dos_setvect(KBD_VECTOR, l_dosKbdISR);
        QF_INT_ENABLE();
    }
    _exit(0);                                                /* exit to DOS */
}
/*..........................................................................*/
void QF_onIdle(void) {                                        /* see NOTE02 */
    QF_INT_ENABLE();
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();                             /* cut-off all interrupts */
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    QF_stop();
}

/*****************************************************************************
* NOTE01:
* The sofware interrupt generated at the end of the tick ISR invokes the
* original DOS tick ISR handler, which was installed at the spare vector
* 0x81. The DOS tick ISR handler generates the EOI (End-Of-Interrupt)
* to the master 8259A PIC.
*
* NOTE02:
* The QF_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally enable interrupts, ideally atomically
* with putting the CPU to the power-saving mode.
*/

/*****************************************************************************
* Product: BSP for State-Local Storage Example, 80x86, Vanilla, Open Watcom
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
#include "qpn_port.h"                  /* the port of the QP-nano framework */
#include "num_ent.h"
#include "ui.h"
#include "bsp.h"
#include "video.h"

#include <dos.h>                       /* for _dos_setvect()/_dos_getvect() */
#include <conio.h>                                      /* for inp()/outp() */
#include <stdlib.h>                                          /* for _exit() */

Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
static void interrupt (*l_dosTmrISR)();
static void interrupt (*l_dosKbdISR)();

#define TMR_VECTOR      0x08
#define KBD_VECTOR      0x09

/*..........................................................................*/
static void interrupt ISR_tmr() {
    QF_tickISR();                          /* process all armed time events */
    outp(0x20, 0x20);                        /* write EOI to the master PIC */
}
/*..........................................................................*/
static void interrupt ISR_kbd() {
    uint8_t key = inp(0x60);      /* key scan code from 8042 kbd controller */
    uint8_t kcr = inp(0x61);               /* get keyboard control register */
    outp(0x61, (uint8_t)(kcr | 0x80));       /* toggle acknowledge bit high */
    outp(0x61, kcr);                          /* toggle acknowledge bit low */

    switch (key) {
        default: {
//            Video_printNumAt(1, 24, VIDEO_FGND_YELLOW, key);
            break;
        }
        case 11: {                                                   /* '0' */
            QActive_post((QActive *)&AO_UI, DIGIT_0_SIG, '0');
            break;
        }
        case 2:                                                      /* '1' */
        case 3:                                                      /* '2' */
        case 4:                                                      /* '3' */
        case 5:                                                      /* '4' */
        case 6:                                                      /* '5' */
        case 7:                                                      /* '6' */
        case 8:                                                      /* '7' */
        case 9:                                                      /* '8' */
        case 10: {                                                   /* '9' */
            QActive_post((QActive *)&AO_UI, DIGIT_1_9_SIG, '0' + (key - 1));
            break;
        }
        case 52: {                                                   /* '.' */
            QActive_post((QActive *)&AO_UI, POINT_SIG, '.');
            break;
        }
        case 12: {                                                   /* '-' */
            QActive_post((QActive *)&AO_UI, NEG_SIG, '-');
            break;
        }
        case 46: {                                                   /* 'c' */
            QActive_post((QActive *)&AO_UI, C_SIG, 0);
            break;
        }
        case 18: {                                                   /* 'e' */
            QActive_post((QActive *)&AO_UI, CE_SIG, 0);
            break;
        }
        case 28: {                                                 /* ENTER */
            QActive_post((QActive *)&AO_UI, ENTER_SIG, 0);
            break;
        }
        case 200: {                                             /* UP-arrow */
            QActive_post((QActive *)&AO_UI, UP_SIG, 0);
            break;
        }
        case 208: {                                           /* DOWN-arrow */
            QActive_post((QActive *)&AO_UI, DOWN_SIG, 0);
            break;
        }
        case 59: {                                                    /* F1 */
            QActive_post((QActive *)&AO_UI, HELP_SIG, 0);
            break;
        }
        case 129: {                                                  /* ESC */
            QActive_post((QActive *)&AO_UI, QUIT_SIG, 0);
            break;
        }
    }

    outp(0x20, 0x20);                        /* write EOI to the master PIC */
}

/*..........................................................................*/
void BSP_init(int argc, char *argv[]) {
}
/*..........................................................................*/
void QF_onStartup(void) {
    uint16_t count;
                                      /* save the origingal DOS vectors ... */
    l_dosTmrISR = _dos_getvect(TMR_VECTOR);
    l_dosKbdISR = _dos_getvect(KBD_VECTOR);

    count = (uint16_t)(((1193180 * 2) / BSP_TICKS_PER_SEC + 1) >> 1);
    outp(0x43, 0x36);                 /* use mode-3 for timer 0 in the 8254 */
    outp(0x40, count & 0xFF);                  /* load low  byte of timer 0 */
    outp(0x40, (count >> 8) & 0xFF);           /* load high byte of timer 0 */

    _dos_setvect(TMR_VECTOR, &ISR_tmr);
    _dos_setvect(KBD_VECTOR, &ISR_kbd);
}
/*..........................................................................*/
void QF_onCleanup(void) {
                               /* restore the DOS system clock tick rate... */
    QF_INT_DISABLE();
    outp(0x43, 0x36);                 /* use mode-3 for timer 0 in the 8254 */
    outp(0x40, 0);                             /* load low  byte of timer 0 */
    outp(0x40, 0);                             /* load high byte of timer 0 */
                                   /* restore the original DOS vectors ...  */
    _dos_setvect(TMR_VECTOR, l_dosTmrISR);
    _dos_setvect(KBD_VECTOR, l_dosKbdISR);
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QF_stop(void) {
    QF_onCleanup();
    _exit(0);                                                /* exit to DOS */
}
/*..........................................................................*/
void QF_onIdle(void) {
    QF_INT_ENABLE();                            /* always enable interrupts */
}
/*..........................................................................*/
/* this function is used by the QP embedded systems-friendly assertions     */
void Q_onAssert(char const * const file, int line) {
    Video_clearRect ( 0, 24, 80, 25, VIDEO_BGND_RED);
    Video_printStrAt( 0, 24, VIDEO_FGND_WHITE, "ASSERTION FAILED in file:");
    Video_printStrAt(26, 24, VIDEO_FGND_YELLOW, file);
    Video_printStrAt(57, 24, VIDEO_FGND_WHITE, "line:");
    Video_printNumAt(62, 24, VIDEO_FGND_YELLOW, line);
    _exit(-1);
}

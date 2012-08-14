/*****************************************************************************
* QHsmTst example
* Last Updated for Version: 4.0.01
* Date of the Last Update:  Jun 12, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
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
#include "bsp.h"
#include "qhsmtst.h"

/*..........................................................................*/
void main (void) {
    QHsmTst_ctor();                                  /* instantiate the HSM */

    BSP_init();                                     /* initialize the board */

    QHsm_init((QHsm *)&HSM_QHsmTst);        /* take the initial transitioin */

    for (;;) {
        char c;

        LED_1_On();
        UART_1_PutCRLF();
        UART_1_PutChar('>');
        c = UART_1_cGetChar();                       /* wait for user input */
        UART_1_PutChar(c);
        UART_1_CPutString(": ");
        LED_1_Off();

        LED_2_On();
        if ('a' <= c && c <= 'i') {                            /* in range? */
            Q_SIG((QHsm *)&HSM_QHsmTst) = (QSignal)(c - 'a' + A_SIG);
        }
        else if ('A' <= c && c <= 'I') {                       /* in range? */
            Q_SIG((QHsm *)&HSM_QHsmTst) = (QSignal)(c - 'A' + A_SIG);
        }
        else {
            Q_SIG((QHsm *)&HSM_QHsmTst) = IGNORE_SIG;
        }
        QHsm_dispatch((QHsm *)&HSM_QHsmTst);          /* dispatch the event */
        LED_2_Off();
    }
}

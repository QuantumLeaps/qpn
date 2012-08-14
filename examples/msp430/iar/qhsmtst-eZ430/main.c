/*****************************************************************************
* Product: QHsmTst example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 05, 2008
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

#include <yfuns.h>                                  /* for the terminal I/O */

/*..........................................................................*/
void main (void) {
    QHsmTst_ctor();           /* instantiate the QHsmTst test state machine */

    BSP_init();                                     /* initialize the board */

    QHsm_init((QHsm *)&HSM_QHsmTst);              /* take the initial tran. */

    for (;;) {           /* interactive testing all kinds of transitions... */
        static uint8_t buf[] = "\nX:";
        __read(0, &buf[1], 1);
        if ('A' <= buf[1] && buf[1] <= 'I') {                  /* in range? */
            __write(1, buf, 3);

            BSP_LED_on();
            Q_SIG((QHsm *)&HSM_QHsmTst) = buf[1] - 'A' + A_SIG;
            QHsm_dispatch((QHsm *)&HSM_QHsmTst);      /* dispatch the event */
            BSP_LED_off();
        }
    }
}

/*****************************************************************************
* Product: QHsmTst Example
* Last Updated for Version: 6.3.7
* Date of the Last Update:  2018-12-13
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2018 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
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
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"
#include "bsp.h"
#include "qhsmtst.h"

#include "safe_std.h" /* portable "safe" <stdio.h>/<string.h> facilities */
#include <stdlib.h>

//Q_DEFINE_THIS_FILE

/*..........................................................................*/
int main(int argc, char *argv[]) {
    QHsmTst_ctor(); /* instantiate the QHsmTst object */

    BSP_init((argc > 1) ? argv[1] : "");
    QF_onStartup();

    if (argc > 1) { /* batch version? */
        /* testing of dynamic transitions... */
        BSP_dispatch(A_SIG);
        BSP_dispatch(B_SIG);
        BSP_dispatch(D_SIG);
        BSP_dispatch(E_SIG);
        BSP_dispatch(I_SIG);
        BSP_dispatch(F_SIG);
        BSP_dispatch(I_SIG);
        BSP_dispatch(I_SIG);
        BSP_dispatch(F_SIG);
        BSP_dispatch(A_SIG);
        BSP_dispatch(B_SIG);
        BSP_dispatch(D_SIG);
        BSP_dispatch(D_SIG);
        BSP_dispatch(E_SIG);
        BSP_dispatch(G_SIG);
        BSP_dispatch(H_SIG);
        BSP_dispatch(H_SIG);
        BSP_dispatch(C_SIG);
        BSP_dispatch(G_SIG);
        BSP_dispatch(C_SIG);
        BSP_dispatch(C_SIG);
    }
    else { /* interactive version */
        PRINTF_S("\n", "");
        for (;;) { /* event loop */
            int c;
            QSignal sig;

            c = (uint8_t)QF_consoleWaitForKey();

            if ('a' <= c && c <= 'i') {      /* in range? */
                sig = (QSignal)(c - 'a' + A_SIG);
            }
            else if ('A' <= c && c <= 'I') { /* in range? */
               sig = (QSignal)(c - 'A' + A_SIG);
            }
            else if (c == '\33') {           /* the ESC key? */
                sig = TERMINATE_SIG;
            }
            else {
                sig = IGNORE_SIG;
            }

            BSP_dispatch(sig);
        }
    }

    QF_onCleanup();
    return 0;
}

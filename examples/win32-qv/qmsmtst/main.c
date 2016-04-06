/*****************************************************************************
* Product: QMsmTst Example, Win32
* Last Updated for Version: 5.6.2
* Date of the Last Update:  2016-04-05
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* http://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpn.h"
#include "bsp.h"
#include "qmsmtst.h"

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
int main(int argc, char *argv[]) {
    QMsmTst_ctor(); /* instantiate the QMsmTst object */

    BSP_init((argc > 1) ? argv[1] : "");

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
        for (;;) { /* event loop */
            int c;
            QSignal sig;

            printf("\n>");
            c = _getche(); /* get a character from the console with echo */
            printf(": ");

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

    return 0;
}

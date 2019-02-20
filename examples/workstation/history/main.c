/*****************************************************************************
* Product: History Example for QP-nano, Win32
* Last updated for version 6.4.0
* Last updated on  2019-02-10
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
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
* https://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpn.h"     /* QP-nano */
#include "bsp.h"     /* Board Support Package */
#include "history.h" /* Application interface */

#include <stdio.h>
#include <stdlib.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
int main() {
    QF_onStartup();

    ToastOven_ctor(); /* instantiate the ToasterOven object */

    BSP_init();       /* initialize the Board Support Package */

    QHSM_INIT(the_oven); /* trigger the initial transition */

    for (;;) {
        uint8_t c;

        printf("\n");
        c = (uint8_t)QF_consoleWaitForKey();
        printf("%c: ", (c != '\33') ? c : 'x');

        switch (c) {
            case 'o':   Q_SIG(the_oven) = OPEN_SIG;      break;
            case 'c':   Q_SIG(the_oven) = CLOSE_SIG;     break;
            case 't':   Q_SIG(the_oven) = TOAST_SIG;     break;
            case 'b':   Q_SIG(the_oven) = BAKE_SIG;      break;
            case 'f':   Q_SIG(the_oven) = OFF_SIG;       break;
            case '\33': Q_SIG(the_oven) = TERMINATE_SIG; break;
        }

        /* dispatch the event into the state machine */
        QHSM_DISPATCH(the_oven);
    }

    QF_onCleanup();
    return 0;
}

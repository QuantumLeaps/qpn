/*****************************************************************************
* Product: QMsmTst example
* Last updated for version 5.3.0
* Last updated on  2014-04-18
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"
#include "qmsmtst.h"

#include <yfuns.h>  /* for the terminal I/O */

/*..........................................................................*/
void main (void) {
    QMsmTst_ctor(); /* instantiate the QHsmTst test state machine */

    BSP_init();     /* initialize the board */

    QMSM_INIT(the_msm);  /* take the initial tran. */

    /* interactive testing all kinds of transitions... */
    for (;;) {
        static uint8_t buf[] = "\nX:";
        __read(0, &buf[1], 1);
        if ('A' <= buf[1] && buf[1] <= 'I') { /* in range? */
            __write(1, buf, 3);

            BSP_LED_on();
            Q_SIG(the_msm) = buf[1] - 'A' + A_SIG;
            QMSM_DISPATCH(the_msm); /* dispatch the event */
            BSP_LED_off();
        }
    }
}

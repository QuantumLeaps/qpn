/*****************************************************************************
* Product: BSP for QHsmTst example, Win32
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
#include "qpn.h"
#include "bsp.h"
#include "qhsmtst.h"

#include <stdlib.h>
#include <stdio.h>

Q_DEFINE_THIS_FILE

/*..........................................................................*/
/* dummy definition of the QF_active[] array (not used in this example) */
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,  (QEvt *)0, 0U }
};

static FILE *l_outFile;

/*..........................................................................*/
void BSP_init(char const *fname) {
   if (*fname == '\0') {
        l_outFile = stdout;  /* use the stdout as the output file */
        printf("QHsmTst example, built on %s at %s,\n"
               "QP-nano: %s.\nPress ESC to quit...\n",
               __DATE__, __TIME__, QP_VERSION_STR);
    }
    else {
        l_outFile = fopen(fname, "w");
        Q_ASSERT(l_outFile != (FILE *)0);

        printf("QHsmTst example, built on %s at %s, QP-nano %s\n"
               "output saved to %s\n",
               __DATE__, __TIME__, QP_VERSION_STR,
               fname);

        fprintf(l_outFile, "QHsmTst example, QP-nano %s\n",
               QP_VERSION_STR);
    }

    QHSM_INIT(the_hsm); /* the top-most initial tran. */
}
/*..........................................................................*/
void BSP_exit(void) {
    fclose(l_outFile);
    printf("\nBye! Bye!\n");
    QF_onCleanup();
    exit(0);
}
/*..........................................................................*/
void BSP_display(char const *msg) {
    fprintf(l_outFile, msg);
}
/*..........................................................................*/
void BSP_dispatch(QSignal sig) {
    if ((A_SIG <= sig) && (sig <= I_SIG)) {
        fprintf(l_outFile, "%c:", 'A' + sig - A_SIG);
    }
    Q_SIG(the_hsm) = sig;
    QHSM_DISPATCH(the_hsm); /* dispatch the event */
    fprintf(l_outFile, "\n");
}


/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    QF_consoleSetup();
}
/*..........................................................................*/
void QF_onCleanup(void) {
    QF_consoleCleanup();
}
/*..........................................................................*/
void QF_onClockTickISR(void) {
}

/*..........................................................................*/
/* this function is used by the QP embedded systems-friendly assertions */
void Q_onAssert(char const * const file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
    exit(-1);
}





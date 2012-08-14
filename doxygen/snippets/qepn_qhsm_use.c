#include "qpn.h"                                /* QP-nano public interface */
#include "qcalc.h"                           /* QCalc HSM derived from QHsm */

static QCalc l_qcalc;                           /* an instance of QCalc HSM */

int main() {
    QCalc_ctor(&l_qcalc);    /* QCalc HSM "constructor" invokes QHsm_ctor() */

    QHsm_init((QHsm *)&l_qcalc);              /* trigger initial transition */

    for (;;) {                                                /* event loop */
        . . .
        /* wait for the next event and assign it to the current event */
        QSIG(&l_qcalc) = ...
        QPAR(&l_qcalc) = ...
        QHsm_dispatch((QHsm *)&l_qcalc);   /* dispatch the event to l_qcalc */
    }
    return 0;
}

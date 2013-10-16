#include "qpn.h"     /* QP-nano public interface */
#include "calc.h"    /* Calc SM derived from QMsm */

static Calc l_calc;  /* an instance of Calc SM */

int main() {
    Calc_ctor(&l_calc); /* Calc SM "constructor" invokes QMsm_ctor() */

    QMSM_INIT(&l_calc); /* trigger initial transition */

    for (;;) { /* event loop */
        . . .
        /* wait for the next event and assign it to the current event */
        QSIG(&l_calc) = ...
        QPAR(&l_calc) = ...
        QMSM_DISPATCH(&l_calc);  /* dispatch the event to l_calc */
    }
    return 0;
}

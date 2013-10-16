#include "qpn_port.h"  /* QP-nano port */
#include "calc.h"      /* Calc HSM derived from QHsm */

static Calc l_calc;    /* an instance of Calc HSM */

int main() {
    Calc_ctor(&l_calc); /* Calc HSM "constructor" invokes QHsm_ctor() */

    QMSM_INIT(&l_calc.super); /* trigger initial transition, late biniding */

    for (;;) { /* event loop */
        . . .
        /* wait for the next event and assign it to the current event */
        QSIG(&l_calc) = ...
        QPAR(&l_calc) = ...
        QMSM_DISPATCH(&l_calc.super); /* dispatch the event, late binding */
    }
    return 0;
}

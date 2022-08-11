#include "qpn.h"   /* QP-nano */
#include "calc.h"  /* Application interface */

Q_DEFINE_THIS_FILE

static Calc l_calc; /* an instance of Calc HSM */

int main() {
    Calc_ctor(&l_calc); /* Calc HSM "constructor" invokes QHsm_ctor() */

    QHSM_INIT(&l_calc.super); /* trigger initial transition, late biniding */

    for (;;) { /* event loop */
        . . .
        /* wait for the next event and assign it to the current event */
        QSIG(&l_calc) = ...
        QPAR(&l_calc) = ...
        QHSM_DISPATCH(&l_calc.super); /* dispatch the event, late binding */
    }
    return 0;
}

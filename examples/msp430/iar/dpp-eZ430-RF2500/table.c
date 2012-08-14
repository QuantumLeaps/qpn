/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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
#include "qpn_port.h"                                       /* QP-nano port */
#include "dpp.h"                                   /* application interface */
#include "bsp.h"                             /* Board Support Package (BSP) */

Q_DEFINE_THIS_MODULE("table")

/* Active object class -----------------------------------------------------*/
typedef struct TableTag {
    QActive super;
    uint8_t forks;
    uint8_t isHungry;
} Table;

static QState Table_initial(Table *me);
static QState Table_serving(Table *me);

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1) % N_PHILO))

/* Global objects ----------------------------------------------------------*/
Table AO_Table;           /* the single instance of the Table active object */

/*..........................................................................*/
void Table_ctor(void) {
    QActive_ctor(&AO_Table.super, (QStateHandler)&Table_initial);
}
/*..........................................................................*/
QState Table_initial(Table *me) {
    return Q_TRAN(&Table_serving);
}
/*..........................................................................*/
QState Table_serving(Table *me) {
    QActive *philo;
    uint8_t n, m;

    switch (Q_SIG(me)) {
        case HUNGRY_SIG: {
            BSP_busyDelay();
            n = Q_PAR(me) - PHILO_0_PRIO;
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && ((me->isHungry & (1 << n)) == 0));

            BSP_displyPhilStat(n, "hungry  ");
            m = LEFT(n);
            if ((me->forks & ((1 << m) | (1 << n))) == 0) {
                me->forks |= ((1 << m) | (1 << n));         /* m and n used */
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + n].act);
                QActive_post(philo, EAT_SIG, n);
                BSP_displyPhilStat(n, "eating  ");
            }
            else {
                me->isHungry |= (1 << n);                       /* n hungry */
            }
            return Q_HANDLED();
        }
        case DONE_SIG: {
            BSP_busyDelay();
            n = Q_PAR(me) - PHILO_0_PRIO;
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && ((me->isHungry & (1 << n)) == 0));

            BSP_displyPhilStat(n, "thinking");
            m = LEFT(n);
                                /* compute the bitmask of positions m and n */
            m = ((1 << n) | (1 << m));
                                      /* both forks of Phil[n] must be used */
            Q_ASSERT((me->forks & m) == m);

            me->forks &= ~m;
            m = RIGHT(n);                       /* check the right neighbor */
            if (((me->isHungry & (1 << m)) != 0)
                && ((me->forks & (1 << m)) == 0))
            {
                me->forks |= ((1 << m) | (1 << n));         /* m and n used */
                me->isHungry &= ~(1 << m);                  /* m not hungry */
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + m].act);
                QActive_post(philo, EAT_SIG, m);
                BSP_displyPhilStat(m, "eating  ");
            }
            m = LEFT(n);                         /* check the left neighbor */
            n = LEFT(m);
            if (((me->isHungry & (1 << m)) != 0)
                && ((me->forks & (1 << n)) == 0))
            {
                me->forks |= ((1 << m) | (1 << n));         /* m and n used */
                me->isHungry &= ~(1 << m);                  /* m not hungry */
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + m].act);
                QActive_post(philo, EAT_SIG, m);
                BSP_displyPhilStat(m, "eating  ");
            }
            return Q_HANDLED();
        }
//        case TERMINATE_SIG: {
//            QF_stop();
//            return Q_HANDLED();
//        }
    }
    return Q_SUPER(&QHsm_top);
}

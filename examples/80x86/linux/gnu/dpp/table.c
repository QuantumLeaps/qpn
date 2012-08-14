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
    uint8_t fork[N_PHILO];
    uint8_t isHungry[N_PHILO];
} Table;

static QState Table_initial(Table *me);
static QState Table_serving(Table *me);

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1) % N_PHILO))
enum { FREE = 0, USED = 1 };

/* Global objects ----------------------------------------------------------*/
Table AO_Table;           /* the single instance of the Table active object */

/*..........................................................................*/
void Table_ctor(void) {
    QActive_ctor(&AO_Table.super, (QStateHandler)&Table_initial);
}
/*..........................................................................*/
QState Table_initial(Table *me) {
    uint8_t n;
    for (n = 0; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0;
    }
    return Q_TRAN(&Table_serving);
}
/*..........................................................................*/
QState Table_serving(Table *me) {
    QActive *philo;
    uint8_t  n, m;

    switch (Q_SIG(me)) {
        case HUNGRY_SIG: {
            BSP_busyDelay();
            n = (uint8_t)(Q_PAR(me) - PHILO_0_PRIO);
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displyPhilStat(n, "hungry  ");
            m = LEFT(n);
            if ((me->fork[m] == FREE) && (me->fork[n] == FREE)) {
                me->fork[m] = me->fork[n] = USED;
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + n].act);
                QActive_post(philo, EAT_SIG, n);
                BSP_displyPhilStat(n, "eating  ");
            }
            else {
                me->isHungry[n] = 1;
            }
            return Q_HANDLED();
        }
        case DONE_SIG: {
            BSP_busyDelay();
            n = (uint8_t)(Q_PAR(me) - PHILO_0_PRIO);
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displyPhilStat(n, "thinking");
            m = LEFT(n);
                                      /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = me->fork[n] = FREE;
            m = RIGHT(n);                       /* check the right neighbor */
            if (me->isHungry[m] && (me->fork[m] == FREE)) {
                me->fork[n] = me->fork[m] = USED;
                me->isHungry[m] = 0;
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + m].act);
                QActive_post(philo, EAT_SIG, m);
                BSP_displyPhilStat(m, "eating  ");
            }
            m = LEFT(n);                         /* check the left neighbor */
            n = LEFT(m);
            if (me->isHungry[m] && (me->fork[n] == FREE)) {
                me->fork[m] = USED;
                me->fork[n] = USED;
                me->isHungry[m] = 0;
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + m].act);
                QActive_post(philo, EAT_SIG, m);
                BSP_displyPhilStat(m, "eating  ");
            }
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            QF_stop();
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

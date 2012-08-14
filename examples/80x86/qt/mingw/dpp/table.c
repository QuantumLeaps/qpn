/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 26, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* Active object class -----------------------------------------------------*/
typedef struct TableTag {
    QActive super;
    uint8_t fork[N_PHILO];
    uint8_t isHungry[N_PHILO];
} Table;

static QState Table_initial(Table * const me);
static QState Table_active (Table * const me);
static QState Table_serving(Table * const me);
static QState Table_paused (Table * const me);

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1) % N_PHILO))
enum ForkState { FREE, USED };

/* Global objects ----------------------------------------------------------*/
Table AO_Table;           /* the single instance of the Table active object */

/*..........................................................................*/
void Table_ctor(void) {
    QActive_ctor(&AO_Table.super, (QStateHandler)&Table_initial);
}
/*..........................................................................*/
QState Table_initial(Table * const me) {
    uint8_t n;
    for (n = 0; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0;
    }
    return Q_TRAN(&Table_serving);
}
/*..........................................................................*/
QState Table_active(Table * const me) {
    switch (Q_SIG(me)) {
        case TERMINATE_SIG: {
            BSP_terminate();
            return Q_HANDLED();
        }
        case EAT_SIG: {
            Q_ERROR();
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Table_serving(Table * const me) {
    uint8_t n, m;
    QActive *philo;

    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            for (n = 0; n < N_PHILO; ++n) {   /* give permissions to eat... */
                if (me->isHungry[n]
                    && (me->fork[LEFT(n)] == FREE)
                    && (me->fork[n] == FREE))
                {
                    me->fork[LEFT(n)] = me->fork[n] = USED;
                    philo =
                        (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + n].act);
                    QActive_post(philo, EAT_SIG, n);
                    me->isHungry[n] = 0;
                    BSP_displayPhilStat(n, "eating  ");
                }
            }
            return Q_HANDLED();
        }
        case HUNGRY_SIG: {
            n = (uint8_t)(Q_PAR(me) - PHILO_0_PRIO);
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displayPhilStat(n, "hungry  ");
            m = LEFT(n);
            if ((me->fork[m] == FREE) && (me->fork[n] == FREE)) {
                me->fork[m] = me->fork[n] = USED;
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + n].act);
                QActive_post(philo, EAT_SIG, n);
                BSP_displayPhilStat(n, "eating  ");
            }
            else {
                me->isHungry[n] = 1;
            }
            return Q_HANDLED();
        }
        case DONE_SIG: {
            n = (uint8_t)(Q_PAR(me) - PHILO_0_PRIO);
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displayPhilStat(n, "thinking");
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
                BSP_displayPhilStat(m, "eating  ");
            }
            m = LEFT(n);                         /* check the left neighbor */
            n = LEFT(m);                  /* left fork of the left neighbor */
            if (me->isHungry[m] && (me->fork[n] == FREE)) {
                me->fork[m] = me->fork[n] = USED;
                me->isHungry[m] = 0;
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + m].act);
                QActive_post(philo, EAT_SIG, m);
                BSP_displayPhilStat(m, "eating  ");
            }
            return Q_HANDLED();
        }
        case PAUSE_SIG: {
            return Q_TRAN(&Table_paused);
        }
    }
    return Q_SUPER(&Table_active);
}
/*..........................................................................*/
QState Table_paused(Table * const me) {
    uint8_t n, m;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_displayPaused(1);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_displayPaused(0);
            return Q_HANDLED();
        }
        case PAUSE_SIG: {
            return Q_TRAN(&Table_serving);
        }
        case HUNGRY_SIG: {
            n = (uint8_t)(Q_PAR(me) - PHILO_0_PRIO);
             /* philo ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));
            me->isHungry[n] = 1;
            BSP_displayPhilStat(n, "hungry  ");
            return Q_HANDLED();
        }
        case DONE_SIG: {
            n = (uint8_t)(Q_PAR(me) - PHILO_0_PRIO);
              /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displayPhilStat(n, "thinking");
            m = LEFT(n);
            /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = me->fork[n] = FREE;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Table_active);
}

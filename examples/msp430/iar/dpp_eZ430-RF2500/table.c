/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 14, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
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
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"                                       /* QP-nano port */
#include "dpp.h"                                   /* application interface */
#include "bsp.h"                             /* Board Support Package (BSP) */

Q_DEFINE_THIS_MODULE("table")

/* Active object class -----------------------------------------------------*/
typedef struct TableTag {
    QMActive super;                                     /* inherit QMActive */
    uint8_t forks;
    uint8_t isHungry;
} Table;

static QState Table_initial(Table * const me);

static QState Table_serving  (Table * const me);
static QMState const Table_serving_ = {
    (QMState const *)0,
    Q_STATE_CAST(&Table_serving),
    Q_ACTION_CAST(0)
};

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1) % N_PHILO))

/* Global objects ----------------------------------------------------------*/
Table AO_Table;           /* the single instance of the Table active object */

/*..........................................................................*/
void Table_ctor(void) {
    QMActive_ctor(&AO_Table.super, Q_STATE_CAST(&Table_initial));
}

/* MSM code ----------------------------------------------------------------*/
/* NOTE: the following code has been written by hand, but it really
* is intended for automatic generation by the free QM modeling tool.
* The QMSM code generation will be supported starting with QM 3.0.0.
*/
/*..........................................................................*/
static QState Table_initial(Table * const me) {
    //static QActionHandler const act_[] = {
    //    Q_ACTION_CAST(0)
    //};
    return QM_INITIAL(&Table_serving_, QMsm_emptyAction_);
}
/*..........................................................................*/
static QState Table_serving(Table * const me) {
    QState status_;
    QActive *philo;
    uint8_t n, m;

    switch (Q_SIG(me)) {
        case HUNGRY_SIG: {
            BSP_busyDelay();
            n = Q_PAR(me) - PHILO_0_PRIO;
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && ((me->isHungry & (1 << n)) == 0));

            BSP_displayPhilStat(n, "h");
            m = LEFT(n);
            if ((me->forks & ((1 << m) | (1 << n))) == 0) {
                me->forks |= ((1 << m) | (1 << n));         /* m and n used */
                philo = (QActive *)Q_ROM_PTR(QF_active[PHILO_0_PRIO + n].act);
                QActive_post(philo, EAT_SIG, n);
                BSP_displayPhilStat(n, "e");
            }
            else {
                me->isHungry |= (1 << n);                       /* n hungry */
            }
            status_ = QM_HANDLED();
            break;
        }
        case DONE_SIG: {
            BSP_busyDelay();
            n = Q_PAR(me) - PHILO_0_PRIO;
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && ((me->isHungry & (1 << n)) == 0));

            BSP_displayPhilStat(n, "t");
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
                BSP_displayPhilStat(m, "e");
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
                BSP_displayPhilStat(m, "e");
            }
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}

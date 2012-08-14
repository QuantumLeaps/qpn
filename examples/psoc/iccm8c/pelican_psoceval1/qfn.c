/*****************************************************************************
* Product: QF-nano implemenation
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 24, 2012
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
#include "qpn_port.h"                                       /* QP-nano port */

Q_DEFINE_THIS_MODULE("qfn")

/**
* \file
* \ingroup qfn
* QF-nano implementation.
*/

/* Global-scope objects ----------------------------------------------------*/
uint8_t volatile QF_readySet_;                      /* ready-set of QF-nano */

/* local objects -----------------------------------------------------------*/
static uint8_t const Q_ROM Q_ROM_VAR l_pow2Lkup[] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
void QActive_post(QActive *me, QSignal sig, QParam par) {
#else
void QActive_post(QActive *me, QSignal sig) {
#endif
    QF_INT_LOCK();
    if (me->nUsed == (uint8_t)0) {
        ++me->nUsed;                             /* update number of events */

        Q_SIG(me) = sig;                      /* deliver the event directly */
#if (Q_PARAM_SIZE != 0)
        Q_PAR(me) = par;
#endif
        QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);    /* set the bit */

#ifdef QK_PREEMPTIVE
        QK_schedule_();                 /* check for synchronous preemption */
#endif
    }
    else {
            /* the queue must be able to accept the event (cannot overflow) */
        Q_ASSERT(me->nUsed <= Q_ROM_BYTE(QF_active[me->prio].end));
        ++me->nUsed;                             /* update number of events */
                                /* insert event into the ring buffer (FIFO) */
        ((QEvt *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].sig = sig;
#if (Q_PARAM_SIZE != 0)
        ((QEvt *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].par = par;
#endif
        if (me->head == (uint8_t)0) {
            me->head = Q_ROM_BYTE(QF_active[me->prio].end);/* wrap the head */
        }
        --me->head;
    }
    QF_INT_UNLOCK();
}
/*..........................................................................*/
#if (Q_PARAM_SIZE != 0)
void QActive_postISR(QActive *me, QSignal sig, QParam par)
#else
void QActive_postISR(QActive *me, QSignal sig)
#endif
{
#ifdef QF_ISR_NEST
#ifdef QF_ISR_KEY_TYPE
    QF_ISR_KEY_TYPE key;
    QF_ISR_LOCK(key);
#else
    QF_INT_LOCK();
#endif
#endif
    if (me->nUsed == (uint8_t)0) {
        ++me->nUsed;                             /* update number of events */

        Q_SIG(me) = sig;                      /* deliver the event directly */
#if (Q_PARAM_SIZE != 0)
        Q_PAR(me) = par;
#endif
        QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);    /* set the bit */
    }
    else {
            /* the queue must be able to accept the event (cannot overflow) */
        Q_ASSERT(me->nUsed <= Q_ROM_BYTE(QF_active[me->prio].end));
        ++me->nUsed;                             /* update number of events */
                                /* insert event into the ring buffer (FIFO) */
        ((QEvt *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].sig = sig;
#if (Q_PARAM_SIZE != 0)
        ((QEvt *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].par = par;
#endif
        if (me->head == (uint8_t)0) {
            me->head = Q_ROM_BYTE(QF_active[me->prio].end);/* wrap the head */
        }
        --me->head;
    }

#ifdef QF_ISR_NEST
#ifdef QF_ISR_KEY_TYPE
    QF_ISR_UNLOCK(key);
#else
    QF_INT_UNLOCK();
#endif
#endif
}

/*--------------------------------------------------------------------------*/
#if (QF_TIMEEVT_CTR_SIZE != 0)

/*..........................................................................*/
void QF_tick(void) {
    static uint8_t p;                /* declared static to save stack space */
    p = (uint8_t)QF_MAX_ACTIVE;
    do {
        static QActive *a;           /* declared static to save stack space */
        a = (QActive *)Q_ROM_PTR(QF_active[p].act);
        if (a->tickCtr != (QTimeEvtCtr)0) {
            if ((--a->tickCtr) == (QTimeEvtCtr)0) {
#if (Q_PARAM_SIZE != 0)
                QActive_postISR(a, (QSignal)Q_TIMEOUT_SIG, (QParam)0);
#else
                QActive_postISR(a, (QSignal)Q_TIMEOUT_SIG);
#endif
            }
        }
    } while ((--p) != (uint8_t)0);
}

#if (QF_TIMEEVT_CTR_SIZE > 1)
/*..........................................................................*/
void QActive_arm(QActive *me, QTimeEvtCtr tout) {
    QF_INT_LOCK();
    me->tickCtr = tout;
    QF_INT_UNLOCK();
}
/*..........................................................................*/
void QActive_disarm(QActive *me) {
    QF_INT_LOCK();
    me->tickCtr = (QTimeEvtCtr)0;
    QF_INT_UNLOCK();
}
#endif                                     /* #if (QF_TIMEEVT_CTR_SIZE > 1) */

#endif                                    /* #if (QF_TIMEEVT_CTR_SIZE != 0) */

/*--------------------------------------------------------------------------*/
#ifndef QK_PREEMPTIVE

int16_t QF_run(void) {
    static uint8_t const Q_ROM Q_ROM_VAR log2Lkup[] = {
        0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4
    };
    static uint8_t const Q_ROM Q_ROM_VAR invPow2Lkup[] = {
        0xFF, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
    };
    static QActive *a;               /* declared static to save stack space */
    static uint8_t p;                /* declared static to save stack space */

                         /* set priorities all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = (QActive *)Q_ROM_PTR(QF_active[p].act);
        Q_ASSERT(a != (QActive *)0);    /* QF_active[p] must be initialized */
        a->prio = p;               /* set the priority of the active object */
    }
         /* trigger initial transitions in all registered active objects... */
    for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
        a = (QActive *)Q_ROM_PTR(QF_active[p].act);
#ifndef QF_FSM_ACTIVE
        QHsm_init((QHsm *)a);         /* take the initial transition in HSM */
#else
        QFsm_init((QFsm *)a);         /* take the initial transition in FSM */
#endif
    }

    QF_onStartup();                              /* invoke startup callback */

    for (;;) {                      /* the event loop of the vanilla kernel */
        QF_INT_LOCK();
        if (QF_readySet_ != (uint8_t)0) {
#if (QF_MAX_ACTIVE > 4)
            if ((QF_readySet_ & 0xF0) != 0U) {        /* upper nibble used? */
                p = (uint8_t)(Q_ROM_BYTE(log2Lkup[QF_readySet_ >> 4]) + 4);
            }
            else                    /* upper nibble of QF_readySet_ is zero */
#endif
            {
                p = Q_ROM_BYTE(log2Lkup[QF_readySet_]);
            }
            QF_INT_UNLOCK();

            a = (QActive *)Q_ROM_PTR(QF_active[p].act);

#ifndef QF_FSM_ACTIVE
            QHsm_dispatch((QHsm *)a);                    /* dispatch to HSM */
#else
            QFsm_dispatch((QFsm *)a);                    /* dispatch to FSM */
#endif

            QF_INT_LOCK();
            if ((--a->nUsed) == (uint8_t)0) {      /* queue becoming empty? */
                QF_readySet_ &= Q_ROM_BYTE(invPow2Lkup[p]);/* clear the bit */
            }
            else {
                Q_SIG(a) = ((QEvt *)Q_ROM_PTR(QF_active[a->prio].queue))
                                [a->tail].sig;
#if (Q_PARAM_SIZE != 0)
                Q_PAR(a) = ((QEvt *)Q_ROM_PTR(QF_active[a->prio].queue))
                                [a->tail].par;
#endif
                if (a->tail == (uint8_t)0) {                /* wrap around? */
                    a->tail = Q_ROM_BYTE(QF_active[a->prio].end);
                }
                --a->tail;
            }
            QF_INT_UNLOCK();
        }
        else {
            QF_onIdle();                                      /* see NOTE01 */
        }
    }

    return (int16_t)0; /* this unreachable return is to make compiler happy */
}

#endif                                             /* #ifndef QK_PREEMPTIVE */

/*****************************************************************************
* NOTE01:
* QF_onIdle() must be called with interrupts locked because the determination
* of the idle condition (no events in the queues) can be changed by any
* interrupt. The QF_onIdle() MUST enable interrups internally, perhaps
* atomically with putting the CPU into a low-power mode.
*/

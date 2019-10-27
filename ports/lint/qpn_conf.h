/**
* @file
* @brief QP-nano configuration file, generic C compiler
* @cond
******************************************************************************
* Last updated for version 5.8.0
* Last updated on  2016-11-18
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
* http://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* \endcond
*/
#ifndef QPN_CONF_H
#define QPN_CONF_H

/*! The size (in bytes) of the single scalar parameter representation
* in the QEvent struct. Valid values: none (0), 1, 2, or 4; default none (0).
*/
/**
* \description
* This macro can be defined in the QP-nano port header file qpn_port.h to
* configure the parameter of Events. If the macro is not defined, the default
* of no event parameter will be chosen. The valid Q_PARAM_SIZE values of 1, 2,
* or 4, correspond to event parameters of uint8_t, uint16_t, and uint32_t,
* respectively.
*
* \sa ::QEvt, #Q_PAR(), QF_post(), QF_postNoLock()
*/
#define Q_PARAM_SIZE            4

/*! The size (in bytes) of the time event-counter representation in
* the QActive struct. Valid values: none (0), 1, 2, or 4; default none (0).
*/
/**
* \description
* This macro can be defined in the QP-nano port header file qpn_port.h to
* configure the internal tick counters of Time Events. If the macro is not
* defined, the default of no time event counter will be chosen. The valid
* QF_TIMEEVT_CTR_SIZE values of 1, 2, or 4, correspond to tick counters of
* uint8_t, uint16_t, and uint32_t, respectively. The tick counter
* representation determines the dynamic range of time delays that a
* Time Event can handle.
*
* \sa #QTimeEvtCtr
*/
#define QF_TIMEEVT_CTR_SIZE     2

/*! Configuration switch to enable/disable periodic time events. */
/**
* \description
* This macro determines if the QF-nano timer is capable of firing
* periodically or is limited to one-shot only. The reason for not allowing
* periodic time events is that they require additional storage (inside
* QActive) for the timeout interval.@n
* @n
* When the macro QF_TIMEEVT_PERIODIC is not defined, the function
* QActive_armX() takes 3 arguments: the active object pointer, the tick
* rate of the timer, and the one-shot timeout.@n
* @n
* When the macro QF_TIMEEVT_PERIODIC is not defined, the function
* QActive_armX() takes additional 4-th argument: the interval to re-arm
* the time event automatically until QActive_disarmX() is called.
*/
#define QF_TIMEEVT_PERIODIC

/*! Configuration switch to enable/disable keeping track of QF time event
usage. */
/**
* \description
* The QF-nano keeps track of the armed time events by means of the timer-sets
* (a separate one for each clock tick rate). The main use of the timer-sets
* is to quickly determine which time events at a given tick rate are still
* armed to enter the most appropriate low-power mode of the MCU.
*/
#define QF_TIMEEVT_USAGE

/*! The preprocessor switch to enable the QK-nano scheduler locking. */
/**
* \description
* QK-nano keeps supports selective scheduler locking up to a specified
* ceiling priority. This scheduler locking can be used, for example, as
* a non-blocking mutual exclusion mechanism (formerly QK-nano mutex).
*/
#define QK_SCHED_LOCK

#endif /* QPN_CONF_H */

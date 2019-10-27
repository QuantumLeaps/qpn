/**
* @file
* @brief QF-nano emulation for POSIX with cooperative QV kernel
* @cond
******************************************************************************
* Last Updated for Version: 6.3.7
* Date of the Last Update:  2018-11-09
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2018 Quantum Leaps, LLC. All rights reserved.
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
* https://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef QFN_PORT_H
#define QFN_PORT_H

/* interrupt disabling policy for task level, see NOTE1 */
#define QF_INT_DISABLE()     QF_enterCriticalSection_()
#define QF_INT_ENABLE()      QF_leaveCriticalSection_()

/* interrupt disabling policy for interrupt level */
/*#define QF_ISR_NEST*/ /* nesting of ISRs not allowed */

#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qvn.h"        /* QV-nano cooperative kernel public interface */

/* QF functions specific to the port */
void QF_enterCriticalSection_(void);
void QF_leaveCriticalSection_(void);

/* set clock tick rate and p-thread priority
* (NOTE ticksPerSec==0 disables the "ticker thread"
*/
void QF_setTickRate(uint32_t ticksPerSec, int_t tickPrio);

/* ISR-level clock tick callback
* (NOTE not called when "ticker thread" is not running)
*/
void QF_onClockTickISR(void);

/* application-level callback to cleanup the application */
void QF_onCleanup(void);

/* abstractions for console access... */
void QF_consoleSetup(void);
void QF_consoleCleanup(void);
int QF_consoleGetKey(void);
int QF_consoleWaitForKey(void);

/* NOTES: ********************************************************************
*
* NOTE1:
* QF-nano, like all small real-time kernels, needs to disable and enable
* interrupts to execute critical sections of code atomically. However,
* POSIX does not really allow disabling/enabling interrutps at the task
* level. Instead, this QF-nano emulation uses therefore a single package-scope
* p-thread mutex QF_pThreadMutex_ to protect all critical sections. The mutex
* is locked upon the entry to each critical sectioni and unlocked upon exit.
*
* Using the single mutex for all crtical section guarantees that only one
* thread at a time can execute inside a critical section. This prevents race
* conditions and data corruption.
*
* Note, however, that the mutex implementation of a critical section behaves
* differently than the standard interrupt locking. For example, the mutex
* approach might be subject to priority inversions. However, most p-thread
* mutex implementations, such as Linux p-threads, should support the
* priority-inheritance protocol.
*/

#endif /* QFN_PORT_H */

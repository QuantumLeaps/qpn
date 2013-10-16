/*****************************************************************************
* Product: QF-nano emulation for POSIX
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 12, 2013
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
#ifndef qfn_posix_h
#define qfn_posix_h

                 /* "interrupt disabling" policy for task level, see NOTE01 */
#define QF_INT_DISABLE()        pthread_mutex_lock(&QF_pThreadMutex_)
#define QF_INT_ENABLE()         pthread_mutex_unlock(&QF_pThreadMutex_)

                        /* "interrupt disabling" policy for interrupt level */
/*#define QF_ISR_NEST*/                      /* nesting of ISRs not allowed */


#include <pthread.h>                                    /* POSIX-thread API */
#include "qfn.h"                /* QF-nano platform-independent header file */

void QF_onCleanup(void);       /* "big" OS, such as Win32, requires cleanup */
void QF_setTickRate(uint32_t ticksPerSec);           /* set clock tick rate */
void QF_onClockTickISR(void);  /* clock tick callback (provided in the app) */

extern pthread_mutex_t QF_pThreadMutex_;   /* mutex for QF critical section */

/*****************************************************************************
* NOTE01:
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
#endif                                                       /* qfn_posix_h */

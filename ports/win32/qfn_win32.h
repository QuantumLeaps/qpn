/*****************************************************************************
* Product: QF-nano emulation for Win32
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 11, 2013
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
#ifndef qfn_win32_h
#define qfn_win32_h
                   /* interrupt disabling policy for task level, see NOTE01 */
#define QF_INT_DISABLE()        QF_enterCriticalSection()
#define QF_INT_ENABLE()         QF_leaveCriticalSection()

                          /* interrupt disabling policy for interrupt level */
/*#define QF_ISR_NEST*/                      /* nesting of ISRs not allowed */

#include "qfn.h"                /* QF-nano platform-independent header file */

void QF_enterCriticalSection(void);
void QF_leaveCriticalSection(void);
void QF_setTickRate(uint32_t ticksPerSec);           /* set clock tick rate */
void QF_onClockTickISR(void);   /* clock tick callback (defined in the app) */
void QF_onCleanup(void);   /* big OSes like Windows might require a cleanup */

/* NOTES: ********************************************************************
*
* NOTE01:
* QF-nano, like all real-time kernels, needs to execute certain sections of
* code indivisibly to avoid data corruption. The most straightforward way of
* protecting such critical sections of code is disabling and enabling
* interrupts, which Win32 does not allow.
*
* This QF-nano port uses internally a single Win32 critical section object
* (Win32 type CRITICAL_SECTION) to protect all critical sections.
*
* Using the single critical section object for all crtical section guarantees
* that only one thread at a time can execute inside a critical section. This
* prevents race conditions and data corruption.
*
* Please note, however, that the Win32 critical section implementation
* behaves differently than interrupt disabling. For example, the Win32
* critical section can potentially be subject to priority inversions. Various
* versions of Windows handle priority inversions differently, but it seems
* that most of them recognize priority inversions and dynamically adjust the
* priorities of threads to prevent it. Please refer to the MSN articles for
* more information.
*/

#endif                                                       /* qfn_win32_h */

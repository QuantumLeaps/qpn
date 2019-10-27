/**
* @file
* @brief QF-nano emulation for Win32 with cooperative QV kernel
* @ingroup ports
* @cond
******************************************************************************
* Last Updated for Version: 6.3.7
* Date of the Last Update:  2018-11-14
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

/* set clock tick rate (NOTE ticksPerSec==0 disables the "ticker thread") */
void QF_setTickRate(uint32_t ticksPerSec, int_t tickPrio);

/* ISR-level clock tick callback
* (NOTE not called when "ticker thread" is not running)
*/
void QF_onClockTickISR(void);

/* application-level callback to cleanup the application */
void QF_onCleanup(void);

/* special adaptations for QWIN GUI applications... */
#ifdef QWIN_GUI
    /* replace main() with main_gui() as the entry point to a GUI app. */
    #define main() main_gui()
    int_t main_gui(); /* prototype of the GUI application entry point */
#endif

/* abstractions for console access... */
void QF_consoleSetup(void);
void QF_consoleCleanup(void);
int QF_consoleGetKey(void);
int QF_consoleWaitForKey(void);

/****************************************************************************/
/* Microsoft C++: portable "safe" facilities from <stdio.h> and <string.h> */
#ifdef _MSC_VER

#if (_MSC_VER < 1900) /* before Visual Studio 2015 */
#define snprintf _snprintf
#endif

#define SNPRINTF_S(buf_, len_, format_, ...) \
    _snprintf_s(buf_, len_, _TRUNCATE, format_, ##__VA_ARGS__)

#define STRNCPY_S(dest_, src_, len_) \
    strncpy_s(dest_, len_, src_, _TRUNCATE)

#define FOPEN_S(fp_, fName_, mode_) \
    if (fopen_s(&fp_, fName_, mode_) != 0) { \
        fp_ = (FILE *)0; \
    } else (void)0

#define CTIME_S(buf_, len_, time_) \
    ctime_s((char *)buf_, len_, time_)

#define SSCANF_S(buf_, format_, ...) \
    sscanf_s(buf_, format_, ##__VA_ARGS__)

#else /* other C/C++ compilers (GNU, etc.) */

#define SNPRINTF_S(buf_, len_, format_, ...) \
    snprintf(buf_, len_, format_, ##__VA_ARGS__)

#define STRNCPY_S(dest_, src_, len_) strncpy(dest_, src_, len_)

#define FOPEN_S(fp_, fName_, mode_) \
    (fp_ = fopen(fName_, mode_))

#define CTIME_S(buf_, len_, time_) \
    strncpy((char *)buf_, ctime(time_), len_)

#define SSCANF_S(buf_, format_, ...) \
    sscanf(buf_, format_, ##__VA_ARGS__)

#endif /* _MSC_VER */


/* NOTES: ********************************************************************
*
* NOTE1:
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

#endif /* QFN_PORT_H */

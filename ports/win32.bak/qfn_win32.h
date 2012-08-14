/*****************************************************************************
* Product: QF-nano emulation for Win32
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Jun 29, 2012
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
#ifndef qfn_win32_h
#define qfn_win32_h
                     /* interrupt locking policy for task level, see NOTE01 */
#define QF_INT_DISABLE()        QF_enterCriticalSection()
#define QF_INT_ENABLE()         QF_leaveCriticalSection()

                            /* interrupt locking policy for interrupt level */
/* #define QF_ISR_NEST */                    /* nesting of ISRs not allowed */

#include "qfn.h"                /* QF-nano platform-independent header file */

void QF_enterCriticalSection(void);
void QF_leaveCriticalSection(void);
void QF_onCleanup(void);       /* "big" OS, such as Win32, requires cleanup */

/*****************************************************************************
* helper functions for drawing embedded front panels...
*/

void SetDlgItemBitmap(void *hWnd, uint16_t itemId, void *hBitmap);

enum DrawButtonAction {
    BTN_NOACTION,
    BTN_PAINTED,
    BTN_DEPRESSED,
    BTN_RELEASED
};

enum DrawButtonAction DrawButton(void const *pdis,
                void const *hBitmapUp, void const *hBitmapDwn);
void DrawBitmap(void const *hdc, void const *hBitmap,
                int xStart, int yStart);

       /* DIB24 "class" is for drawing graphic LCDs with up to 24-bit color */
typedef struct DIB24Tag {
    uint16_t width;
    uint16_t xScale;
    uint16_t height;
    uint16_t yScale;
    void    *hBitmap;
    void    *hItem;
    uint8_t *bits;
    uint8_t  bgColor[3];
} DIB24;

void DIB24_ctor(DIB24 * const me,
                uint16_t width,  uint16_t xScale,
                uint16_t height, uint16_t yScale,
                void *hItem, uint8_t const bgColor[3]);
void DIB24_xtor(DIB24 * const me);
void DIB24_clear(DIB24 * const me);
void DIB24_setAt(DIB24 * const me, uint16_t x, uint16_t y,
                 uint8_t const color[3]);
void DIB24_clearAt(DIB24 * const me, uint16_t x, uint16_t y);
void DIB24_redraw(DIB24 * const me);

/* NOTES: ********************************************************************
*
* NOTE01:
* QF, like all real-time frameworks, needs to execute certain sections of
* code indivisibly to avoid data corruption. The most straightforward way of
* protecting such critical sections of code is disabling and enabling
* interrupts, which Win32 does not allow.
*
* This QF port uses therefore a single package-scope Win32 critical section
* object QF_win32CritSect_ to protect all critical sections.
*
* Using the single critical section object for all crtical section guarantees
* that only one thread at a time can execute inside a critical section. This
* prevents race conditions and data corruption.
*
* Please note, however, that the Win32 critical section implementation
* behaves differently than interrupt locking. A common Win32 critical section
* ensures that only one thread at a time can execute a critical section, but
* it does not guarantee that a context switch cannot occur within the
* critical section. In fact, such context switches probably will happen, but
* they should not cause concurrency hazards because the critical section
* eliminates all race conditionis.
*
* Unlinke simply disabling and enabling interrupts, the critical section
* approach is also subject to priority inversions. Various versions of
* Windows handle priority inversions differently, but it seems that most of
* them recognize priority inversions and dynamically adjust the priorities of
* threads to prevent it. Please refer to the MSN articles for more
* information.
*/

#endif                                                       /* qfn_win32_h */

/*****************************************************************************
* Product: DPP example, 80x86, Win32-1T
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
#include "qpn_port.h"
#include "dpp.h"
#include "bsp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>                                           /* Win32 API */

#include <stdio.h>
#include "resource.h"

Q_DEFINE_THIS_FILE

/* Global objects ----------------------------------------------------------*/

/* local variables ---------------------------------------------------------*/
static HINSTANCE l_hInst;                      /* this application instance */
static HWND      l_hWnd;                              /* main window handle */
static HBITMAP   l_hThinking;
static HBITMAP   l_hHungry;
static HBITMAP   l_hEating;
static HBITMAP   l_hBTN_UP;
static HBITMAP   l_hBTN_DWN;

static uint8_t   l_running;
static unsigned  l_rnd;                                      /* random seed */

static int const c_PhiloId[] = {
    IDC_PHILO_0,
    IDC_PHILO_1,
    IDC_PHILO_2,
    IDC_PHILO_3,
    IDC_PHILO_4
};

static BOOL CALLBACK dppDlg(HWND hWnd, UINT iMsg,
                            WPARAM wParam, LPARAM lParam);

/*..........................................................................*/
static DWORD WINAPI isrThread(LPVOID par) {  /* Win32 thred to emulate ISRs */
    (void)par;                                          /* unused parameter */

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    l_running = (uint8_t)1;
    while (l_running) {
        Sleep(1000UL / BSP_TICKS_PER_SEC);      /* wait for the tick period */
        QF_tickISR();          /* perform the QF-nano clock tick processing */
    }
    return 0;                                             /* return success */
}
/*..........................................................................*/
static DWORD WINAPI QF_app(LPVOID par) {
    extern int main(void);
    (void)par;                                          /* unused parameter */
    return main();                                /* run the QF application */
}
/*..........................................................................*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
                   LPSTR cmdLine, int iCmdShow)
{
    (void)hPrevInst;
    (void)iCmdShow;

    l_hInst = hInst;                       /* save the application instance */

    BSP_init(cmdLine);              /* initialize the Board Support Package */

       /* create the modal dialog box that is the GUI of the appliction ... */
    return DialogBox(l_hInst, MAKEINTRESOURCE(IDD_APPLICATION),
                     NULL, &dppDlg);
}
/*..........................................................................*/
static BOOL CALLBACK dppDlg(HWND hWnd, UINT iMsg,
                            WPARAM wParam, LPARAM lParam)
{
    switch (iMsg) {
        case WM_INITDIALOG: {
            l_hWnd = hWnd;                   /* save the main window hanlde */

            /* set the application icons... */
            SetClassLong(hWnd, GCL_HICON,
                         (LONG)LoadIcon(l_hInst, MAKEINTRESOURCE(IDI_QP)));
            SetClassLong(hWnd, GCL_HICONSM,
                         (LONG)LoadIcon(l_hInst, MAKEINTRESOURCE(IDI_QP)));

            l_hThinking = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_THINKING));
            l_hHungry   = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_HUNGRY));
            l_hEating   = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_EATING));
            l_hBTN_UP   = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_BTN_UP));
            l_hBTN_DWN  = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_BTN_DWN));

            /* --> QP: spawn the thread to run the QF application... */
            Q_ALLEGE(CreateThread(NULL, 0, &QF_app, NULL, 0, NULL)
                     != (HANDLE)0);
            break;
        }

        /* commands from regular buttons and menus... */ 
        case WM_COMMAND: {
            switch (wParam) {
                case IDOK:
                case IDCANCEL: {
                    QActive_post((QActive *)&AO_Table, TERMINATE_SIG, 0U);
                    break;
                }
            }
            break;
        }

        /* user-drawn buttons... */
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
            switch (pdis->CtlID) {
                case IDC_PAUSE: {               /* PAUSE owner-drawn button */
                    switch (DrawButton(pdis, l_hBTN_UP, l_hBTN_DWN)) {
                        case BTN_DEPRESSED: {
                            QActive_post((QActive *)&AO_Table, PAUSE_SIG, 0U);
                            break;
                        }
                        case BTN_RELEASED: {
                            QActive_post((QActive *)&AO_Table, PAUSE_SIG, 0U);
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    return FALSE;
}
/*..........................................................................*/
int BSP_init(char *cmdLine) {
    (void)cmdLine;
    BSP_randomSeed(1234);
    return 1;
}
/*..........................................................................*/
void QF_onStartup(void) {
                                                   /* create the ISR thread */
    Q_ALLEGE(CreateThread(NULL, 1024, &isrThread, 0, 0, NULL)
             != NULL);                        /* ISR thread must be created */
}
/*..........................................................................*/
void QF_onIdle(void) {                    /* called within critical section */
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QF_onCleanup(void) {
    l_running = (uint8_t)0;
}
/*..........................................................................*/
void BSP_displayPhilStat(uint8_t n, char_t const *stat) {
    HWND item;
    HBITMAP hBitmap = l_hThinking;

    Q_REQUIRE(n < Q_DIM(c_PhiloId));

    item = GetDlgItem(l_hWnd, c_PhiloId[n]);
    switch (stat[0]) {
        case 't': hBitmap = l_hThinking; break;
        case 'h': hBitmap = l_hHungry;   break;
        case 'e': hBitmap = l_hEating;   break;
        default: Q_ERROR(); break;
    }
    SendMessage(item, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
}
/*..........................................................................*/
void BSP_displayPaused(uint8_t paused) {
    char buf[16];
    LoadString(l_hInst,
        (paused != 0U) ? IDS_PAUSED : IDS_RUNNING, buf, Q_DIM(buf));
    SetDlgItemText(l_hWnd, IDC_PAUSED, buf);
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    QF_stop();
    EndDialog(l_hWnd, result);
}
/*..........................................................................*/
uint32_t BSP_random(void) {  /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    char message[80];
    _snprintf_s(message, Q_DIM(message) - 1, _TRUNCATE,
                "Assertion failed in module %hs line %d", file, line);
    MessageBox(l_hWnd, message, "!!! ASSERTION !!!",
               MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
    BSP_terminate(-1);
}

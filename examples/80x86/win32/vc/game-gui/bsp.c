/*****************************************************************************
* Product: "Fly 'n' Shoot" game example, 80x86, Win32
* Last Updated for Version: 4.5.01
* Date of the Last Update:  Jun 28, 2012
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
#include "game.h"
#include "bsp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>                                           /* Win32 API */

#include <stdio.h>
#include <time.h>
#include "resource.h"

Q_DEFINE_THIS_FILE

#define LCD_WIDTH     BSP_SCREEN_WIDTH
#define LCD_HEIGHT    BSP_SCREEN_HEIGHT
#define LCD_X_SCALE   2
#define LCD_Y_SCALE   2

/* local variables ---------------------------------------------------------*/
static HINSTANCE l_hInst;                      /* this application instance */
static HWND      l_hWnd;                              /* main window handle */
static LPSTR     l_cmdLine;                      /* the command line string */

static HBITMAP   l_hBTN_UP;
static HBITMAP   l_hBTN_DWN;
static HBITMAP   l_hLED_OFF;
static HBITMAP   l_hLED_ON;
static DIB24     l_lcd;
static uint8_t   const c_offColor[] = {  15U,  15U,  15U };
static uint8_t   const c_onColor [] = { 255U, 255U,   0U };
static uint8_t   l_ship_pos = GAME_SHIP_Y;

static uint8_t   l_running;

static BOOL CALLBACK appDlg(HWND hWnd, UINT iMsg,
                            WPARAM wParam, LPARAM lParam);
int main(void);

/*..........................................................................*/
static DWORD WINAPI isrThread(LPVOID par) {  /* Win32 thred to emulate ISRs */
    (void)par;                                          /* unused parameter */

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    l_running = (uint8_t)1;
    while (l_running) {
        Sleep(1000UL / BSP_TICKS_PER_SEC);      /* wait for the tick period */

        QF_tickISR();                      /* process all armed time events */

               /* post TIME_TICK events to all interested active objects... */
        QActive_postISR((QActive *)&AO_Tunnel,  TIME_TICK_SIG, 0U);
        QActive_postISR((QActive *)&AO_Ship,    TIME_TICK_SIG, 0U);
        QActive_postISR((QActive *)&AO_Missile, TIME_TICK_SIG, 0U);
    }
    return 0;                                             /* return success */
}
/*..........................................................................*/
static DWORD WINAPI QF_app(LPVOID par) {
    (void)par;                                          /* unused parameter */
    return main();                                /* run the QF application */
}
/*..........................................................................*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
                   LPSTR cmdLine, int iCmdShow)
{
    (void)hPrevInst;
    (void)iCmdShow;

    l_hInst   = hInst;                     /* save the application instance */
    l_cmdLine = cmdLine;                    /* save the command line string */

       /* create the modal dialog box that is the GUI of the appliction ... */
    return DialogBox(l_hInst, MAKEINTRESOURCE(IDD_APPLICATION),
                     NULL, &appDlg);
}
/*..........................................................................*/
static BOOL CALLBACK appDlg(HWND hWnd, UINT iMsg,
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

            /* prepare the LCD display */
            DIB24_ctor(&l_lcd,
                       LCD_WIDTH, LCD_X_SCALE, LCD_HEIGHT, LCD_Y_SCALE,
                       GetDlgItem(hWnd, IDC_LCD), c_offColor);

            /* pre-load bitmaps for the user-drawn buttons... */
            l_hBTN_UP  = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_BTN_UP));
            l_hBTN_DWN = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_BTN_DWN));
            l_hLED_OFF = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_LED_OFF));
            l_hLED_ON  = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_LED_ON));

            BSP_updateScore(0);

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
                    //QActive_postISR((QActive *)&AO_tunnel,
                    //                TIME_TICK_SIG, 0U);
                    BSP_terminate(0);
                    break;
                }
            }
            break;
        }

        /* user-drawn buttons... */
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
            switch (pdis->CtlID) {
                case IDC_USER: {                 /* USER owner-drawn button */
                    switch (DrawButton(pdis, l_hBTN_UP, l_hBTN_DWN)) {
                        case BTN_DEPRESSED: {
                            BSP_playerTrigger();
                            break;
                        }
                        case BTN_RELEASED: {
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }

        /* mouse input... */
        case WM_MOUSEWHEEL: {
            if ((HIWORD(wParam) & 0x8000U) == 0U) {
                BSP_moveShipUp();
            }
            else {
                BSP_moveShipDown();
            }
            break;
        }
    }
    return FALSE;
}
/*..........................................................................*/
void BSP_init(void) {
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    QF_stop();
    EndDialog(l_hWnd, result);
}
/*..........................................................................*/
void QF_onStartup(void) {
                                                   /* create the ISR thread */
    Q_ALLEGE(CreateThread(NULL, 1024, &isrThread, 0, 0, NULL)
             != NULL);                        /* ISR thread must be created */
}
/*..........................................................................*/
void QF_onCleanup(void) {
    l_running = (uint8_t)0;
}
/*..........................................................................*/
void QF_onIdle(void) {                   /* called within critical section! */
    QF_INT_ENABLE();
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

/*..........................................................................*/
void BSP_drawBitmap(uint8_t const *bitmap) {
    uint16_t x, y;
    for (y = 0; y < LCD_HEIGHT; ++y) {
        for (x = 0; x < LCD_WIDTH; ++x) {
            uint8_t bits = bitmap[x + (y/8U)*LCD_WIDTH];
            if ((bits & (1U << (y & 0x07U))) != 0U) {
                DIB24_setAt(&l_lcd, x, y, c_onColor);
            }
            else {
                DIB24_clearAt(&l_lcd, x, y);
            }
        }
    }
    DIB24_redraw(&l_lcd);
}
/*..........................................................................*/
void BSP_drawNString(uint8_t x, uint8_t y, char const *str) {
    static uint8_t const font5x7[95][5] = {
        { 0x00, 0x00, 0x00, 0x00, 0x00 },                            /* ' ' */
        { 0x00, 0x00, 0x4F, 0x00, 0x00 },                              /* ! */
        { 0x00, 0x07, 0x00, 0x07, 0x00 },                              /* " */
        { 0x14, 0x7F, 0x14, 0x7F, 0x14 },                              /* # */
        { 0x24, 0x2A, 0x7F, 0x2A, 0x12 },                              /* $ */
        { 0x23, 0x13, 0x08, 0x64, 0x62 },                              /* % */
        { 0x36, 0x49, 0x55, 0x22, 0x50 },                              /* & */
        { 0x00, 0x05, 0x03, 0x00, 0x00 },                              /* ' */
        { 0x00, 0x1C, 0x22, 0x41, 0x00 },                              /* ( */
        { 0x00, 0x41, 0x22, 0x1C, 0x00 },                              /* ) */
        { 0x14, 0x08, 0x3E, 0x08, 0x14 },                              /* * */
        { 0x08, 0x08, 0x3E, 0x08, 0x08 },                              /* + */
        { 0x00, 0x50, 0x30, 0x00, 0x00 },                              /* , */
        { 0x08, 0x08, 0x08, 0x08, 0x08 },                              /* - */
        { 0x00, 0x60, 0x60, 0x00, 0x00 },                              /* . */
        { 0x20, 0x10, 0x08, 0x04, 0x02 },                              /* / */
        { 0x3E, 0x51, 0x49, 0x45, 0x3E },                              /* 0 */
        { 0x00, 0x42, 0x7F, 0x40, 0x00 },                              /* 1 */
        { 0x42, 0x61, 0x51, 0x49, 0x46 },                              /* 2 */
        { 0x21, 0x41, 0x45, 0x4B, 0x31 },                              /* 3 */
        { 0x18, 0x14, 0x12, 0x7F, 0x10 },                              /* 4 */
        { 0x27, 0x45, 0x45, 0x45, 0x39 },                              /* 5 */
        { 0x3C, 0x4A, 0x49, 0x49, 0x30 },                              /* 6 */
        { 0x01, 0x71, 0x09, 0x05, 0x03 },                              /* 7 */
        { 0x36, 0x49, 0x49, 0x49, 0x36 },                              /* 8 */
        { 0x06, 0x49, 0x49, 0x29, 0x1E },                              /* 9 */
        { 0x00, 0x36, 0x36, 0x00, 0x00 },                              /* : */
        { 0x00, 0x56, 0x36, 0x00, 0x00 },                              /* ; */
        { 0x08, 0x14, 0x22, 0x41, 0x00 },                              /* < */
        { 0x14, 0x14, 0x14, 0x14, 0x14 },                              /* = */
        { 0x00, 0x41, 0x22, 0x14, 0x08 },                              /* > */
        { 0x02, 0x01, 0x51, 0x09, 0x06 },                              /* ? */
        { 0x32, 0x49, 0x79, 0x41, 0x3E },                              /* @ */
        { 0x7E, 0x11, 0x11, 0x11, 0x7E },                              /* A */
        { 0x7F, 0x49, 0x49, 0x49, 0x36 },                              /* B */
        { 0x3E, 0x41, 0x41, 0x41, 0x22 },                              /* C */
        { 0x7F, 0x41, 0x41, 0x22, 0x1C },                              /* D */
        { 0x7F, 0x49, 0x49, 0x49, 0x41 },                              /* E */
        { 0x7F, 0x09, 0x09, 0x09, 0x01 },                              /* F */
        { 0x3E, 0x41, 0x49, 0x49, 0x7A },                              /* G */
        { 0x7F, 0x08, 0x08, 0x08, 0x7F },                              /* H */
        { 0x00, 0x41, 0x7F, 0x41, 0x00 },                              /* I */
        { 0x20, 0x40, 0x41, 0x3F, 0x01 },                              /* J */
        { 0x7F, 0x08, 0x14, 0x22, 0x41 },                              /* K */
        { 0x7F, 0x40, 0x40, 0x40, 0x40 },                              /* L */
        { 0x7F, 0x02, 0x0C, 0x02, 0x7F },                              /* M */
        { 0x7F, 0x04, 0x08, 0x10, 0x7F },                              /* N */
        { 0x3E, 0x41, 0x41, 0x41, 0x3E },                              /* O */
        { 0x7F, 0x09, 0x09, 0x09, 0x06 },                              /* P */
        { 0x3E, 0x41, 0x51, 0x21, 0x5E },                              /* Q */
        { 0x7F, 0x09, 0x19, 0x29, 0x46 },                              /* R */
        { 0x46, 0x49, 0x49, 0x49, 0x31 },                              /* S */
        { 0x01, 0x01, 0x7F, 0x01, 0x01 },                              /* T */
        { 0x3F, 0x40, 0x40, 0x40, 0x3F },                              /* U */
        { 0x1F, 0x20, 0x40, 0x20, 0x1F },                              /* V */
        { 0x3F, 0x40, 0x38, 0x40, 0x3F },                              /* W */
        { 0x63, 0x14, 0x08, 0x14, 0x63 },                              /* X */
        { 0x07, 0x08, 0x70, 0x08, 0x07 },                              /* Y */
        { 0x61, 0x51, 0x49, 0x45, 0x43 },                              /* Z */
        { 0x00, 0x7F, 0x41, 0x41, 0x00 },                              /* [ */
        { 0x02, 0x04, 0x08, 0x10, 0x20 },                              /* \ */
        { 0x00, 0x41, 0x41, 0x7F, 0x00 },                              /* ] */
        { 0x04, 0x02, 0x01, 0x02, 0x04 },                              /* ^ */
        { 0x40, 0x40, 0x40, 0x40, 0x40 },                              /* _ */
        { 0x00, 0x01, 0x02, 0x04, 0x00 },                              /* ` */
        { 0x20, 0x54, 0x54, 0x54, 0x78 },                              /* a */
        { 0x7F, 0x48, 0x44, 0x44, 0x38 },                              /* b */
        { 0x38, 0x44, 0x44, 0x44, 0x20 },                              /* c */
        { 0x38, 0x44, 0x44, 0x48, 0x7F },                              /* d */
        { 0x38, 0x54, 0x54, 0x54, 0x18 },                              /* e */
        { 0x08, 0x7E, 0x09, 0x01, 0x02 },                              /* f */
        { 0x0C, 0x52, 0x52, 0x52, 0x3E },                              /* g */
        { 0x7F, 0x08, 0x04, 0x04, 0x78 },                              /* h */
        { 0x00, 0x44, 0x7D, 0x40, 0x00 },                              /* i */
        { 0x20, 0x40, 0x44, 0x3D, 0x00 },                              /* j */
        { 0x7F, 0x10, 0x28, 0x44, 0x00 },                              /* k */
        { 0x00, 0x41, 0x7F, 0x40, 0x00 },                              /* l */
        { 0x7C, 0x04, 0x18, 0x04, 0x78 },                              /* m */
        { 0x7C, 0x08, 0x04, 0x04, 0x78 },                              /* n */
        { 0x38, 0x44, 0x44, 0x44, 0x38 },                              /* o */
        { 0x7C, 0x14, 0x14, 0x14, 0x08 },                              /* p */
        { 0x08, 0x14, 0x14, 0x18, 0x7C },                              /* q */
        { 0x7C, 0x08, 0x04, 0x04, 0x08 },                              /* r */
        { 0x48, 0x54, 0x54, 0x54, 0x20 },                              /* s */
        { 0x04, 0x3F, 0x44, 0x40, 0x20 },                              /* t */
        { 0x3C, 0x40, 0x40, 0x20, 0x7C },                              /* u */
        { 0x1C, 0x20, 0x40, 0x20, 0x1C },                              /* v */
        { 0x3C, 0x40, 0x30, 0x40, 0x3C },                              /* w */
        { 0x44, 0x28, 0x10, 0x28, 0x44 },                              /* x */
        { 0x0C, 0x50, 0x50, 0x50, 0x3C },                              /* y */
        { 0x44, 0x64, 0x54, 0x4C, 0x44 },                              /* z */
        { 0x00, 0x08, 0x36, 0x41, 0x00 },                              /* { */
        { 0x00, 0x00, 0x7F, 0x00, 0x00 },                              /* | */
        { 0x00, 0x41, 0x36, 0x08, 0x00 },                              /* } */
        { 0x02, 0x01, 0x02, 0x04, 0x02 },                              /* ~ */
    };
    int dx, dy;

    while (*str != '\0') {
        uint8_t const *ch = &font5x7[*str - ' '][0];
        for (dx = 0U; dx < 5U; ++dx) {
            for (dy = 0U; dy < 8U; ++dy) {
                if ((ch[dx] & (1U << dy)) != 0U) {
                    DIB24_setAt(&l_lcd, (uint16_t)(x + dx),
                                        (uint16_t)(y*8U + dy), c_onColor);
                }
                else {
                    DIB24_clearAt(&l_lcd, (uint16_t)(x + dx),
                                          (uint16_t)(y*8U + dy));
                }
            }
        }
        ++str;
        x += 6;
    }
    DIB24_redraw(&l_lcd);
}
/*..........................................................................*/
void BSP_updateScore(uint16_t score) {
    static HBITMAP segments[10];

    if (segments[0] == NULL) {                        /* first time through */
        segments[0] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG0));
        segments[1] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG1));
        segments[2] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG2));
        segments[3] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG3));
        segments[4] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG4));
        segments[5] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG5));
        segments[6] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG6));
        segments[7] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG7));
        segments[8] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG8));
        segments[9] = LoadBitmap(l_hInst, MAKEINTRESOURCE(IDB_SEG9));
    }

    /* update the score in a traitional static control */
    SetDlgItemInt(l_hWnd, IDC_SCORE, score, FALSE);

    /* update the score in a 7-segment display */
    SetDlgItemBitmap(l_hWnd, IDC_SEG0, segments[score % 10]);
    score /= 10U;
    SetDlgItemBitmap(l_hWnd, IDC_SEG1, segments[score % 10]);
    score /= 10U;
    SetDlgItemBitmap(l_hWnd, IDC_SEG2, segments[score % 10]);
    score /= 10U;
    SetDlgItemBitmap(l_hWnd, IDC_SEG3, segments[score % 10]);
}
/*..........................................................................*/
void BSP_displayOn(void) {
    SetDlgItemBitmap(l_hWnd, IDC_LED, l_hLED_OFF);
}
/*..........................................................................*/
void BSP_displayOff(void) {
    SetDlgItemBitmap(l_hWnd, IDC_LED, l_hLED_ON);
    DIB24_clear(&l_lcd);
    DIB24_redraw(&l_lcd);
}
/*..........................................................................*/
void BSP_playerTrigger(void) {
    QActive_post((QActive *)&AO_Ship,   PLAYER_TRIGGER_SIG, 0U);
    QActive_post((QActive *)&AO_Tunnel, PLAYER_TRIGGER_SIG, 0U);
}
/*..........................................................................*/
void BSP_moveShipUp(void) {
    if (l_ship_pos > 0x00) {
        --l_ship_pos;
    }
    QActive_postISR((QActive *)&AO_Ship, PLAYER_SHIP_MOVE_SIG,
                    ((l_ship_pos << 8) | GAME_SHIP_X));
}
//............................................................................
void BSP_moveShipDown(void) {
    if (l_ship_pos < (GAME_SCREEN_HEIGHT - 3)) {
        ++l_ship_pos;
    }
    QActive_postISR((QActive *)&AO_Ship, PLAYER_SHIP_MOVE_SIG,
                    ((l_ship_pos << 8) | GAME_SHIP_X));
}

/*****************************************************************************
* Product: Win32 utilities for embedded front panels 
* Last Updated for Version: 4.5.02
* Date of the Last Update:  Jun 30, 2012
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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>                                           /* Win32 API */

#include "qpn_port.h"                                       /* QP-nano port */

/*..........................................................................*/
void SetDlgItemBitmap(void *hWnd, uint16_t itemId, void *hBitmap) {
    SendMessage(GetDlgItem(hWnd, (int)itemId), STM_SETIMAGE,
                IMAGE_BITMAP, (LPARAM)hBitmap);
}
/*..........................................................................*/
/* DrawBitmap() function adapted from the book "Programming Windows" by
* Charles Petzold.
*/
void DrawBitmap(void const *hdc, void const *hBitmap,
                int xStart, int yStart)
{
    BITMAP bm;
    POINT  ptSize, ptOrg;
    HDC    hdcMem = CreateCompatibleDC((HDC)hdc);

    SelectObject(hdcMem, (HBITMAP)hBitmap);
    SetMapMode(hdcMem, GetMapMode((HDC)hdc));

    GetObject((HBITMAP)hBitmap, sizeof(BITMAP), (LPVOID)&bm);
    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;
    DPtoLP((HDC)hdc, &ptSize, 1);

    ptOrg.x = 0;
    ptOrg.y = 0;
    DPtoLP(hdcMem, &ptOrg, 1);

    BitBlt((HDC)hdc, xStart, yStart, ptSize.x, ptSize.y,
           hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);
    DeleteDC(hdcMem);
}
/*..........................................................................*/
enum DrawButtonAction DrawButton(void const *pdis,
                          void const *hBitmapUp, void const *hBitmapDwn)
{
    enum DrawButtonAction ret = BTN_NOACTION;
    LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)pdis;

    if ((lpdis->itemAction & ODA_DRAWENTIRE) != 0U) {
        DrawBitmap(lpdis->hDC, hBitmapUp,
                   lpdis->rcItem.left, lpdis->rcItem.top);
        ret = BTN_PAINTED;
    }
    else if ((lpdis->itemAction & ODA_SELECT) != 0U) {
        if ((lpdis->itemState & ODS_SELECTED) != 0U) {
            DrawBitmap(lpdis->hDC, hBitmapDwn,
                       lpdis->rcItem.left, lpdis->rcItem.top);
            ret = BTN_DEPRESSED;
        }
        else {
            DrawBitmap(lpdis->hDC, hBitmapUp,
                       lpdis->rcItem.left, lpdis->rcItem.top);
            ret = BTN_RELEASED;
        }
    }
    return ret;
}

/*--------------------------------------------------------------------------*/
void DIB24_ctor(DIB24 * const me,
                uint16_t width,  uint16_t xScale,
                uint16_t height, uint16_t yScale,
                void *hItem, uint8_t const bgColor[3])
{
    HDC hDC;
    BITMAPINFO bi24BitInfo;

    me->width  = width;
    me->xScale = xScale;
    me->height = height;
    me->yScale = yScale;

    me->bgColor[0] = bgColor[0];
    me->bgColor[1] = bgColor[1];
    me->bgColor[2] = bgColor[2];

    me->hItem = hItem;

    bi24BitInfo.bmiHeader.biBitCount    = 3U*8U;             /* 3 RGB bytes */
    bi24BitInfo.bmiHeader.biCompression = BI_RGB;              /* RGB color */
    bi24BitInfo.bmiHeader.biPlanes      = 1U;
    bi24BitInfo.bmiHeader.biSize        = sizeof(bi24BitInfo.bmiHeader);
    bi24BitInfo.bmiHeader.biWidth       = me->width  * me->xScale;
    bi24BitInfo.bmiHeader.biHeight      = me->height * me->yScale;

    hDC = CreateCompatibleDC(NULL);
    me->hBitmap = CreateDIBSection(hDC, &bi24BitInfo, DIB_RGB_COLORS,
                                   (void **)&me->bits, 0, 0);
    DeleteDC(hDC);

    DIB24_clear(me);
    DIB24_redraw(me);
}
/*..........................................................................*/
void DIB24_xtor(DIB24 * const me) {
    DeleteObject(me->hBitmap);
}
/*..........................................................................*/
void DIB24_clear(DIB24 * const me) {
    unsigned n;
    uint8_t r = me->bgColor[0];
    uint8_t g = me->bgColor[1];
    uint8_t b = me->bgColor[2];
    uint8_t *bits = me->bits;

    for (n = me->width*me->xScale * me->height * me->yScale;
         n != 0;
         --n, bits += 3)
    {
        bits[0] = b;
        bits[1] = g;
        bits[2] = r;
    }
}
/*..........................................................................*/
void DIB24_setAt(DIB24 * const me, uint16_t x, uint16_t y,
                 uint8_t const color[3])
{
    uint16_t sx, sy;
    uint8_t *pixelRGB =
        &me->bits[3*(me->xScale*x
                  + me->xScale*me->width * me->yScale*(me->height - 1U - y))];
    uint8_t r = color[0];
    uint8_t g = color[1];
    uint8_t b = color[2];
    for (sy = me->yScale; sy != 0U;
         --sy, pixelRGB += me->xScale*me->width*3U)
    {
        for (sx = 3*me->xScale; sx != 0U; sx -= 3U) {
            pixelRGB[sx - 3U] = b;
            pixelRGB[sx - 2U] = g;
            pixelRGB[sx - 1U] = r;
        }
    }
}
/*..........................................................................*/
void DIB24_clearAt(DIB24 * const me, uint16_t x, uint16_t y) {
    DIB24_setAt(me, x, y, me->bgColor);
}
/*..........................................................................*/
void DIB24_redraw(DIB24 * const me) {
    SendMessage(me->hItem, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)me->hBitmap);
}

/*****************************************************************************
* Product: "Fly'n'Shoot" game, BSP for Qt (EK-LM3S811 simulation)
* Last Updated for Version: 4.5.01
* Date of the Last Update:  Jun 14, 2012
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
#include <QtGui>
#include "gui.h"
#include "qpn_app.h"                   // Qt Application for emulating QP-nano
//-----------------
extern "C" {

#include "qpn_port.h"
#include "game.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* adjust the following constants to your LCD... */
static int const LCD_WIDTH     = BSP_SCREEN_WIDTH;
static int const LCD_HEIGHT    = BSP_SCREEN_HEIGHT;
static int const LCD_X_SCALE   = 2;
static int const LCD_Y_SCALE   = 2;
static uint8_t const LCD_ON_R  = 0xFFU;
static uint8_t const LCD_ON_G  = 0xFFU;
static uint8_t const LCD_ON_B  = 0xFFU;
static uint8_t const LCD_OFF_R = 0x00U;
static uint8_t const LCD_OFF_G = 0x00U;
static uint8_t const LCD_OFF_B = 0x00U;
static int const BMP_HDR = 54U;   // size of the header in the BMP file format

/* BMP header + pixel array */
static uchar l_bmp24[BMP_HDR
                + 3*(LCD_WIDTH*LCD_X_SCALE * LCD_HEIGHT*LCD_Y_SCALE)];
static QPixmap *l_pixmap; /* Qt Pixmap object */

//............................................................................
static uint8_t l_ship_pos = GAME_SHIP_Y;

//............................................................................
static inline void LCD_setAt(int x, int y,
                             uint8_t R, uint8_t G, uint8_t B)
{
    uchar *pixelRGB =
        &l_bmp24[BMP_HDR
                 + 3*LCD_X_SCALE*x
                 + 3*LCD_X_SCALE*LCD_WIDTH*LCD_X_SCALE*(LCD_HEIGHT - 1 - y)];
    for (int sy = 0; sy < LCD_Y_SCALE;
         ++sy, pixelRGB += LCD_WIDTH*LCD_X_SCALE*3) {
        for (int sx = 0; sx < 3*LCD_X_SCALE; sx += 3) {
            pixelRGB[0 + sx] = R;
            pixelRGB[1 + sx] = G;
            pixelRGB[2 + sx] = B;
        }
    }
}

//............................................................................
void QPnApp::onClockTick(void) {                                       // slot
    QF_tickISR();
    QActive_post((QActive *)&AO_Tunnel,  TIME_TICK_SIG, 0U);
    QActive_post((QActive *)&AO_Ship,    TIME_TICK_SIG, 0U);
    QActive_post((QActive *)&AO_Missile, TIME_TICK_SIG, 0U);
}
//............................................................................
void QF_onStartup(void) {
    QF_setClockTick(BSP_TICKS_PER_SEC);
}
/*..........................................................................*/
void BSP_init() {
     l_bmp24[ 0] = 'B';
     l_bmp24[ 1] = 'M';
     l_bmp24[ 2] = sizeof(l_bmp24) & 0xFF;
     l_bmp24[ 3] = (sizeof(l_bmp24) >>  8) & 0xFF;
     l_bmp24[ 4] = (sizeof(l_bmp24) >> 16) & 0xFF;
     l_bmp24[ 5] = (sizeof(l_bmp24) >> 24) & 0xFF;
     l_bmp24[10] = BMP_HDR;

     l_bmp24[14] = 40;  // size of the secondary header
     l_bmp24[18] = (LCD_WIDTH * LCD_X_SCALE) & 0xFF;
     l_bmp24[19] = ((LCD_WIDTH * LCD_X_SCALE) >>  8) & 0xFF;
     l_bmp24[20] = ((LCD_WIDTH * LCD_X_SCALE) >> 16) & 0xFF;
     l_bmp24[21] = ((LCD_WIDTH * LCD_X_SCALE) >> 24) & 0xFF;
     l_bmp24[22] = (LCD_HEIGHT * LCD_X_SCALE) & 0xFF;
     l_bmp24[23] = ((LCD_HEIGHT * LCD_Y_SCALE) >>  8) & 0xFF;
     l_bmp24[24] = ((LCD_HEIGHT * LCD_Y_SCALE) >> 16) & 0xFF;
     l_bmp24[25] = ((LCD_HEIGHT * LCD_Y_SCALE) >> 24) & 0xFF;
     l_bmp24[26] = 1;           // number of planes
     l_bmp24[28] = 3*8;  // color bits per pixel
     l_bmp24[38] = 0x12; // hor: # of pixels per meter LSB
     l_bmp24[39] = 0x0B; // hor: # of pixels per meter MSB
     l_bmp24[42] = 0x12; // ver: # of pixels per meter LSB
     l_bmp24[43] = 0x0B; // ver: # of pixels per meter MSB

     l_pixmap = new QPixmap();
}
/*..........................................................................*/
void BSP_drawBitmap(uint8_t const *bitmap) {
    for (int y = 0; y < LCD_HEIGHT; ++y) {
        for (int x = 0; x < LCD_WIDTH; ++x) {
            uint8_t bits = bitmap[x + (y/8)*LCD_WIDTH];
            if ((bits & (1U << (y & 0x07U))) != 0U) {
                LCD_setAt(x, y,
                          LCD_ON_R, LCD_ON_G, LCD_ON_B);
            }
            else {
                LCD_setAt(x, y,
                          LCD_OFF_R, LCD_OFF_G, LCD_OFF_B);
            }
        }
    }

    (void)l_pixmap->loadFromData(l_bmp24, sizeof(l_bmp24), "BMP");
    Gui::instance->m_display->setPixmap(*l_pixmap);
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

    while (*str != '\0') {
        uint8_t const *ch = &font5x7[*str - ' '][0];
        for (int dx = 0; dx < 5; ++dx) {
            for (int dy = 0; dy < 8; ++dy) {
                if ((ch[dx] & (1U << dy)) != 0U) {
                    LCD_setAt(x + dx, y*8 + dy,
                              LCD_ON_R, LCD_ON_G, LCD_ON_B);
                }
                else {
                    LCD_setAt(x + dx, y*8 + dy,
                              LCD_OFF_R, LCD_OFF_G, LCD_OFF_B);
                }
            }
        }
        ++str;
        x += 6;
    }
    (void)l_pixmap->loadFromData(l_bmp24, sizeof(l_bmp24), "BMP");
    Gui::instance->m_display->setPixmap(*l_pixmap);
}
/*..........................................................................*/
void BSP_updateScore(uint16_t score) {
    Gui::instance->m_score->setText(QString("%1").arg(score, -4));
}
/*..........................................................................*/
void BSP_displayOn(void) {
}
/*..........................................................................*/
void BSP_displayOff(void) {
    memset(&l_bmp24[BMP_HDR], 0x00,
           3*(LCD_WIDTH*LCD_X_SCALE * LCD_HEIGHT*LCD_Y_SCALE));
    (void)l_pixmap->loadFromData(l_bmp24, sizeof(l_bmp24), "BMP");
    Gui::instance->m_display->setPixmap(*l_pixmap);
}
/*..........................................................................*/
void BSP_moveShipUp(void) {
    if (l_ship_pos > 0U) {
        --l_ship_pos;
    }
    QActive_post((QActive *)&AO_Ship, PLAYER_SHIP_MOVE_SIG,
                 ((l_ship_pos << 8) | GAME_SHIP_X));
}
//............................................................................
void BSP_moveShipDown(void) {
    if (l_ship_pos < (GAME_SCREEN_HEIGHT - 3)) {
        ++l_ship_pos;
    }
    QActive_post((QActive *)&AO_Ship, PLAYER_SHIP_MOVE_SIG,
                 ((l_ship_pos << 8) | GAME_SHIP_X));
}
//............................................................................
void Q_onAssert(char_t const * const file, int line) {
    QF_stop();                                   // stop the system clock tick
    QMessageBox::critical(0, "PROBLEM",
        QString("<p>Assertion failed in module <b>%1</b>,"
                "line <b>%2</b></p>")
            .arg(file)
            .arg(line));
    qFatal("Assertion failed in module %s, line %d", file, line);
}

}  // extern "C"




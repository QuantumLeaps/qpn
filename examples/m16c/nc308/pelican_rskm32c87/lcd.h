#ifndef LCD_H
#define LCD_H
/***********************************************************************************

FILE NAME          lcd.h

DESCRIPTION        Driver for KS0066u LCD Module Controller (8 characters by 2 lines )
                on the Renesas RSK boards - header file

Copyright   : 2005 Renesas Technology Europe Ltd.
Copyright   : 2005 Renesas Technology Corporation.
All Rights Reserved
***********************************************************************************/

/***********************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
21.11.2005 RTE-DDE First Release
***********************************************************************************/
void InitialiseDisplay( void );
void DisplayString(unsigned char position, _far char * string);
void LCD_write(unsigned char data_or_ctrl, unsigned char value);
void LCD_nibble_write(unsigned char data_or_ctrl, unsigned char value);
void DisplayDelay(unsigned long int units);
/* RS Register Select pin */
#define RS_PIN                 p2_0
/* Display Enable pin */
#define EN_PIN                p2_1

/* Data bus port */
#define DATA_PORT            p2
/* Bit mask from entire port */
#define DATA_PORT_MASK        0xF0
/* Number of bits data needs to shift */
#define DATA_PORT_SHIFT        4

#define DATA_WR 1
#define CTRL_WR 0

/* Set to ensure base delay of 1mS minimum */
#define DELAY_TIMING        0x05
/* number of lines on the LCD display */
#define NUMB_CHARS_PER_LINE    8
/* Maximum charactors per line of LCD display. */
#define MAXIMUM_LINES        2

#define LCD_LINE1 0
#define LCD_LINE2 16

/**********************************************************************************/
/* LCD commands - use LCD_write function to write these commands to the LCD.      */
/**********************************************************************************/
/* Clear LCD display and home cursor */
#define LCD_CLEAR        0x01
/* move cursor to line 1 */
#define LCD_HOME_L1      0x80
/* move cursor to line 2 */
#define LCD_HOME_L2      0xC0
/* Cursor auto decrement after R/W */
#define CURSOR_MODE_DEC  0x04
/* Cursor auto increment after R/W */
#define CURSOR_MODE_INC  0x06
/* Setup, 4 bits,2 lines, 5X7 */
#define FUNCTION_SET     0x28
/* Display ON with Cursor */
#define LCD_CURSOR_ON    0x0E
/* Display ON with Cursor off */
#define LCD_CURSOR_OFF   0x0C
/* Display on with blinking cursor */
#define LCD_CURSOR_BLINK 0x0D
/*Move Cursor Left One Position */
#define LCD_CURSOR_LEFT  0x10
/* Move Cursor Right One Position */
#define LCD_CURSOR_RIGHT 0x14

#define LCD_DISPLAY_ON   0x04
#define LCD_TWO_LINE     0x08

#endif
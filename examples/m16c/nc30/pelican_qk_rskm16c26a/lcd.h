#ifndef LCD_H
#define LCD_H

/***********************************************************************************
FILE NAME      : lcd.h

DESCRIPTION    : Driver for KS0066u LCD Module Controller (8 characters by 2 lines )
              on the Renesas RSK boards - header file

Copyright   : 2006 Renesas Technology Europe Ltd.
Copyright   : 2006 Renesas Technology Corporation.
              All Rights Reserved
***********************************************************************************/

/***********************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
27.03.2006 RTE-NPS First Release
***********************************************************************************/

void InitialiseDisplay( void );
void DisplayString(unsigned char position, _far char * string);
void LCD_write(unsigned char data_or_ctrl, unsigned char value);
void LCD_nibble_write(unsigned char data_or_ctrl, unsigned char value);
void DisplayDelay(unsigned long int units);
#define RS_PIN                 p9_0    /* RS Register Select pin */
#define EN_PIN                p9_1    /* Display Enable pin */

#define DATA_PORT            p6         /* Data bus port */
#define DATA_PORT_MASK        0x0F    /* Bit mask from entire port */
#define DATA_PORT_SHIFT        0        /* Number of bits data needs to shift */

#define DATA_WR 1
#define CTRL_WR 0

#define DELAY_TIMING        0x01    /* Set to ensure base delay of 1mS minimum */
#define MAXIMUM_LINES        2        /* number of lines on the LCD display */
#define NUMB_CHARS_PER_LINE    8        /* Maximum charactors per line of LCD display. */

#define LCD_LINE1 0
#define LCD_LINE2 16

/**********************************************************************************
 LCD commands - use LCD_write function to write these commands to the LCD.
**********************************************************************************/
#define LCD_CLEAR        0x01    /* Clear LCD display and home cursor   */
#define LCD_HOME_L1      0x80    /* move cursor to line 1               */
#define LCD_HOME_L2      0xC0    /* move cursor to line 2               */
#define CURSOR_MODE_DEC  0x04    /* Cursor auto decrement after R/W     */
#define CURSOR_MODE_INC  0x06    /* Cursor auto increment after R/W     */
#define FUNCTION_SET     0x28    /* Setup, 4 bits,2 lines, 5X7          */
#define LCD_CURSOR_ON    0x0E    /* Display ON with Cursor              */
#define LCD_CURSOR_OFF   0x0C    /* Display ON with Cursor off            */
#define LCD_CURSOR_BLINK 0x0D    /* Display on with blinking cursor     */
#define LCD_CURSOR_LEFT  0x10    /* Move Cursor Left One Position       */
#define LCD_CURSOR_RIGHT 0x14    /* Move Cursor Right One Position      */

#define LCD_DISPLAY_ON   0x04
#define LCD_TWO_LINE     0x08

#endif
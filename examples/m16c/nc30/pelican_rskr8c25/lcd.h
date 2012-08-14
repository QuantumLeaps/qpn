/***********************************************************************************
FILE NAME      : LCD.h
DESCRIPTION : LCD Module header.

Copyright   : 2006 Renesas Technology Europe Ltd.
Copyright   : 2006 Renesas Solutions Corporation.
Copyright   : 2006 Renesas Technology Corporation.
              All Rights Reserved

***********************************************************************************/

/***********************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
02.05.2006 RTE-NPS First Release
***********************************************************************************/

#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

/***********************************************************************************
Macro definitions
***********************************************************************************/
/* General Values */
/* RS Register Select pin */
#define RS_PIN                 p1_1
/* Display Enable pin */
#define EN_PIN                 p1_2

/* Data bus port */
#define DATA_PORT              p0

/* Bit mask from entire port */
#define DATA_PORT_MASK         0x0F

/* Number of bits data needs to shift */
#define DATA_PORT_SHIFT        0

#define DATA_WR 1
#define CTRL_WR 0

/* Set to ensure base delay of 1mS minimum */
#define DELAY_TIMING           2

/* number of lines on the LCD display */
#define MAXIMUM_LINES          2
/* Maximum charactors per line on LCD display. */
#define NUMB_CHARS_PER_LINE    8

/* Points to line1 of the LCD */
#define LCD_LINE1 0
/* Points to line2 of the LCD */
#define LCD_LINE2 16

/**********************************************************************************
   LCD commands - use LCD_write function to write these commands to the LCD.
**********************************************************************************/
/* Clear LCD display and cursor home  */
#define LCD_CLEAR        0x01
/* Place cursor at line 1             */
#define LCD_HOME_L1      0x80
/* Place cursor at line 2             */
#define LCD_HOME_L2      0xC0
/* Cursor auto decrement after R/W    */
#define CURSOR_MODE_DEC  0x04
/* Cursor auto increment after R/W    */
#define CURSOR_MODE_INC  0x06
/* Setup, 4 bits, 2 lines, 5x7        */
#define FUNCTION_SET     0x28
/* Display ON with Cursor               */
#define LCD_CURSOR_ON    0x0E
/* Display ON with Cursor off         */
#define LCD_CURSOR_OFF   0x0C
/* Display ON with cursor blinking    */
#define LCD_CURSOR_BLINK 0x0D
/* Move Cursor Left One Position      */
#define LCD_CURSOR_LEFT  0x10
/* Move Cursor Right One Position     */
#define LCD_CURSOR_RIGHT 0x14

/* Display LCD module                 */
#define LCD_DISPLAY_ON   0x04
/* Can use 2-lines of LCD module      */
#define LCD_TWO_LINE     0x08

/***********************************************************************************
Function Prototypes
***********************************************************************************/
void InitialiseDisplay( void );
void DisplayString(unsigned char position, _far char * string);
void LCD_write(unsigned char data_or_ctrl, unsigned char value);
void LCD_nibble_write(unsigned char data_or_ctrl, unsigned char value);
void DisplayDelay(unsigned long int units);
void Convert_16BitNumber_ToString(unsigned int c, char* buffer);

#endif        /*    LCD_H_INCLUDED    */
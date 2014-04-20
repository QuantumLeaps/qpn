/******************************************************************************/
/* lnk_msp430g2553.cmd - LINKER COMMAND FILE FOR LINKING MSP430G2553 PROGRAMS     */
/*                                                                            */
/*   Usage:  lnk430 <obj files...>    -o <out file> -m <map file> lnk.cmd     */
/*           cl430  <src files...> -z -o <out file> -m <map file> lnk.cmd     */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* These linker options are for command line linking only.  For IDE linking,  */
/* you should set your linker options in Project Properties                   */
/* -c                                               LINK USING C CONVENTIONS  */
/* -stack  0x0100                                   SOFTWARE STACK SIZE       */
/* -heap   0x0100                                   HEAP AREA SIZE            */
/*                                                                            */
/*----------------------------------------------------------------------------*/


/****************************************************************************/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/****************************************************************************/

MEMORY
{
    SFR                     : origin = 0x0000, length = 0x0010
    PERIPHERALS_8BIT        : origin = 0x0010, length = 0x00F0
    PERIPHERALS_16BIT       : origin = 0x0100, length = 0x0100
    RAM                     : origin = 0x0200, length = 0x0200
    INFOA                   : origin = 0x10C0, length = 0x0040
    INFOB                   : origin = 0x1080, length = 0x0040
    INFOC                   : origin = 0x1040, length = 0x0040
    INFOD                   : origin = 0x1000, length = 0x0040
    FLASH                   : origin = 0xC000, length = 0x3FE0
    INT00                   : origin = 0xFFE0, length = 0x0002
    INT01                   : origin = 0xFFE2, length = 0x0002
    INT02                   : origin = 0xFFE4, length = 0x0002
    INT03                   : origin = 0xFFE6, length = 0x0002
    INT04                   : origin = 0xFFE8, length = 0x0002
    INT05                   : origin = 0xFFEA, length = 0x0002
    INT06                   : origin = 0xFFEC, length = 0x0002
    INT07                   : origin = 0xFFEE, length = 0x0002
    INT08                   : origin = 0xFFF0, length = 0x0002
    INT09                   : origin = 0xFFF2, length = 0x0002
    INT10                   : origin = 0xFFF4, length = 0x0002
    INT11                   : origin = 0xFFF6, length = 0x0002
    INT12                   : origin = 0xFFF8, length = 0x0002
    INT13                   : origin = 0xFFFA, length = 0x0002
    INT14                   : origin = 0xFFFC, length = 0x0002
    RESET                   : origin = 0xFFFE, length = 0x0002
}

/****************************************************************************/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/****************************************************************************/

SECTIONS
{
    .bss        : {} > RAM                /* GLOBAL & STATIC VARS              */
    .data       : {} > RAM                /* GLOBAL & STATIC VARS              */
    .sysmem     : {} > RAM                /* DYNAMIC MEMORY ALLOCATION AREA    */
    .stack      : {} > RAM (HIGH)         /* SOFTWARE SYSTEM STACK             */

    .text       : {} > FLASH              /* CODE                              */
    .cinit      : {} > FLASH              /* INITIALIZATION TABLES             */
    .const      : {} > FLASH              /* CONSTANT DATA                     */
    .cio        : {} > RAM                /* C I/O BUFFER                      */

    .pinit      : {} > FLASH              /* C++ CONSTRUCTOR TABLES            */
    .init_array : {} > FLASH              /* C++ CONSTRUCTOR TABLES            */
    .mspabi.exidx : {} > FLASH            /* C++ CONSTRUCTOR TABLES            */
    .mspabi.extab : {} > FLASH            /* C++ CONSTRUCTOR TABLES            */

    .infoA     : {} > INFOA              /* MSP430 INFO FLASH MEMORY SEGMENTS */
    .infoB     : {} > INFOB
    .infoC     : {} > INFOC
    .infoD     : {} > INFOD

    /* MSP430 INTERRUPT VECTORS          */
    .int00       : {}               > INT00
    .int01       : {}               > INT01
    PORT1        : { * ( .int02 ) } > INT02 type = VECT_INIT
    PORT2        : { * ( .int03 ) } > INT03 type = VECT_INIT
    .int04       : {}               > INT04
    ADC10        : { * ( .int05 ) } > INT05 type = VECT_INIT
    USCIAB0TX    : { * ( .int06 ) } > INT06 type = VECT_INIT
    USCIAB0RX    : { * ( .int07 ) } > INT07 type = VECT_INIT
    TIMER0_A1    : { * ( .int08 ) } > INT08 type = VECT_INIT
    TIMER0_A0    : { * ( .int09 ) } > INT09 type = VECT_INIT
    WDT          : { * ( .int10 ) } > INT10 type = VECT_INIT
    COMPARATORA   : { * ( .int11 ) } > INT11 type = VECT_INIT
    TIMER1_A1    : { * ( .int12 ) } > INT12 type = VECT_INIT
    TIMER1_A0    : { * ( .int13 ) } > INT13 type = VECT_INIT
    NMI          : { * ( .int14 ) } > INT14 type = VECT_INIT
    .reset       : {}               > RESET  /* MSP430 RESET VECTOR         */ 
}

/****************************************************************************/
/* INCLUDE PERIPHERALS MEMORY MAP                                           */
/****************************************************************************/

-l msp430g2553.cmd


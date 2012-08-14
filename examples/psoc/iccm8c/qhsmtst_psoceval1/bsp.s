    cpu LMM
    .module bsp.c
    .area text(rom, con, rel)
    .dbfile ./bsp.c
    .dbfunc e BSP_init _BSP_init fV
_BSP_init::
    .dbline -1
    .dbline 43
; /*****************************************************************************
; * Product: BSP for the PSoCEVAL1 board
; * Last Updated for Version: 4.0.01
; * Date of the Last Update:  Jun 13, 2008
; *
; *                    Q u a n t u m     L e a P s
; *                    ---------------------------
; *                    innovating embedded systems
; *
; * Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
; *
; * This software may be distributed and modified under the terms of the GNU
; * General Public License version 2 (GPL) as published by the Free Software
; * Foundation and appearing in the file GPL.TXT included in the packaging of
; * this file. Please note that GPL Section 2[b] requires that all works based
; * on this software must also be made publicly available under the terms of
; * the GPL ("Copyleft").
; *
; * Alternatively, this software may be distributed and modified under the
; * terms of Quantum Leaps commercial licenses, which expressly supersede
; * the GPL and are specifically designed for licensees interested in
; * retaining the proprietary status of their code.
; *
; * Contact information:
; * Quantum Leaps Web site:  http://www.quantum-leaps.com
; * e-mail:                  info@quantum-leaps.com
; *
; *-----------------------------------------------------------------------------
; * PSoCEVAL1 board custom wiring:
; *     P00 -> LED1
; *     P01 -> LED2
; *     P02 -> LED3
; *     P03 -> LED4
; *     P10 -> SW
; *     P16 -> RX = Serial RX
; *     P27 -> TX = Serial TX
; *****************************************************************************/
; #include "qpn_port.h"
; #include "bsp.h"
; #include "qhsmtst.h"
;
; /*..........................................................................*/
; void BSP_init(void) {
    .dbline 44
;     LED_1_Start();
    push X
    xcall _LED_1_Start
    .dbline 45
;     LED_2_Start();
    xcall _LED_2_Start
    .dbline 46
;     LED_3_Start();
    xcall _LED_3_Start
    .dbline 47
;     LED_4_Start();
    xcall _LED_4_Start
    .dbline 49
;
;     UART_1_Start(UART_PARITY_NONE);                          /* enable UART */
    mov A,0
    xcall _UART_1_Start
    .dbline 50
;     UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    .dbline 51
;     UART_1_CPutString("QHsmTst example, QP-nano ");       /* opening string */
    mov A,>L2
    push A
    mov A,<L2
    mov X,A
    pop A
    xcall _UART_1_CPutString
    pop X
    .dbline 52
;     UART_1_CPutString(QP_getVersion());                  /* QP-nano version */
    xcall _QP_getVersion
    push X
    mov REG[0xd0],>__r0
    mov A,[__r0]
    push A
    mov A,[__r1]
    mov X,A
    pop A
    xcall _UART_1_CPutString
    .dbline 53
;     UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    pop X
    .dbline -2
    .dbline 54
; }
L1:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e BSP_exit _BSP_exit fV
;            dly -> X+0
_BSP_exit::
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 56
; /*..........................................................................*/
; void BSP_exit(void) {
    .dbline 57
L4:
    .dbline 57
;     for (;;) {       /* NOTE: replace the loop with reset for final version */
    .dbline 59
;         uint16_t volatile dly;
;         LED_4_On();
    push X
    xcall _LED_4_On
    pop X
    .dbline 60
    mov [X+1],16
    mov [X+0],39
    xjmp L11
L8:
    .dbline 60
    .dbline 61
L9:
    .dbline 60
    dec [X+1]
    sbb [X+0],0
L11:
    .dbline 60
;         for (dly = 10000; dly != 0; --dly) {
    cmp [X+0],0
    jnz L8
    cmp [X+1],0
    jnz L8
X1:
    .dbline 62
;         }
;         LED_4_Off();
    push X
    xcall _LED_4_Off
    pop X
    .dbline 63
    mov [X+1],16
    mov [X+0],39
    xjmp L15
L12:
    .dbline 63
    .dbline 64
L13:
    .dbline 63
    dec [X+1]
    sbb [X+0],0
L15:
    .dbline 63
    cmp [X+0],0
    jnz L12
    cmp [X+1],0
    jnz L12
X2:
    .dbline 65
    .dbline 57
    .dbline 57
    xjmp L4
X0:
    .dbline -2
L3:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l dly 0 X
    .dbend
    .dbfunc e BSP_display _BSP_display fV
;            msg -> X-5
_BSP_display::
    .dbline -1
    push X
    mov X,SP
    .dbline 68
;         for (dly = 10000; dly != 0; --dly) {
;         }
;     }
; }
; /*..........................................................................*/
; void BSP_display(char const *msg) {
    .dbline 69
;     UART_1_CPutString(msg);                           /* output the message */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    mov X,A
    pop A
    xcall _UART_1_CPutString
    pop X
    .dbline -2
    .dbline 70
; }
L16:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l msg -5 pc
    .dbend
    .dbfunc e Q_onAssert _Q_onAssert fV
;           line -> X-7
;           file -> X-5
_Q_onAssert::
    .dbline -1
    push X
    mov X,SP
    .dbline 72
; /*..........................................................................*/
; void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    .dbline 73
;     (void)file;                                   /* avoid compiler warning */
    .dbline 74
;     (void)line;                                   /* avoid compiler warning */
    .dbline 75
;     BSP_exit();
    xcall _BSP_exit
    .dbline -2
    .dbline 76
; }
L17:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l line -7 I
    .dbsym l file -5 ppc
    .dbend
    .area lit(rom, con, rel)
L2:
    .byte 'Q,'H,'s,'m,'T,'s,'t,32,'e,'x,'a,'m,'p,'l,'e,44
    .byte 32,'Q,'P,45,'n,'a,'n,'o,32,0

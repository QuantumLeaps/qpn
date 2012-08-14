    cpu LMM
    .module main.c
    .area text(rom, con, rel)
    .dbfile ./main.c
    .dbfunc e main _main fV
;              c -> X+0
_main::
    .dbline -1
    push X
    mov X,SP
    add SP,1
    .dbline 33
; /*****************************************************************************
; * QHsmTst example
; * Last Updated for Version: 4.0.01
; * Date of the Last Update:  Jun 12, 2008
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
; *****************************************************************************/
; #include "qpn_port.h"
; #include "bsp.h"
; #include "qhsmtst.h"
;
; /*..........................................................................*/
; void main (void) {
    .dbline 34
;     QHsmTst_ctor();                                  /* instantiate the HSM */
    xcall _QHsmTst_ctor
    .dbline 36
;
;     BSP_init();                                     /* initialize the board */
    xcall _BSP_init
    .dbline 38
;
;     QHsm_init((QHsm *)&HSM_QHsmTst);        /* take the initial transitioin */
    mov A,>_HSM_QHsmTst
    push A
    mov A,<_HSM_QHsmTst
    push A
    xcall _QHsm_init
    add SP,-2
    .dbline 40
L2:
    .dbline 40
;
;     for (;;) {
    .dbline 43
;         char c;
;
;         LED_1_On();
    push X
    xcall _LED_1_On
    .dbline 44
;         UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    .dbline 45
;         UART_1_PutChar('>');
    mov A,62
    xcall _UART_1_PutChar
    .dbline 46
;         c = UART_1_cGetChar();                       /* wait for user input */
    xcall _UART_1_cGetChar
    mov REG[0xd0],>__r0
    pop X
    mov [X+0],A
    .dbline 47
;         UART_1_PutChar(c);
    push X
    mov A,[X+0]
    xcall _UART_1_PutChar
    .dbline 48
;         UART_1_CPutString(": ");
    mov A,>L6
    push A
    mov A,<L6
    mov X,A
    pop A
    xcall _UART_1_CPutString
    .dbline 49
;         LED_1_Off();
    xcall _LED_1_Off
    .dbline 51
;
;         LED_2_On();
    xcall _LED_2_On
    pop X
    .dbline 52
;         if ('a' <= c && c <= 'i') {                            /* in range? */
    cmp [X+0],97
    jc L7
X1:
    mov A,105
    cmp A,[X+0]
    jc L7
X2:
    .dbline 52
    .dbline 53
;             Q_SIG((QHsm *)&HSM_QHsmTst) = (QSignal)(c - 'a' + A_SIG);
    mov A,[X+0]
    sub A,97
    add A,5
    mov REG[0xd0],>_HSM_QHsmTst
    mov [_HSM_QHsmTst+2],A
    .dbline 54
;         }
    xjmp L8
L7:
    .dbline 55
;         else if ('A' <= c && c <= 'I') {                       /* in range? */
    cmp [X+0],65
    jc L10
X3:
    mov A,73
    cmp A,[X+0]
    jc L10
X4:
    .dbline 55
    .dbline 56
;             Q_SIG((QHsm *)&HSM_QHsmTst) = (QSignal)(c - 'A' + A_SIG);
    mov A,[X+0]
    sub A,65
    add A,5
    mov REG[0xd0],>_HSM_QHsmTst
    mov [_HSM_QHsmTst+2],A
    .dbline 57
;         }
    xjmp L11
L10:
    .dbline 58
;         else {
    .dbline 59
;             Q_SIG((QHsm *)&HSM_QHsmTst) = IGNORE_SIG;
    mov REG[0xd0],>_HSM_QHsmTst
    mov [_HSM_QHsmTst+2],15
    .dbline 60
;         }
L11:
L8:
    .dbline 61
    mov A,>_HSM_QHsmTst
    push A
    mov A,<_HSM_QHsmTst
    push A
    xcall _QHsm_dispatch
    add SP,-2
    .dbline 62
    push X
    xcall _LED_2_Off
    pop X
    .dbline 63
    .dbline 40
    .dbline 40
    xjmp L2
X0:
    .dbline -2
L1:
    add SP,-1
    pop X
    .dbline 0 ; func end
    jmp .
    .dbsym l c 0 c
    .dbend
    .area lit(rom, con, rel)
L6:
    .byte 58,32,0

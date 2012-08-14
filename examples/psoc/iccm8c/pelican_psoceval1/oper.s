    cpu LMM
    .module oper.c
    .area text(rom, con, rel)
    .dbfile ./oper.c
    .dbfunc e Oper_ctor _Oper_ctor fV
_Oper_ctor::
    .dbline -1
    .dbline 53
; /*****************************************************************************
; * Product: PELICAN crossing example
; * Last Updated for Version: 4.0.01
; * Date of the Last Update:  May 03, 2008
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
; #include "pelican.h"
;
; /*--------------------------------------------------------------------------*/
; typedef struct OperTag {
;     QActive super;                                  /* derived from QActive */
; } Oper;
;
; QState Oper_initial       (Oper *me);
; QState Oper_in_service    (Oper *me);
; QState Oper_out_of_service(Oper *me);
;
; static Oper l_oper;  /* the encapsulated instance of Operator active object */
;
; /* global objects ----------------------------------------------------------*/
; Oper AO_Oper;              /* the single instance of the Oper active object */
;
; /*--------------------------------------------------------------------------*/
; enum OperTimeouts {                            /* various timeouts in ticks */
;     IN_SERVICE_TOUT      = BSP_TICKS_PER_SEC * 30,       /* in-service time */
;     OUT_OF_SERVICE_TOUT  = BSP_TICKS_PER_SEC * 8,    /* out-of-service time */
; };
;
; /*..........................................................................*/
; void Oper_ctor(void) {
    .dbline 54
;     QActive_ctor((QActive *)&AO_Oper, (QStateHandler)&Oper_initial);
    mov REG[0xd0],>_AO_Oper
    mov [_AO_Oper+1],<PL_Oper_initial
    mov [_AO_Oper],>PL_Oper_initial
    .dbline -2
    .dbline 55
; }
L1:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e Oper_initial _Oper_initial fc
;             me -> X-5
_Oper_initial::
    .dbline -1
    push X
    mov X,SP
    .dbline 58
;
; /* HSM definition ----------------------------------------------------------*/
; QState Oper_initial(Oper *me) {
    .dbline 59
;     return Q_TRAN(&Oper_in_service);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Oper_in_service
    mvi [__r1],A
    mov A,<PL_Oper_in_service
    mvi [__r1],A
    mov A,2
    .dbline -2
L2:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc e Oper_in_service _Oper_in_service fc
;             me -> X-5
_Oper_in_service::
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 62
; }
; /*..........................................................................*/
; QState Oper_in_service(Oper *me) {
    .dbline 63
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+1],A
    mov [X+0],0
    cmp [X+0],0
    jnz X1
    cmp [X+1],1
    jz L7
X1:
    cmp [X+0],0
    jnz X2
    cmp [X+1],4
    jz L8
X2:
    xjmp L4
X0:
    .dbline 63
;     switch (Q_SIG(me)) {
L7:
    .dbline 64
;         case Q_ENTRY_SIG: {
    .dbline 65
;             QActive_arm((QActive *)me, IN_SERVICE_TOUT);
    mov A,7
    push A
    mov A,-128
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    .dbline 66
;             QActive_post((QActive *)&AO_Pelican, ON_SIG, 0);
    mov A,0
    push A
    mov A,7
    push A
    mov A,>_AO_Pelican
    push A
    mov A,<_AO_Pelican
    push A
    xcall _QActive_post
    add SP,-8
    .dbline 67
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L3
L8:
    .dbline 69
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 70
;             return Q_TRAN(&Oper_out_of_service);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Oper_out_of_service
    mvi [__r1],A
    mov A,<PL_Oper_out_of_service
    mvi [__r1],A
    mov A,2
    xjmp L3
L4:
    .dbline 73
;         }
;     }
;     return Q_SUPER(&QHsm_top);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsm_top
    mvi [__r1],A
    mov A,<PL_QHsm_top
    mvi [__r1],A
    mov A,3
    .dbline -2
L3:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc e Oper_out_of_service _Oper_out_of_service fc
;             me -> X-5
_Oper_out_of_service::
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 76
; }
; /*..........................................................................*/
; QState Oper_out_of_service(Oper *me) {
    .dbline 77
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+1],A
    mov [X+0],0
    cmp [X+0],0
    jnz X4
    cmp [X+1],1
    jz L13
X4:
    cmp [X+0],0
    jnz X5
    cmp [X+1],4
    jz L14
X5:
    xjmp L10
X3:
    .dbline 77
;     switch (Q_SIG(me)) {
L13:
    .dbline 78
;         case Q_ENTRY_SIG: {
    .dbline 79
;             QActive_arm((QActive *)me, OUT_OF_SERVICE_TOUT);
    mov A,2
    push A
    mov A,0
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    .dbline 80
;             QActive_post((QActive *)&AO_Pelican, OFF_SIG, 0);
    mov A,0
    push A
    mov A,6
    push A
    mov A,>_AO_Pelican
    push A
    mov A,<_AO_Pelican
    push A
    xcall _QActive_post
    add SP,-8
    .dbline 81
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L9
L14:
    .dbline 83
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 84
;             return Q_TRAN(&Oper_in_service);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Oper_in_service
    mvi [__r1],A
    mov A,<PL_Oper_in_service
    mvi [__r1],A
    mov A,2
    xjmp L9
L10:
    .dbline 87
;         }
;     }
;     return Q_SUPER(&QHsm_top);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsm_top
    mvi [__r1],A
    mov A,<PL_QHsm_top
    mvi [__r1],A
    mov A,3
    .dbline -2
L9:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .area data(ram, con, rel)
    .dbfile ./oper.c
_l_oper:
    .byte 0,0,0,0,0,0,0,0,0,0
    .dbsym s l_oper _l_oper X
    .area data(ram, con, rel)
    .dbfile ./oper.c
_AO_Oper::
    .byte 0,0,0,0,0,0,0,0,0,0
    .dbsym e AO_Oper _AO_Oper X
    .area func_lit
PL_QHsm_top:    .word _QHsm_top
PL_Oper_out_of_service:    .word _Oper_out_of_service
PL_Oper_in_service:    .word _Oper_in_service
PL_Oper_initial:    .word _Oper_initial

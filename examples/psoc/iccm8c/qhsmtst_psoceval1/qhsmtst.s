    cpu LMM
    .module qhsmtst.c
    .area text(rom, con, rel)
    .dbfile ./qhsmtst.c
    .dbfunc e QHsmTst_ctor _QHsmTst_ctor fV
_QHsmTst_ctor::
    .dbline -1
    .dbline 51
; /*****************************************************************************
; * QHsmTst example
; * Last Updated for Version: 4.0.00
; * Date of the Last Update:  Apr 07, 2008
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
; #include "qhsmtst.h"
;
; /* QHsmTst class -----------------------------------------------------------*/
; typedef struct QHsmTstTag {
;     QHsm super;                                        /* extend class QHsm */
;                                              /* extended state variables... */
;     uint8_t foo;
; } QHsmTst;
;
; void   QHsmTst_ctor(void);                               /* the ctor */
; static QState QHsmTst_initial(QHsmTst *me);  /* initial pseudostate-handler */
; static QState QHsmTst_s    (QHsmTst *me);                  /* state-handler */
; static QState QHsmTst_s1   (QHsmTst *me);                  /* state-handler */
; static QState QHsmTst_s11  (QHsmTst *me);                  /* state-handler */
; static QState QHsmTst_s2   (QHsmTst *me);                  /* state-handler */
; static QState QHsmTst_s21  (QHsmTst *me);                  /* state-handler */
; static QState QHsmTst_s211 (QHsmTst *me);                  /* state-handler */
;
; /* global objects ----------------------------------------------------------*/
; QHsmTst HSM_QHsmTst;                /* the sole instance of the QHsmTst HSM */
;
; /*..........................................................................*/
; void QHsmTst_ctor(void) {
    .dbline 52
;     QHsm_ctor(&HSM_QHsmTst.super, (QStateHandler)&QHsmTst_initial);
    mov REG[0xd0],>_HSM_QHsmTst
    mov [_HSM_QHsmTst+1],<PL_QHsmTst_initial
    mov [_HSM_QHsmTst],>PL_QHsmTst_initial
    .dbline 53
;     HSM_QHsmTst.foo = 0;              /* initialize extended state variable */
    mov [_HSM_QHsmTst+4],0
    .dbline -2
    .dbline 54
; }
L1:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc s QHsmTst_initial _QHsmTst_initial fc
;             me -> X-5
_QHsmTst_initial:
    .dbline -1
    push X
    mov X,SP
    .dbline 57
;
; /*..........................................................................*/
; static QState QHsmTst_initial(QHsmTst *me) {
    .dbline 58
;     BSP_display("top-INIT;");
    mov A,>L4
    push A
    mov A,<L4
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 59
;     return Q_TRAN(&QHsmTst_s2);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s2
    mvi [__r1],A
    mov A,<PL_QHsmTst_s2
    mvi [__r1],A
    mov A,2
    .dbline -2
L3:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s QHsmTst_s _QHsmTst_s fc
;             me -> X-5
_QHsmTst_s:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 62
; }
; /*..........................................................................*/
; static QState QHsmTst_s(QHsmTst *me) {
    .dbline 63
;     switch (Q_SIG(me)) {
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
    jnz X2
    cmp [X+1],1
    jz L9
X2:
    cmp [X+0],0
    jnz X3
    cmp [X+1],2
    jz L11
X3:
    cmp [X+0],0
    jnz X4
    cmp [X+1],3
    jz L13
X4:
    mov A,[X+1]
    sub A,1
    mov A,[X+0]
    xor A,-128
    sbb A,(0 ^ 0x80)
    jc L6
X5:
L22:
    cmp [X+0],0
    jnz X6
    cmp [X+1],9
    jz L15
X6:
    cmp [X+0],0
    jnz X7
    cmp [X+1],13
    jz L17
X7:
    cmp [X+0],0
    jnz X8
    cmp [X+1],14
    jz L21
X8:
    xjmp L6
X0:
    .dbline 63
L9:
    .dbline 64
;         case Q_ENTRY_SIG: {
    .dbline 65
;             BSP_display("s-ENTRY;");
    mov A,>L10
    push A
    mov A,<L10
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 66
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L5
L11:
    .dbline 68
;         }
;         case Q_EXIT_SIG: {
    .dbline 69
;             BSP_display("s-EXIT;");
    mov A,>L12
    push A
    mov A,<L12
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 70
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L5
L13:
    .dbline 72
;         }
;         case Q_INIT_SIG: {
    .dbline 73
;             BSP_display("s-INIT;");
    mov A,>L14
    push A
    mov A,<L14
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 74
;             return Q_TRAN(&QHsmTst_s11);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s11
    mvi [__r1],A
    mov A,<PL_QHsmTst_s11
    mvi [__r1],A
    mov A,2
    xjmp L5
L15:
    .dbline 76
;         }
;         case E_SIG: {
    .dbline 77
;             BSP_display("s-E;");
    mov A,>L16
    push A
    mov A,<L16
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 78
;             return Q_TRAN(&QHsmTst_s11);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s11
    mvi [__r1],A
    mov A,<PL_QHsmTst_s11
    mvi [__r1],A
    mov A,2
    xjmp L5
L17:
    .dbline 80
;         }
;         case I_SIG: {                   /* internal transition with a guard */
    .dbline 81
;             if (me->foo) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jz L7
    .dbline 81
    .dbline 82
;                 BSP_display("s-I;");
    mov A,>L20
    push A
    mov A,<L20
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 83
;                 me->foo = 0;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 84
;                 return Q_HANDLED();
    xjmp L5
X1:
    .dbline 86
;             }
;             break;
L21:
    .dbline 88
;         }
;         case TERMINATE_SIG: {
    .dbline 89
;             BSP_exit();
    xcall _BSP_exit
    .dbline 90
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L5
L6:
L7:
    .dbline 93
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
L5:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s QHsmTst_s1 _QHsmTst_s1 fc
;             me -> X-5
_QHsmTst_s1:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 96
; }
; /*..........................................................................*/
; static QState QHsmTst_s1(QHsmTst *me) {
    .dbline 97
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
    jnz X11
    cmp [X+1],1
    jz L27
X11:
    cmp [X+0],0
    jnz X12
    cmp [X+1],2
    jz L29
X12:
    cmp [X+0],0
    jnz X13
    cmp [X+1],3
    jz L31
X13:
    cmp [X+0],0
    jnz X14
    cmp [X+1],5
    jz L33
X14:
    cmp [X+0],0
    jnz X15
    cmp [X+1],6
    jz L35
X15:
    cmp [X+0],0
    jnz X16
    cmp [X+1],7
    jz L37
X16:
    cmp [X+0],0
    jnz X17
    cmp [X+1],8
    jz L39
X17:
    cmp [X+0],0
    jnz X18
    cmp [X+1],10
    jz L43
X18:
    cmp [X+0],0
    jnz X19
    cmp [X+1],13
    jz L45
X19:
    xjmp L24
X9:
    .dbline 97
;     switch (Q_SIG(me)) {
L27:
    .dbline 98
;         case Q_ENTRY_SIG: {
    .dbline 99
;             BSP_display("s1-ENTRY;");
    mov A,>L28
    push A
    mov A,<L28
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 100
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L23
L29:
    .dbline 102
;         }
;         case Q_EXIT_SIG: {
    .dbline 103
;             BSP_display("s1-EXIT;");
    mov A,>L30
    push A
    mov A,<L30
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 104
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L23
L31:
    .dbline 106
;         }
;         case Q_INIT_SIG: {
    .dbline 107
;             BSP_display("s1-INIT;");
    mov A,>L32
    push A
    mov A,<L32
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 108
;             return Q_TRAN(&QHsmTst_s11);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s11
    mvi [__r1],A
    mov A,<PL_QHsmTst_s11
    mvi [__r1],A
    mov A,2
    xjmp L23
L33:
    .dbline 110
;         }
;         case A_SIG: {
    .dbline 111
;             BSP_display("s1-A;");
    mov A,>L34
    push A
    mov A,<L34
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 112
;             return Q_TRAN(&QHsmTst_s1);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s1
    mvi [__r1],A
    mov A,<PL_QHsmTst_s1
    mvi [__r1],A
    mov A,2
    xjmp L23
L35:
    .dbline 114
;         }
;         case B_SIG: {
    .dbline 115
;             BSP_display("s1-B;");
    mov A,>L36
    push A
    mov A,<L36
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 116
;             return Q_TRAN(&QHsmTst_s11);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s11
    mvi [__r1],A
    mov A,<PL_QHsmTst_s11
    mvi [__r1],A
    mov A,2
    xjmp L23
L37:
    .dbline 118
;         }
;         case C_SIG: {
    .dbline 119
;             BSP_display("s1-C;");
    mov A,>L38
    push A
    mov A,<L38
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 120
;             return Q_TRAN(&QHsmTst_s2);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s2
    mvi [__r1],A
    mov A,<PL_QHsmTst_s2
    mvi [__r1],A
    mov A,2
    xjmp L23
L39:
    .dbline 122
;         }
;         case D_SIG: {                            /* transition with a gurad */
    .dbline 123
;             if (!me->foo) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L25
    .dbline 123
    .dbline 124
;                 BSP_display("s1-D;");
    mov A,>L42
    push A
    mov A,<L42
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 125
;                 me->foo = 1;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,1
    mvi [__r1],A
    .dbline 126
;                 return Q_TRAN(&QHsmTst_s);
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s
    mvi [__r1],A
    mov A,<PL_QHsmTst_s
    mvi [__r1],A
    mov A,2
    xjmp L23
X10:
    .dbline 128
;             }
;             break;
L43:
    .dbline 130
;         }
;         case F_SIG: {
    .dbline 131
;             BSP_display("s1-F;");
    mov A,>L44
    push A
    mov A,<L44
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 132
;             return Q_TRAN(&QHsmTst_s211);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s211
    mvi [__r1],A
    mov A,<PL_QHsmTst_s211
    mvi [__r1],A
    mov A,2
    xjmp L23
L45:
    .dbline 134
;         }
;         case I_SIG: {                                /* internal transition */
    .dbline 135
;             BSP_display("s1-I;");
    mov A,>L46
    push A
    mov A,<L46
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 136
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L23
L24:
L25:
    .dbline 139
;         }
;     }
;     return Q_SUPER(&QHsmTst_s);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s
    mvi [__r1],A
    mov A,<PL_QHsmTst_s
    mvi [__r1],A
    mov A,3
    .dbline -2
L23:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s QHsmTst_s11 _QHsmTst_s11 fc
;             me -> X-5
_QHsmTst_s11:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 142
; }
; /*..........................................................................*/
; static QState QHsmTst_s11(QHsmTst *me) {
    .dbline 143
;     switch (Q_SIG(me)) {
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
    jnz X22
    cmp [X+1],1
    jz L51
X22:
    cmp [X+0],0
    jnz X23
    cmp [X+1],2
    jz L53
X23:
    mov A,[X+1]
    sub A,1
    mov A,[X+0]
    xor A,-128
    sbb A,(0 ^ 0x80)
    jc L48
X24:
L63:
    cmp [X+0],0
    jnz X25
    cmp [X+1],8
    jz L55
X25:
    cmp [X+0],0
    jnz X26
    cmp [X+1],11
    jz L59
X26:
    cmp [X+0],0
    jnz X27
    cmp [X+1],12
    jz L61
X27:
    xjmp L48
X20:
    .dbline 143
L51:
    .dbline 144
;         case Q_ENTRY_SIG: {
    .dbline 145
;             BSP_display("s11-ENTRY;");
    mov A,>L52
    push A
    mov A,<L52
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 146
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L47
L53:
    .dbline 148
;         }
;         case Q_EXIT_SIG: {
    .dbline 149
;             BSP_display("s11-EXIT;");
    mov A,>L54
    push A
    mov A,<L54
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 150
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L47
L55:
    .dbline 152
;         }
;         case D_SIG: {                            /* transition with a gurad */
    .dbline 153
;             if (me->foo) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jz L49
    .dbline 153
    .dbline 154
;                 BSP_display("s11-D;");
    mov A,>L58
    push A
    mov A,<L58
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 155
;                 me->foo = 0;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 156
;                 return Q_TRAN(&QHsmTst_s1);
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s1
    mvi [__r1],A
    mov A,<PL_QHsmTst_s1
    mvi [__r1],A
    mov A,2
    xjmp L47
X21:
    .dbline 158
;             }
;             break;
L59:
    .dbline 160
;         }
;         case G_SIG: {
    .dbline 161
;             BSP_display("s11-G;");
    mov A,>L60
    push A
    mov A,<L60
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 162
;             return Q_TRAN(&QHsmTst_s211);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s211
    mvi [__r1],A
    mov A,<PL_QHsmTst_s211
    mvi [__r1],A
    mov A,2
    xjmp L47
L61:
    .dbline 164
;         }
;         case H_SIG: {
    .dbline 165
;             BSP_display("s11-H;");
    mov A,>L62
    push A
    mov A,<L62
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 166
;             return Q_TRAN(&QHsmTst_s);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s
    mvi [__r1],A
    mov A,<PL_QHsmTst_s
    mvi [__r1],A
    mov A,2
    xjmp L47
L48:
L49:
    .dbline 169
;         }
;     }
;     return Q_SUPER(&QHsmTst_s1);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s1
    mvi [__r1],A
    mov A,<PL_QHsmTst_s1
    mvi [__r1],A
    mov A,3
    .dbline -2
L47:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s QHsmTst_s2 _QHsmTst_s2 fc
;             me -> X-5
_QHsmTst_s2:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 172
; }
; /*..........................................................................*/
; static QState QHsmTst_s2(QHsmTst *me) {
    .dbline 173
;     switch (Q_SIG(me)) {
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
    jnz X30
    cmp [X+1],1
    jz L68
X30:
    cmp [X+0],0
    jnz X31
    cmp [X+1],2
    jz L70
X31:
    cmp [X+0],0
    jnz X32
    cmp [X+1],3
    jz L72
X32:
    cmp [X+0],0
    jnz X33
    cmp [X+1],7
    jz L74
X33:
    cmp [X+0],0
    jnz X34
    cmp [X+1],10
    jz L76
X34:
    mov A,[X+1]
    sub A,1
    mov A,[X+0]
    xor A,-128
    sbb A,(0 ^ 0x80)
    jc L65
X35:
L82:
    cmp [X+0],0
    jnz X36
    cmp [X+1],13
    jz L78
X36:
    xjmp L65
X28:
    .dbline 173
L68:
    .dbline 174
;         case Q_ENTRY_SIG: {
    .dbline 175
;             BSP_display("s2-ENTRY;");
    mov A,>L69
    push A
    mov A,<L69
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 176
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L64
L70:
    .dbline 178
;         }
;         case Q_EXIT_SIG: {
    .dbline 179
;             BSP_display("s2-EXIT;");
    mov A,>L71
    push A
    mov A,<L71
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 180
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L64
L72:
    .dbline 182
;         }
;         case Q_INIT_SIG: {
    .dbline 183
;             BSP_display("s2-INIT;");
    mov A,>L73
    push A
    mov A,<L73
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 184
;             return Q_TRAN(&QHsmTst_s211);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s211
    mvi [__r1],A
    mov A,<PL_QHsmTst_s211
    mvi [__r1],A
    mov A,2
    xjmp L64
L74:
    .dbline 186
;         }
;         case C_SIG: {
    .dbline 187
;             BSP_display("s2-C;");
    mov A,>L75
    push A
    mov A,<L75
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 188
;             return Q_TRAN(&QHsmTst_s1);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s1
    mvi [__r1],A
    mov A,<PL_QHsmTst_s1
    mvi [__r1],A
    mov A,2
    xjmp L64
L76:
    .dbline 190
;         }
;         case F_SIG: {
    .dbline 191
;             BSP_display("s2-F;");
    mov A,>L77
    push A
    mov A,<L77
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 192
;             return Q_TRAN(&QHsmTst_s11);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s11
    mvi [__r1],A
    mov A,<PL_QHsmTst_s11
    mvi [__r1],A
    mov A,2
    xjmp L64
L78:
    .dbline 194
;         }
;         case I_SIG: {                   /* internal transition with a guard */
    .dbline 195
;             if (!me->foo) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L66
    .dbline 195
    .dbline 196
;                 BSP_display("s2-I;");
    mov A,>L81
    push A
    mov A,<L81
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 197
;                 me->foo = (uint8_t)1;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,1
    mvi [__r1],A
    .dbline 198
;                 return Q_HANDLED();
    mov A,0
    xjmp L64
X29:
    .dbline 200
;             }
;             break;
L65:
L66:
    .dbline 203
;         }
;     }
;     return Q_SUPER(&QHsmTst_s);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s
    mvi [__r1],A
    mov A,<PL_QHsmTst_s
    mvi [__r1],A
    mov A,3
    .dbline -2
L64:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s QHsmTst_s21 _QHsmTst_s21 fc
;             me -> X-5
_QHsmTst_s21:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 206
; }
; /*..........................................................................*/
; static QState QHsmTst_s21(QHsmTst *me) {
    .dbline 207
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
    jnz X38
    cmp [X+1],1
    jz L87
X38:
    cmp [X+0],0
    jnz X39
    cmp [X+1],2
    jz L89
X39:
    cmp [X+0],0
    jnz X40
    cmp [X+1],3
    jz L91
X40:
    cmp [X+0],0
    jnz X41
    cmp [X+1],5
    jz L93
X41:
    cmp [X+0],0
    jnz X42
    cmp [X+1],6
    jz L95
X42:
    cmp [X+0],0
    jnz X43
    cmp [X+1],11
    jz L97
X43:
    xjmp L84
X37:
    .dbline 207
;     switch (Q_SIG(me)) {
L87:
    .dbline 208
;         case Q_ENTRY_SIG: {
    .dbline 209
;             BSP_display("s21-ENTRY;");
    mov A,>L88
    push A
    mov A,<L88
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 210
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L83
L89:
    .dbline 212
;         }
;         case Q_EXIT_SIG: {
    .dbline 213
;             BSP_display("s21-EXIT;");
    mov A,>L90
    push A
    mov A,<L90
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 214
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L83
L91:
    .dbline 216
;         }
;         case Q_INIT_SIG: {
    .dbline 217
;             BSP_display("s21-INIT;");
    mov A,>L92
    push A
    mov A,<L92
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 218
;             return Q_TRAN(&QHsmTst_s211);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s211
    mvi [__r1],A
    mov A,<PL_QHsmTst_s211
    mvi [__r1],A
    mov A,2
    xjmp L83
L93:
    .dbline 220
;         }
;         case A_SIG: {
    .dbline 221
;             BSP_display("s21-A;");
    mov A,>L94
    push A
    mov A,<L94
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 222
;             return Q_TRAN(&QHsmTst_s21);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s21
    mvi [__r1],A
    mov A,<PL_QHsmTst_s21
    mvi [__r1],A
    mov A,2
    xjmp L83
L95:
    .dbline 224
;         }
;         case B_SIG: {
    .dbline 225
;             BSP_display("s21-B;");
    mov A,>L96
    push A
    mov A,<L96
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 226
;             return Q_TRAN(&QHsmTst_s211);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s211
    mvi [__r1],A
    mov A,<PL_QHsmTst_s211
    mvi [__r1],A
    mov A,2
    xjmp L83
L97:
    .dbline 228
;         }
;         case G_SIG: {
    .dbline 229
;             BSP_display("s21-G;");
    mov A,>L98
    push A
    mov A,<L98
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 230
;             return Q_TRAN(&QHsmTst_s1);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s1
    mvi [__r1],A
    mov A,<PL_QHsmTst_s1
    mvi [__r1],A
    mov A,2
    xjmp L83
L84:
    .dbline 233
;         }
;     }
;     return Q_SUPER(&QHsmTst_s2);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s2
    mvi [__r1],A
    mov A,<PL_QHsmTst_s2
    mvi [__r1],A
    mov A,3
    .dbline -2
L83:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s QHsmTst_s211 _QHsmTst_s211 fc
;             me -> X-5
_QHsmTst_s211:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 236
; }
; /*..........................................................................*/
; static QState QHsmTst_s211(QHsmTst *me) {
    .dbline 237
;     switch (Q_SIG(me)) {
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
    mov A,[X+1]
    sub A,8
    mov [__rY],A
    mov A,[X+0]
    xor A,-128
    sbb A,(0 ^ 0x80)
    or A,[__rY]
    jz L107
    jnc L112
X45:
L111:
    cmp [X+0],0
    jnz X46
    cmp [X+1],1
    jz L103
X46:
    cmp [X+0],0
    jnz X47
    cmp [X+1],2
    jz L105
X47:
    xjmp L100
L112:
    cmp [X+0],0
    jnz X48
    cmp [X+1],12
    jz L109
X48:
    xjmp L100
X44:
    .dbline 237
L103:
    .dbline 238
;         case Q_ENTRY_SIG: {
    .dbline 239
;             BSP_display("s211-ENTRY;");
    mov A,>L104
    push A
    mov A,<L104
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 240
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L99
L105:
    .dbline 242
;         }
;         case Q_EXIT_SIG: {
    .dbline 243
;             BSP_display("s211-EXIT;");
    mov A,>L106
    push A
    mov A,<L106
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 244
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L99
L107:
    .dbline 246
;         }
;         case D_SIG: {
    .dbline 247
;             BSP_display("s211-D;");
    mov A,>L108
    push A
    mov A,<L108
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 248
;             return Q_TRAN(&QHsmTst_s21);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s21
    mvi [__r1],A
    mov A,<PL_QHsmTst_s21
    mvi [__r1],A
    mov A,2
    xjmp L99
L109:
    .dbline 250
;         }
;         case H_SIG: {
    .dbline 251
;             BSP_display("s211-H;");
    mov A,>L110
    push A
    mov A,<L110
    push A
    xcall _BSP_display
    add SP,-2
    .dbline 252
;             return Q_TRAN(&QHsmTst_s);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s
    mvi [__r1],A
    mov A,<PL_QHsmTst_s
    mvi [__r1],A
    mov A,2
    xjmp L99
L100:
    .dbline 255
;         }
;     }
;     return Q_SUPER(&QHsmTst_s21);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_QHsmTst_s21
    mvi [__r1],A
    mov A,<PL_QHsmTst_s21
    mvi [__r1],A
    mov A,3
    .dbline -2
L99:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .area data(ram, con, rel)
    .dbfile ./qhsmtst.c
_HSM_QHsmTst::
    .byte 0,0,0,0,0
    .dbsym e HSM_QHsmTst _HSM_QHsmTst X
    .area lit(rom, con, rel)
L110:
    .byte 's,50,49,49,45,'H,59,0
L108:
    .byte 's,50,49,49,45,'D,59,0
L106:
    .byte 's,50,49,49,45,'E,'X,'I,'T,59,0
L104:
    .byte 's,50,49,49,45,'E,'N,'T,'R,'Y,59,0
L98:
    .byte 's,50,49,45,'G,59,0
L96:
    .byte 's,50,49,45,'B,59,0
L94:
    .byte 's,50,49,45,'A,59,0
L92:
    .byte 's,50,49,45,'I,'N,'I,'T,59,0
L90:
    .byte 's,50,49,45,'E,'X,'I,'T,59,0
L88:
    .byte 's,50,49,45,'E,'N,'T,'R,'Y,59,0
L81:
    .byte 's,50,45,'I,59,0
L77:
    .byte 's,50,45,'F,59,0
L75:
    .byte 's,50,45,'C,59,0
L73:
    .byte 's,50,45,'I,'N,'I,'T,59,0
L71:
    .byte 's,50,45,'E,'X,'I,'T,59,0
L69:
    .byte 's,50,45,'E,'N,'T,'R,'Y,59,0
L62:
    .byte 's,49,49,45,'H,59,0
L60:
    .byte 's,49,49,45,'G,59,0
L58:
    .byte 's,49,49,45,'D,59,0
L54:
    .byte 's,49,49,45,'E,'X,'I,'T,59,0
L52:
    .byte 's,49,49,45,'E,'N,'T,'R,'Y,59,0
L46:
    .byte 's,49,45,'I,59,0
L44:
    .byte 's,49,45,'F,59,0
L42:
    .byte 's,49,45,'D,59,0
L38:
    .byte 's,49,45,'C,59,0
L36:
    .byte 's,49,45,'B,59,0
L34:
    .byte 's,49,45,'A,59,0
L32:
    .byte 's,49,45,'I,'N,'I,'T,59,0
L30:
    .byte 's,49,45,'E,'X,'I,'T,59,0
L28:
    .byte 's,49,45,'E,'N,'T,'R,'Y,59,0
L20:
    .byte 's,45,'I,59,0
L16:
    .byte 's,45,'E,59,0
L14:
    .byte 's,45,'I,'N,'I,'T,59,0
L12:
    .byte 's,45,'E,'X,'I,'T,59,0
L10:
    .byte 's,45,'E,'N,'T,'R,'Y,59,0
L4:
    .byte 't,'o,'p,45,'I,'N,'I,'T,59,0
    .area func_lit
PL_QHsmTst_s21:    .word _QHsmTst_s21
PL_QHsmTst_s211:    .word _QHsmTst_s211
PL_QHsmTst_s:    .word _QHsmTst_s
PL_QHsmTst_s1:    .word _QHsmTst_s1
PL_QHsm_top:    .word _QHsm_top
PL_QHsmTst_s11:    .word _QHsmTst_s11
PL_QHsmTst_s2:    .word _QHsmTst_s2
PL_QHsmTst_initial:    .word _QHsmTst_initial

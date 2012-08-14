    cpu LMM
    .module pelican.c
    .area text(rom, con, rel)
    .dbfile ./pelican.c
    .dbfunc e Pelican_ctor _Pelican_ctor fV
_Pelican_ctor::
    .dbline -1
    .dbline 64
; /*****************************************************************************
; * Product: PELICAN crossing example
; * Last Updated for Version: 4.0.00
; * Date of the Last Update:  Apr 05, 2008
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
; /* Pelican class -----------------------------------------------------------*/
; typedef struct PelicanTag {
;     QActive super;                                  /* derived from QActive */
;     uint8_t flashCtr;                           /* pedestrian flash counter */
; } Pelican;
;
; static QState Pelican_initial         (Pelican *me);
; static QState Pelican_offline         (Pelican *me);
; static QState Pelican_operational     (Pelican *me);
; static QState Pelican_carsEnabled     (Pelican *me);
; static QState Pelican_carsGreen       (Pelican *me);
; static QState Pelican_carsGreenNoPed  (Pelican *me);
; static QState Pelican_carsGreenPedWait(Pelican *me);
; static QState Pelican_carsGreenInt    (Pelican *me);
; static QState Pelican_carsYellow      (Pelican *me);
; static QState Pelican_pedsEnabled     (Pelican *me);
; static QState Pelican_pedsWalk        (Pelican *me);
; static QState Pelican_pedsFlash       (Pelican *me);
;
; enum PelicanTimeouts {                         /* various timeouts in ticks */
;     CARS_GREEN_MIN_TOUT = BSP_TICKS_PER_SEC * 8,      /* min green for cars */
;     CARS_YELLOW_TOUT = BSP_TICKS_PER_SEC * 3,            /* yellow for cars */
;     PEDS_WALK_TOUT   = BSP_TICKS_PER_SEC * 3,      /* walking time for peds */
;     PEDS_FLASH_TOUT  = BSP_TICKS_PER_SEC / 5,  /* flashing timeout for peds */
;     PEDS_FLASH_NUM   = 5*2,                   /* number of flashes for peds */
;     OFF_FLASH_TOUT   = BSP_TICKS_PER_SEC / 2   /* flashing timeout when off */
; };
;
; /* Global objects ----------------------------------------------------------*/
; Pelican AO_Pelican;     /* the single instance of the Pelican active object */
;
; /*..........................................................................*/
; void Pelican_ctor(void) {
    .dbline 65
;     QActive_ctor((QActive *)&AO_Pelican, (QStateHandler)&Pelican_initial);
    mov REG[0xd0],>_AO_Pelican
    mov [_AO_Pelican+1],<PL_Pelican_initial
    mov [_AO_Pelican],>PL_Pelican_initial
    .dbline -2
    .dbline 66
; }
L1:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc s Pelican_initial _Pelican_initial fc
;             me -> X-5
_Pelican_initial:
    .dbline -1
    push X
    mov X,SP
    .dbline 69
;
; /* HSM definition ----------------------------------------------------------*/
; static QState Pelican_initial(Pelican *me) {
    .dbline 70
;     return Q_TRAN(&Pelican_operational);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_operational
    mvi [__r1],A
    mov A,<PL_Pelican_operational
    mvi [__r1],A
    mov A,2
    .dbline -2
L2:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_operational _Pelican_operational fc
;             me -> X-5
_Pelican_operational:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 73
; }
; /*..........................................................................*/
; static QState Pelican_operational(Pelican *me) {
    .dbline 74
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
    cmp [X+1],3
    jz L8
X2:
    cmp [X+0],0
    jnz X3
    cmp [X+1],6
    jz L9
X3:
    xjmp L4
X0:
    .dbline 74
;     switch (Q_SIG(me)) {
L7:
    .dbline 75
;         case Q_ENTRY_SIG: {
    .dbline 76
;             BSP_signalCars(CARS_RED);
    mov A,0
    push A
    xcall _BSP_signalCars
    .dbline 77
;             BSP_signalPeds(PEDS_DONT_WALK);
    mov A,0
    push A
    xcall _BSP_signalPeds
    add SP,-2
    .dbline 78
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L3
L8:
    .dbline 80
;         }
;         case Q_INIT_SIG: {
    .dbline 81
;             return Q_TRAN(&Pelican_carsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,2
    xjmp L3
L9:
    .dbline 83
;         }
;         case OFF_SIG: {
    .dbline 84
;             return Q_TRAN(&Pelican_offline);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_offline
    mvi [__r1],A
    mov A,<PL_Pelican_offline
    mvi [__r1],A
    mov A,2
    xjmp L3
L4:
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
L3:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_carsEnabled _Pelican_carsEnabled fc
;             me -> X-5
_Pelican_carsEnabled:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 90
; }
; /*..........................................................................*/
; static QState Pelican_carsEnabled(Pelican *me) {
    .dbline 91
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
    jnz X5
    cmp [X+1],2
    jz L14
X5:
    cmp [X+0],0
    jnz X6
    cmp [X+1],3
    jz L15
X6:
    xjmp L11
X4:
    .dbline 91
;     switch (Q_SIG(me)) {
L14:
    .dbline 92
;         case Q_EXIT_SIG: {
    .dbline 93
;             BSP_signalCars(CARS_RED);
    mov A,0
    push A
    xcall _BSP_signalCars
    add SP,-1
    .dbline 94
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L10
L15:
    .dbline 96
;         }
;         case Q_INIT_SIG: {
    .dbline 97
;             return Q_TRAN(&Pelican_carsGreen);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,2
    xjmp L10
L11:
    .dbline 100
;         }
;     }
;     return Q_SUPER(&Pelican_operational);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_operational
    mvi [__r1],A
    mov A,<PL_Pelican_operational
    mvi [__r1],A
    mov A,3
    .dbline -2
L10:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_carsGreen _Pelican_carsGreen fc
;             me -> X-5
_Pelican_carsGreen:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 103
; }
; /*..........................................................................*/
; static QState Pelican_carsGreen(Pelican *me) {
    .dbline 104
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
    jnz X8
    cmp [X+1],1
    jz L20
X8:
    cmp [X+0],0
    jnz X9
    cmp [X+1],3
    jz L21
X9:
    xjmp L17
X7:
    .dbline 104
;     switch (Q_SIG(me)) {
L20:
    .dbline 105
;         case Q_ENTRY_SIG: {
    .dbline 106
;             QActive_arm((QActive *)me, CARS_GREEN_MIN_TOUT);
    mov A,2
    push A
    mov A,0
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    .dbline 107
;             BSP_signalCars(CARS_GREEN);
    mov A,2
    push A
    xcall _BSP_signalCars
    add SP,-5
    .dbline 108
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L16
L21:
    .dbline 110
;         }
;         case Q_INIT_SIG: {
    .dbline 111
;             return Q_TRAN(&Pelican_carsGreenNoPed);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreenNoPed
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreenNoPed
    mvi [__r1],A
    mov A,2
    xjmp L16
L17:
    .dbline 114
;         }
;     }
;     return Q_SUPER(&Pelican_carsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,3
    .dbline -2
L16:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_carsGreenNoPed _Pelican_carsGreenNoPed fc
;             me -> X-5
_Pelican_carsGreenNoPed:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 117
; }
; /*..........................................................................*/
; static QState Pelican_carsGreenNoPed(Pelican *me) {
    .dbline 118
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
    jz L26
X11:
    cmp [X+0],0
    jnz X12
    cmp [X+1],4
    jz L29
X12:
    cmp [X+0],0
    jnz X13
    cmp [X+1],5
    jz L28
X13:
    xjmp L23
X10:
    .dbline 118
;     switch (Q_SIG(me)) {
L26:
    .dbline 119
;         case Q_ENTRY_SIG: {
    .dbline 120
;             BSP_showState(me->super.prio, "carsGreenNoPed");
    mov A,>L27
    push A
    mov A,<L27
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    add SP,-3
    .dbline 121
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L22
L28:
    .dbline 123
;         }
;         case PEDS_WAITING_SIG: {
    .dbline 124
;             return Q_TRAN(&Pelican_carsGreenPedWait);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreenPedWait
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreenPedWait
    mvi [__r1],A
    mov A,2
    xjmp L22
L29:
    .dbline 126
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 127
;             return Q_TRAN(&Pelican_carsGreenInt);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreenInt
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreenInt
    mvi [__r1],A
    mov A,2
    xjmp L22
L23:
    .dbline 130
;         }
;     }
;     return Q_SUPER(&Pelican_carsGreen);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,3
    .dbline -2
L22:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_carsGreenPedWait _Pelican_carsGreenPedWait fc
;             me -> X-5
_Pelican_carsGreenPedWait:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 133
; }
; /*..........................................................................*/
; static QState Pelican_carsGreenPedWait(Pelican *me) {
    .dbline 134
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
    jnz X15
    cmp [X+1],1
    jz L34
X15:
    cmp [X+0],0
    jnz X16
    cmp [X+1],4
    jz L36
X16:
    xjmp L31
X14:
    .dbline 134
;     switch (Q_SIG(me)) {
L34:
    .dbline 135
;         case Q_ENTRY_SIG: {
    .dbline 136
;             BSP_showState(me->super.prio, "carsGreenPedWait");
    mov A,>L35
    push A
    mov A,<L35
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    add SP,-3
    .dbline 137
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L30
L36:
    .dbline 139
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 140
;             return Q_TRAN(&Pelican_carsYellow);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsYellow
    mvi [__r1],A
    mov A,<PL_Pelican_carsYellow
    mvi [__r1],A
    mov A,2
    xjmp L30
L31:
    .dbline 143
;         }
;     }
;     return Q_SUPER(&Pelican_carsGreen);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,3
    .dbline -2
L30:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_carsGreenInt _Pelican_carsGreenInt fc
;             me -> X-5
_Pelican_carsGreenInt:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 146
; }
; /*..........................................................................*/
; static QState Pelican_carsGreenInt(Pelican *me) {
    .dbline 147
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
    sub A,1
    mov [__rY],A
    mov A,[X+0]
    xor A,-128
    sbb A,(0 ^ 0x80)
    jc L38
    or A,[__rY]
    jz L41
X18:
L44:
    cmp [X+0],0
    jnz X19
    cmp [X+1],5
    jz L43
X19:
    xjmp L38
X17:
    .dbline 147
L41:
    .dbline 148
;         case Q_ENTRY_SIG: {
    .dbline 149
;             BSP_showState(me->super.prio, "carsGreenInt");
    mov A,>L42
    push A
    mov A,<L42
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    add SP,-3
    .dbline 150
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L37
L43:
    .dbline 152
;         }
;         case PEDS_WAITING_SIG: {
    .dbline 153
;             return Q_TRAN(&Pelican_carsYellow);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsYellow
    mvi [__r1],A
    mov A,<PL_Pelican_carsYellow
    mvi [__r1],A
    mov A,2
    xjmp L37
L38:
    .dbline 156
;         }
;     }
;     return Q_SUPER(&Pelican_carsGreen);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,<PL_Pelican_carsGreen
    mvi [__r1],A
    mov A,3
    .dbline -2
L37:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_carsYellow _Pelican_carsYellow fc
;             me -> X-5
_Pelican_carsYellow:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 159
; }
; /*..........................................................................*/
; static QState Pelican_carsYellow(Pelican *me) {
    .dbline 160
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
    jnz X21
    cmp [X+1],1
    jz L49
X21:
    cmp [X+0],0
    jnz X22
    cmp [X+1],4
    jz L51
X22:
    xjmp L46
X20:
    .dbline 160
;     switch (Q_SIG(me)) {
L49:
    .dbline 161
;         case Q_ENTRY_SIG: {
    .dbline 162
;             BSP_showState(me->super.prio, "carsYellow");
    mov A,>L50
    push A
    mov A,<L50
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    add SP,-3
    .dbline 163
;             QActive_arm((QActive *)me, CARS_YELLOW_TOUT);
    mov A,0
    push A
    mov A,-64
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    .dbline 164
;             BSP_signalCars(CARS_YELLOW);
    mov A,1
    push A
    xcall _BSP_signalCars
    add SP,-5
    .dbline 165
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L45
L51:
    .dbline 167
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 168
;             return Q_TRAN(&Pelican_pedsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_pedsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_pedsEnabled
    mvi [__r1],A
    mov A,2
    xjmp L45
L46:
    .dbline 171
;         }
;     }
;     return Q_SUPER(&Pelican_carsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,3
    .dbline -2
L45:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_pedsEnabled _Pelican_pedsEnabled fc
;             me -> X-5
_Pelican_pedsEnabled:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 174
; }
; /*..........................................................................*/
; static QState Pelican_pedsEnabled(Pelican *me) {
    .dbline 175
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
    jnz X24
    cmp [X+1],2
    jz L56
X24:
    cmp [X+0],0
    jnz X25
    cmp [X+1],3
    jz L57
X25:
    xjmp L53
X23:
    .dbline 175
;     switch (Q_SIG(me)) {
L56:
    .dbline 176
;         case Q_EXIT_SIG: {
    .dbline 177
;             BSP_signalPeds(PEDS_DONT_WALK);
    mov A,0
    push A
    xcall _BSP_signalPeds
    add SP,-1
    .dbline 178
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L52
L57:
    .dbline 180
;         }
;         case Q_INIT_SIG: {
    .dbline 181
;             return Q_TRAN(&Pelican_pedsWalk);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_pedsWalk
    mvi [__r1],A
    mov A,<PL_Pelican_pedsWalk
    mvi [__r1],A
    mov A,2
    xjmp L52
L53:
    .dbline 184
;         }
;     }
;     return Q_SUPER(&Pelican_operational);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_operational
    mvi [__r1],A
    mov A,<PL_Pelican_operational
    mvi [__r1],A
    mov A,3
    .dbline -2
L52:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_pedsWalk _Pelican_pedsWalk fc
;             me -> X-5
_Pelican_pedsWalk:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 187
; }
; /*..........................................................................*/
; static QState Pelican_pedsWalk(Pelican *me) {
    .dbline 188
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
    jnz X27
    cmp [X+1],1
    jz L62
X27:
    cmp [X+0],0
    jnz X28
    cmp [X+1],4
    jz L64
X28:
    xjmp L59
X26:
    .dbline 188
;     switch (Q_SIG(me)) {
L62:
    .dbline 189
;         case Q_ENTRY_SIG: {
    .dbline 190
;             BSP_showState(me->super.prio, "pedsWalk");
    mov A,>L63
    push A
    mov A,<L63
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    add SP,-3
    .dbline 191
;             QActive_arm((QActive *)me, PEDS_WALK_TOUT);
    mov A,0
    push A
    mov A,-64
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    .dbline 192
;             BSP_signalPeds(PEDS_WALK);
    mov A,2
    push A
    xcall _BSP_signalPeds
    add SP,-5
    .dbline 193
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L58
L64:
    .dbline 195
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 196
;             return Q_TRAN(&Pelican_pedsFlash);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_pedsFlash
    mvi [__r1],A
    mov A,<PL_Pelican_pedsFlash
    mvi [__r1],A
    mov A,2
    xjmp L58
L59:
    .dbline 199
;         }
;     }
;     return Q_SUPER(&Pelican_pedsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_pedsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_pedsEnabled
    mvi [__r1],A
    mov A,3
    .dbline -2
L58:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_pedsFlash _Pelican_pedsFlash fc
;             me -> X-5
_Pelican_pedsFlash:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 202
; }
; /*..........................................................................*/
; static QState Pelican_pedsFlash(Pelican *me) {
    .dbline 203
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
    jz L69
X30:
    cmp [X+0],0
    jnz X31
    cmp [X+1],4
    jz L71
X31:
    xjmp L66
X29:
    .dbline 203
;     switch (Q_SIG(me)) {
L69:
    .dbline 204
;         case Q_ENTRY_SIG: {
    .dbline 205
;             BSP_showState(me->super.prio, "pedsFlash");
    mov A,>L70
    push A
    mov A,<L70
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    .dbline 206
;             QActive_arm((QActive *)me, PEDS_FLASH_TOUT);
    mov A,0
    push A
    mov A,12
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    add SP,-7
    .dbline 207
;             me->flashCtr = PEDS_FLASH_NUM*2 + 1;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,21
    mvi [__r1],A
    .dbline 208
;             return Q_HANDLED();
    mov A,0
    xjmp L65
L71:
    .dbline 210
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 211
;             if (me->flashCtr != 0) {                     /* still flashing? */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jz L72
    .dbline 211
    .dbline 212
;                 if ((me->flashCtr & 1) == 0) {             /* even counter? */
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    tst [__r0],1
    jnz L74
    .dbline 212
    .dbline 213
;                     BSP_signalPeds(PEDS_DONT_WALK);
    mov A,0
    push A
    xcall _BSP_signalPeds
    add SP,-1
    .dbline 214
;                 }
    xjmp L75
L74:
    .dbline 215
;                 else {                               /* must be odd counter */
    .dbline 216
;                     BSP_signalPeds(PEDS_BLANK);
    mov A,1
    push A
    xcall _BSP_signalPeds
    add SP,-1
    .dbline 217
;                 }
L75:
    .dbline 218
;                 --me->flashCtr;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    dec [__r1]
    mov [__r2],A
    sub [__r2],1
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    .dbline 219
;                 QActive_arm((QActive *)me, PEDS_FLASH_TOUT);
    mov A,0
    push A
    mov A,12
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    add SP,-4
    .dbline 220
;             }
    xjmp L73
L72:
    .dbline 221
;             else {                                         /* done flashing */
    .dbline 222
;                 return Q_TRAN(&Pelican_carsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_carsEnabled
    mvi [__r1],A
    mov A,2
    xjmp L65
L73:
    .dbline 224
;             }
;             return Q_HANDLED();
    mov REG[0xd0],>__r0
    mov A,0
    xjmp L65
L66:
    .dbline 227
;         }
;     }
;     return Q_SUPER(&Pelican_pedsEnabled);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_pedsEnabled
    mvi [__r1],A
    mov A,<PL_Pelican_pedsEnabled
    mvi [__r1],A
    mov A,3
    .dbline -2
L65:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc s Pelican_offline _Pelican_offline fc
;             me -> X-5
_Pelican_offline:
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 230
; }
; /*..........................................................................*/
; static QState Pelican_offline(Pelican *me) {
    .dbline 231
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
    jnz X33
    cmp [X+1],1
    jz L80
X33:
    cmp [X+0],0
    jnz X34
    cmp [X+1],4
    jz L82
X34:
    mov A,[X+1]
    sub A,1
    mov A,[X+0]
    xor A,-128
    sbb A,(0 ^ 0x80)
    jc L77
X35:
L86:
    cmp [X+0],0
    jnz X36
    cmp [X+1],7
    jz L85
X36:
    xjmp L77
X32:
    .dbline 231
L80:
    .dbline 232
;         case Q_ENTRY_SIG: {
    .dbline 233
;             BSP_showState(me->super.prio, "offline");
    mov A,>L81
    push A
    mov A,<L81
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    push A
    xcall _BSP_showState
    .dbline 234
;             QActive_arm((QActive *)me, OFF_FLASH_TOUT);
    mov A,0
    push A
    mov A,32
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    add SP,-7
    .dbline 235
;             me->flashCtr = 0;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 236
;             return Q_HANDLED();
    xjmp L76
L82:
    .dbline 238
;         }
;         case Q_TIMEOUT_SIG: {
    .dbline 239
;             QActive_arm((QActive *)me, OFF_FLASH_TOUT);
    mov A,0
    push A
    mov A,32
    push A
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    xcall _QActive_arm
    add SP,-4
    .dbline 240
;             if ((me->flashCtr & 1) == 0) {                 /* even counter? */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    tst [__r0],1
    jnz L83
    .dbline 240
    .dbline 241
;                 BSP_signalCars(CARS_RED);
    mov A,0
    push A
    xcall _BSP_signalCars
    .dbline 242
;                 BSP_signalPeds(PEDS_DONT_WALK);
    mov A,0
    push A
    xcall _BSP_signalPeds
    add SP,-2
    .dbline 243
;             }
    xjmp L84
L83:
    .dbline 244
;             else {
    .dbline 245
;                 BSP_signalCars(CARS_OFF);
    mov A,3
    push A
    xcall _BSP_signalCars
    .dbline 246
;                 BSP_signalPeds(PEDS_BLANK);
    mov A,1
    push A
    xcall _BSP_signalPeds
    add SP,-2
    .dbline 247
;             }
L84:
    .dbline 248
;             me->flashCtr ^= 1;                  /* toggle the flash counter */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,10
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    dec [__r1]
    mov [__r2],A
    xor [__r2],1
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    .dbline 249
;             return Q_HANDLED();
    mov A,0
    xjmp L76
L85:
    .dbline 251
;         }
;         case ON_SIG: {
    .dbline 252
;             return Q_TRAN(&Pelican_operational);
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,>PL_Pelican_operational
    mvi [__r1],A
    mov A,<PL_Pelican_operational
    mvi [__r1],A
    mov A,2
    xjmp L76
L77:
    .dbline 255
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
L76:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .area data(ram, con, rel)
    .dbfile ./pelican.c
_AO_Pelican::
    .word 0,0,0,0,0
    .byte 0
    .dbsym e AO_Pelican _AO_Pelican X
    .area lit(rom, con, rel)
L81:
    .byte 'o,'f,'f,'l,'i,'n,'e,0
L70:
    .byte 'p,'e,'d,'s,'F,'l,'a,'s,'h,0
L63:
    .byte 'p,'e,'d,'s,'W,'a,'l,'k,0
L50:
    .byte 'c,'a,'r,'s,'Y,'e,'l,'l,'o,'w,0
L42:
    .byte 'c,'a,'r,'s,'G,'r,'e,'e,'n,'I,'n,'t,0
L35:
    .byte 'c,'a,'r,'s,'G,'r,'e,'e,'n,'P,'e,'d,'W,'a,'i,'t
    .byte 0
L27:
    .byte 'c,'a,'r,'s,'G,'r,'e,'e,'n,'N,'o,'P,'e,'d,0
    .area func_lit
PL_Pelican_pedsFlash:    .word _Pelican_pedsFlash
PL_Pelican_pedsWalk:    .word _Pelican_pedsWalk
PL_Pelican_pedsEnabled:    .word _Pelican_pedsEnabled
PL_Pelican_carsYellow:    .word _Pelican_carsYellow
PL_Pelican_carsGreenInt:    .word _Pelican_carsGreenInt
PL_Pelican_carsGreenPedWait:    .word _Pelican_carsGreenPedWait
PL_Pelican_carsGreenNoPed:    .word _Pelican_carsGreenNoPed
PL_Pelican_carsGreen:    .word _Pelican_carsGreen
PL_QHsm_top:    .word _QHsm_top
PL_Pelican_offline:    .word _Pelican_offline
PL_Pelican_carsEnabled:    .word _Pelican_carsEnabled
PL_Pelican_operational:    .word _Pelican_operational
PL_Pelican_initial:    .word _Pelican_initial

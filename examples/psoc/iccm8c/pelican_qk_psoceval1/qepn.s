    cpu LMM
    .module qepn.c
    .area lit(rom, con, rel)
_l_this_file:
    .byte 'q,'e,'p,'n,0
    .dbfile ./qepn.c
    .dbsym s l_this_file _l_this_file A[5:5]c
L2:
    .byte 52,46,48,46,48,50,0
    .area text(rom, con, rel)
    .dbfile ./qepn.c
    .dbfunc e QP_getVersion _QP_getVersion fpc
    .dbsym s version L2 A[7:7]c
_QP_getVersion::
    .dbline -1
    .dbline 48
; /*****************************************************************************
; * Product: QEP-nano implemenation
; * Last Updated for Version: 4.0.02
; * Date of the Last Update:  Jul 11, 2008
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
; #include "qpn_port.h"                                       /* QP-nano port */
;
; #ifndef Q_NHSM
; Q_DEFINE_THIS_MODULE(qepn)
; #endif
;
; /**
; * \file
; * \ingroup qepn qfn
; * QEP-nano implementation.
; */
;
; /** empty signal for internal use only */
; #define QEP_EMPTY_SIG_        0
;
; /** maximum depth of state nesting (including the top level), must be >= 2 */
; #define QEP_MAX_NEST_DEPTH_   5
;
; /*..........................................................................*/
; /*lint -e970 -e971 */      /* ignore MISRA rules 13 and 14 in this function */
; char const Q_ROM * Q_ROM_VAR QP_getVersion(void) {
    .dbline 50
;     static char const Q_ROM Q_ROM_VAR version[] = "4.0.02";
;     return version;
    mov REG[0xd0],>__r0
    mov [__r1],<L2
    mov [__r0],>L2
    .dbline -2
L1:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e QHsm_top _QHsm_top fc
;             me -> X-5
_QHsm_top::
    .dbline -1
    push X
    mov X,SP
    .dbline 81
; }
;
; #ifndef Q_NFSM
; /*..........................................................................*/
; void QFsm_init(QFsm *me) {
;     (void)(*me->state)(me);      /* execute the top-most initial transition */
;
;     Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
;     (void)(*me->state)(me);                             /* enter the target */
; }
; /*..........................................................................*/
; #ifndef QK_PREEMPTIVE
; void QFsm_dispatch(QFsm *me) {
; #else
; void QFsm_dispatch(QFsm *me) Q_REENTRANT {
; #endif
;     QStateHandler s = me->state;
;
;     if ((*s)(me) == Q_RET_TRAN) {                      /* transition taken? */
;         Q_SIG(me) = (QSignal)Q_EXIT_SIG;
;         (void)(*s)(me);                                  /* exit the source */
;
;         Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
;         (void)(*me->state)(me);                         /* enter the target */
;     }
; }
; #endif                                                            /* Q_NFSM */
;
; #ifndef Q_NHSM
; /*..........................................................................*/
; QState QHsm_top(QHsm *me) {
    .dbline 82
;     (void)me;             /* supress the "unused argument" compiler warning */
    .dbline 83
;     return Q_IGNORED();                 /* the top state ignores all events */
    mov REG[0xd0],>__r0
    mov A,1
    .dbline -2
L3:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .dbfunc e QHsm_init _QHsm_init fV
;           path -> X+3
;             ip -> X+2
;              t -> X+0
;             me -> X-5
_QHsm_init::
    .dbline -1
    push X
    mov X,SP
    add SP,13
    .dbline 86
; }
; /*..........................................................................*/
; void QHsm_init(QHsm *me) {
    .dbline 89
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    cmp A,2
    .dbline 89
    .dbline 89
    jz L6
L5:
    .dbline 89
;     QStateHandler t;
;
;     Q_ALLEGE((*me->state)(me) == Q_RET_TRAN);/* initial tran. must be taken */
    mov A,0
    push A
    mov A,89
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L6:
    .dbline 91
;
;     t = (QStateHandler)&QHsm_top;         /* an HSM starts in the top state */
    mov [X+1],<PL_QHsm_top
    mov [X+0],>PL_QHsm_top
L7:
    .dbline 92
;     do {                              /* drill into the target hierarchy... */
    .dbline 94
;         QStateHandler path[QEP_MAX_NEST_DEPTH_];
;         int8_t ip = (int8_t)0;
    mov [X+2],0
    .dbline 96
;
;         path[0] = me->state;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+3],A
    mvi A,[__r1]
    mov [X+4],A
    .dbline 97
;         Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 98
;         (void)(*me->state)(me);
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    xjmp L11
L10:
    .dbline 99
    .dbline 100
    mov REG[0xd0],>__r0
    mov A,[X+2]
    add A,1
    mov [__r0],A
    mov [X+2],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r4],7
    mov [__r5],X
    add [__r5],3
    mov A,[__r0]
    mov [__r1],A
    mov [__r0],0
    tst [__r1],-128
    jz X0
    mov [__r0],-1
X0:
    mov REG[0xd0],>__r0
    asl [__r1]
    rlc [__r0]
    mov A,[__r5]
    add [__r1],A
    mov A,[__r4]
    adc [__r0],A
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    mov A,[__r3]
    mvi [__r1],A
    .dbline 101
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 102
L11:
    .dbline 99
;         while (me->state != t) {
    mov REG[0xd0],>__r0
    mov A,[X+1]
    mov [__r1],A
    mov A,[X+0]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov A,[__r2]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L10
    mov A,[__r3]
    cmp A,[__r1]
    jnz L10
X1:
    .dbline 103
;             path[++ip] = me->state;
;             (void)(*me->state)(me);
;         }
;         me->state = path[0];
    mov REG[0xd0],>__r0
    mov A,[X+4]
    mov [__r1],A
    mov A,[X+3]
    mov [__r0],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    mov A,[__r1]
    mvi [__r3],A
    .dbline 105
    mov A,[X+2]
    cmp A,5
    rrc A
    xor A,5
    xor A,[X+2]
    rlc A
    jnc L13
X2:
    .dbline 105
    .dbline 105
    xjmp L14
L13:
    .dbline 105
;                                             /* entry path must not overflow */
;         Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
    mov A,0
    push A
    mov A,105
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L14:
    .dbline 107
;
;         Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,1
    mvi [__r1],A
L15:
    .dbline 108
;         do {        /* retrace the entry path in reverse (correct) order... */
    .dbline 109
;             (void)(*path[ip])(me);                        /* enter path[ip] */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov [__r0],7
    mov [__r1],X
    add [__r1],3
    mov A,[X+2]
    mov [__r3],A
    mov [__r2],0
    tst [__r3],-128
    jz X3
    mov [__r2],-1
X3:
    mov REG[0xd0],>__r0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 110
L16:
    .dbline 110
;         } while ((--ip) >= (int8_t)0);
    mov REG[0xd0],>__r0
    mov A,[X+2]
    sub A,1
    mov [__r0],A
    mov [X+2],A
    tst [__r0],-128
    jz L15
    .dbline 112
;
;         t = path[0];
    mov A,[X+4]
    mov [X+1],A
    mov A,[X+3]
    mov [X+0],A
    .dbline 113
;         Q_SIG(me) = (QSignal)Q_INIT_SIG;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,3
    mvi [__r1],A
    .dbline 114
L8:
    .dbline 114
;     } while ((*t)(me) == Q_RET_TRAN);        /* initial transition handled? */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov A,[X+1]
    mov [__r1],A
    mov A,[X+0]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    cmp A,2
    jz L7
    .dbline 115
;     me->state = t;
    mov A,[X+1]
    mov [__r1],A
    mov A,[X+0]
    mov [__r0],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    mov A,[__r1]
    mvi [__r3],A
    .dbline -2
    .dbline 116
; }
L4:
    add SP,-13
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l path 3 A[10:5]pfc
    .dbsym l ip 2 C
    .dbsym l t 0 pfc
    .dbsym l me -5 pX
    .dbend
    .dbfunc e QHsm_dispatch _QHsm_dispatch fV
;             iq -> X+16
;             ip -> X+15
;              r -> X+14
;              s -> X+12
;              t -> X+10
;           path -> X+0
;             me -> X-5
_QHsm_dispatch::
    .dbline -1
    push X
    mov X,SP
    add SP,17
    .dbline 121
; /*..........................................................................*/
; #ifndef QK_PREEMPTIVE
; void QHsm_dispatch(QHsm *me) {
; #else
; void QHsm_dispatch(QHsm *me) Q_REENTRANT {
    .dbline 128
; #endif
;     QStateHandler path[QEP_MAX_NEST_DEPTH_];
;     QStateHandler s;
;     QStateHandler t;
;     QState r;
;
;     t = me->state;                                /* save the current state */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+10],A
    mvi A,[__r1]
    mov [X+11],A
L19:
    .dbline 130
;
;     do {                             /* process the event hierarchically... */
    .dbline 131
;         s = me->state;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+12],A
    mvi A,[__r1]
    mov [X+13],A
    .dbline 132
;         r = (*s)(me);                             /* invoke state handler s */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    mov [X+14],A
    .dbline 133
L20:
    .dbline 133
;     } while (r == Q_RET_SUPER);
    cmp [X+14],3
    jz L19
    .dbline 135
;
;     if (r == Q_RET_TRAN) {                             /* transition taken? */
    cmp [X+14],2
    jz X22
    ljmp L22
X22:
    .dbline 135
    .dbline 136
;         int8_t ip = (int8_t)(-1);            /* transition entry path index */
    mov [X+15],-1
    .dbline 139
;         int8_t iq;                    /* helper transition entry path index */
;
;         path[0] = me->state;           /* save the target of the transition */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+0],A
    mvi A,[__r1]
    mov [X+1],A
    .dbline 140
;         path[1] = t;
    mov A,[X+11]
    mov [X+3],A
    mov A,[X+10]
    mov [X+2],A
    xjmp L26
L25:
    .dbline 142
;
;         while (t != s) {    /* exit current state to transition source s... */
    .dbline 143
;             Q_SIG(me) = (QSignal)Q_EXIT_SIG;        /* find superstate of t */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,2
    mvi [__r1],A
    .dbline 144
;             if ((*t)(me) == Q_RET_HANDLED) {        /* exit action handled? */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    cmp A,0
    jnz L28
    .dbline 144
    .dbline 145
;                 Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 146
;                 (void)(*t)(me);                     /* find superstate of t */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 147
;             }
L28:
    .dbline 148
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+10],A
    mvi A,[__r1]
    mov [X+11],A
    .dbline 149
L26:
    .dbline 142
    mov REG[0xd0],>__r0
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X+11]
    mov [__r3],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L25
    mov A,[__r3]
    cmp A,[__r1]
    jnz L25
X4:
    .dbline 151
;             t = me->state;                /* me->state holds the superstate */
;         }
;
;         t = path[0];                            /* target of the transition */
    mov A,[X+1]
    mov [X+11],A
    mov A,[X+0]
    mov [X+10],A
    .dbline 153
;
;         if (s == t) {      /* (a) check source==target (transition to self) */
    mov REG[0xd0],>__r0
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X+13]
    mov [__r3],A
    mov A,[X+12]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L30
    mov A,[__r3]
    cmp A,[__r1]
    jnz L30
X5:
    .dbline 153
    .dbline 154
;             Q_SIG(me) = (QSignal)Q_EXIT_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,2
    mvi [__r1],A
    .dbline 155
;             (void)(*s)(me);                              /* exit the source */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 156
;             ip = (int8_t)0;                             /* enter the target */
    mov [X+15],0
    .dbline 157
;         }
    xjmp L31
L30:
    .dbline 158
;         else {
    .dbline 159
;             Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 160
;             (void)(*t)(me);                    /* find superstate of target */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 161
;             t = me->state;
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+10],A
    mvi A,[__r1]
    mov [X+11],A
    .dbline 162
;             if (s == t) {                /* (b) check source==target->super */
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X+13]
    mov [__r3],A
    mov A,[X+12]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L32
    mov A,[__r3]
    cmp A,[__r1]
    jnz L32
X6:
    .dbline 162
    .dbline 163
;                 ip = (int8_t)0;                         /* enter the target */
    mov [X+15],0
    .dbline 164
;             }
    xjmp L33
L32:
    .dbline 165
;             else {
    .dbline 166
;                 Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 167
;                 (void)(*s)(me);                /* find superstate of source */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 170
;
;                                   /* (c) check source->super==target->super */
;                 if (me->state == t) {
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov A,[__r2]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L34
    mov A,[__r3]
    cmp A,[__r1]
    jnz L34
X7:
    .dbline 170
    .dbline 171
;                     Q_SIG(me) = (QSignal)Q_EXIT_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,2
    mvi [__r1],A
    .dbline 172
;                     (void)(*s)(me);                      /* exit the source */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 173
;                     ip = (int8_t)0;                     /* enter the target */
    mov [X+15],0
    .dbline 174
;                 }
    xjmp L35
L34:
    .dbline 175
;                 else {
    .dbline 177
;                                          /* (d) check source->super==target */
;                     if (me->state == path[0]) {
    mov REG[0xd0],>__r0
    mov A,[X+1]
    mov [__r1],A
    mov A,[X+0]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov A,[__r2]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L36
    mov A,[__r3]
    cmp A,[__r1]
    jnz L36
X8:
    .dbline 177
    .dbline 178
;                         Q_SIG(me) = (QSignal)Q_EXIT_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,2
    mvi [__r1],A
    .dbline 179
;                         (void)(*s)(me);                  /* exit the source */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 180
;                     }
    xjmp L37
L36:
    .dbline 181
;                     else { /* (e) check rest of source==target->super->super..
    .dbline 184
;                             * and store the entry path along the way
;                             */
;                         iq = (int8_t)0;      /* indicate that LCA not found */
    mov [X+16],0
    .dbline 185
;                         ip = (int8_t)1;  /* enter target and its superstate */
    mov [X+15],1
    .dbline 186
;                         path[1] = t;       /* save the superstate of target */
    mov A,[X+11]
    mov [X+3],A
    mov A,[X+10]
    mov [X+2],A
    .dbline 187
;                         t = me->state;                /* save source->super */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+10],A
    mvi A,[__r1]
    mov [X+11],A
    .dbline 189
;
;                         Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 190
;                         r = (*path[1])(me);    /* find target->super->super */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+3]
    mov [__r1],A
    mov A,[X+2]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    mov [X+14],A
    xjmp L41
L40:
    .dbline 191
;                         while (r == Q_RET_SUPER) {
    .dbline 192
;                             path[++ip] = me->state; /* store the entry path */
    mov REG[0xd0],>__r0
    mov A,[X+15]
    add A,1
    mov [__r0],A
    mov [X+15],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r4],7
    mov [__r5],X
    mov A,[__r0]
    mov [__r1],A
    mov [__r0],0
    tst [__r1],-128
    jz X9
    mov [__r0],-1
X9:
    mov REG[0xd0],>__r0
    asl [__r1]
    rlc [__r0]
    mov A,[__r5]
    add [__r1],A
    mov A,[__r4]
    adc [__r0],A
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    mov A,[__r3]
    mvi [__r1],A
    .dbline 193
;                             if (me->state == s) {      /* is it the source? */
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov A,[__r2]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L43
    mov A,[__r3]
    cmp A,[__r1]
    jnz L43
X10:
    .dbline 193
    .dbline 194
;                                 iq = (int8_t)1;  /* indicate that LCA found */
    mov [X+16],1
    .dbline 196
    mov A,[X+15]
    cmp A,5
    rrc A
    xor A,5
    xor A,[X+15]
    rlc A
    jnc L45
X11:
    .dbline 196
    .dbline 196
    xjmp L46
L45:
    .dbline 196
;                                             /* entry path must not overflow */
;                                 Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
    mov A,0
    push A
    mov A,-60
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L46:
    .dbline 197
;                                 --ip;            /* do not enter the source */
    dec [X+15]
    .dbline 198
;                                 r = Q_RET_HANDLED;    /* terminate the loop */
    mov [X+14],0
    .dbline 199
;                             }
    xjmp L44
L43:
    .dbline 200
;                             else {   /* it is not the source, keep going up */
    .dbline 201
;                                 r = (*me->state)(me);    /* superstate of t */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    mov [X+14],A
    .dbline 202
;                             }
L44:
    .dbline 203
L41:
    .dbline 191
    cmp [X+14],3
    jz L40
    .dbline 204
;                         }
;                         if (iq == (int8_t)0) {    /* the LCA not found yet? */
    cmp [X+16],0
    jnz L47
    .dbline 204
    .dbline 207
    mov A,[X+15]
    cmp A,5
    rrc A
    xor A,5
    xor A,[X+15]
    rlc A
    jnc L49
X12:
    .dbline 207
    .dbline 207
    xjmp L50
L49:
    .dbline 207
;
;                                             /* entry path must not overflow */
;                             Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
    mov A,0
    push A
    mov A,-49
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L50:
    .dbline 209
;
;                             Q_SIG(me) = (QSignal)Q_EXIT_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,2
    mvi [__r1],A
    .dbline 210
;                             (void)(*s)(me);              /* exit the source */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 215
;
;                                 /* (f) check the rest of source->super
;                                  *                  == target->super->super...
;                                  */
;                             iq = ip;
    mov A,[X+15]
    mov [X+16],A
    .dbline 216
;                             r = Q_RET_IGNORED;    /* indicate LCA NOT found */
    mov [X+14],1
L51:
    .dbline 217
;                             do {
    .dbline 218
;                                 s = path[iq];
    mov REG[0xd0],>__r0
    mov [__r0],7
    mov [__r1],X
    mov A,[X+16]
    mov [__r3],A
    mov [__r2],0
    tst [__r3],-128
    jz X13
    mov [__r2],-1
X13:
    mov REG[0xd0],>__r0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+12],A
    mvi A,[__r1]
    mov [X+13],A
    .dbline 219
;                                 if (t == s) {           /* is this the LCA? */
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X+11]
    mov [__r3],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L54
    mov A,[__r3]
    cmp A,[__r1]
    jnz L54
X14:
    .dbline 219
    .dbline 220
;                                     r = Q_RET_HANDLED;/* indicate LCA found */
    mov [X+14],0
    .dbline 221
;                                     ip = (int8_t)(iq - 1);/*do not enter LCA*/
    mov A,[X+16]
    sub A,1
    mov [X+15],A
    .dbline 222
;                                     iq = (int8_t)(-1);/* terminate the loop */
    mov [X+16],-1
    .dbline 223
;                                 }
    xjmp L55
L54:
    .dbline 224
;                                 else {
    .dbline 225
;                                     --iq; /* try lower superstate of target */
    dec [X+16]
    .dbline 226
;                                 }
L55:
    .dbline 227
L52:
    .dbline 227
;                             } while (iq >= (int8_t)0);
    tst [X+16],-128
    jz L51
    .dbline 229
;
;                             if (r != Q_RET_HANDLED) { /* LCA not found yet? */
    cmp [X+14],0
    jz L56
    .dbline 229
    .dbline 233
;                                     /* (g) check each source->super->...
;                                      * for each target->super...
;                                      */
;                                 r = Q_RET_IGNORED;          /* keep looping */
    mov [X+14],1
L58:
    .dbline 234
;                                 do {
    .dbline 236
;                                                        /* exit t unhandled? */
;                                     Q_SIG(me) = (QSignal)Q_EXIT_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,2
    mvi [__r1],A
    .dbline 237
;                                     if ((*t)(me) == Q_RET_HANDLED) {
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    cmp A,0
    jnz L61
    .dbline 237
    .dbline 238
;                                         Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 239
;                                         (void)(*t)(me);  /* find super of t */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 240
;                                     }
L61:
    .dbline 241
;                                     t = me->state;    /*  set to super of t */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+10],A
    mvi A,[__r1]
    mov [X+11],A
    .dbline 242
;                                     iq = ip;
    mov A,[X+15]
    mov [X+16],A
L63:
    .dbline 243
;                                     do {
    .dbline 244
;                                         s = path[iq];
    mov REG[0xd0],>__r0
    mov [__r0],7
    mov [__r1],X
    mov A,[X+16]
    mov [__r3],A
    mov [__r2],0
    tst [__r3],-128
    jz X15
    mov [__r2],-1
X15:
    mov REG[0xd0],>__r0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+12],A
    mvi A,[__r1]
    mov [X+13],A
    .dbline 245
;                                         if (t == s) {       /* is this LCA? */
    mov A,[X+13]
    mov [__r1],A
    mov A,[X+12]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X+11]
    mov [__r3],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L66
    mov A,[__r3]
    cmp A,[__r1]
    jnz L66
X16:
    .dbline 245
    .dbline 247
;                                                         /* do not enter LCA */
;                                             ip = (int8_t)(iq - 1);
    mov A,[X+16]
    sub A,1
    mov [X+15],A
    .dbline 248
;                                             iq = (int8_t)(-1);/*break inner */
    mov [X+16],-1
    .dbline 249
;                                             r = Q_RET_HANDLED;/*break outer */
    mov [X+14],0
    .dbline 250
;                                         }
    xjmp L67
L66:
    .dbline 251
;                                         else {
    .dbline 252
;                                             --iq;
    dec [X+16]
    .dbline 253
;                                         }
L67:
    .dbline 254
L64:
    .dbline 254
;                                     } while (iq >= (int8_t)0);
    tst [X+16],-128
    jz L63
    .dbline 255
L59:
    .dbline 255
;                                 } while (r != Q_RET_HANDLED);
    cmp [X+14],0
    jnz L58
    .dbline 256
;                             }
L56:
    .dbline 257
;                         }
L47:
    .dbline 258
;                     }
L37:
    .dbline 259
;                 }
L35:
    .dbline 260
;             }
L33:
    .dbline 261
;         }
L31:
    .dbline 263
;                     /* retrace the entry path in reverse (desired) order... */
;         Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,1
    mvi [__r1],A
    .dbline 264
    xjmp L71
L68:
    .dbline 264
    .dbline 265
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov [__r0],7
    mov [__r1],X
    mov A,[X+15]
    mov [__r3],A
    mov [__r2],0
    tst [__r3],-128
    jz X17
    mov [__r2],-1
X17:
    mov REG[0xd0],>__r0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 266
L69:
    .dbline 264
    dec [X+15]
L71:
    .dbline 264
;         for (; ip >= (int8_t)0; --ip) {
    tst [X+15],-128
    jz L68
    .dbline 267
;             (void)(*path[ip])(me);                        /* enter path[ip] */
;         }
;         t = path[0];                      /* stick the target into register */
    mov A,[X+1]
    mov [X+11],A
    mov A,[X+0]
    mov [X+10],A
    .dbline 268
;         me->state = t;                          /* update the current state */
    mov REG[0xd0],>__r0
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov [__r0],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    mov A,[__r1]
    mvi [__r3],A
    .dbline 271
;
;                                       /* drill into the target hierarchy... */
;         Q_SIG(me) = (QSignal)Q_INIT_SIG;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,3
    mvi [__r1],A
    xjmp L73
L72:
    .dbline 272
;         while ((*t)(me) == Q_RET_TRAN) {
    .dbline 273
;             ip = (int8_t)0;
    mov [X+15],0
    .dbline 275
;
;             path[0] = me->state;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [X+0],A
    mvi A,[__r1]
    mov [X+1],A
    .dbline 276
;             Q_SIG(me) = (QSignal)QEP_EMPTY_SIG_;
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    .dbline 277
;             (void)(*me->state)(me);                  /* find the superstate */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    xjmp L76
L75:
    .dbline 278
    .dbline 279
    mov REG[0xd0],>__r0
    mov A,[X+15]
    add A,1
    mov [__r0],A
    mov [X+15],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r4],7
    mov [__r5],X
    mov A,[__r0]
    mov [__r1],A
    mov [__r0],0
    tst [__r1],-128
    jz X18
    mov [__r0],-1
X18:
    mov REG[0xd0],>__r0
    asl [__r1]
    rlc [__r0]
    mov A,[__r5]
    add [__r1],A
    mov A,[__r4]
    adc [__r0],A
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    mov A,[__r3]
    mvi [__r1],A
    .dbline 280
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov A,[X-4]
    mov [__r1],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 281
L76:
    .dbline 278
;             while (me->state != t) {
    mov REG[0xd0],>__r0
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    push X
    push A
    mov X,[__r1]
    romx
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mvi A,[__r3]
    mov [__r3],A
    mov A,[__r2]
    push X
    push A
    mov X,[__r3]
    romx
    mov [__r2],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r3],A
    pop X
    mov A,[__r2]
    cmp A,[__r0]
    jnz L75
    mov A,[__r3]
    cmp A,[__r1]
    jnz L75
X19:
    .dbline 282
;                 path[++ip] = me->state;
;                 (void)(*me->state)(me);              /* find the superstate */
;             }
;             me->state = path[0];
    mov REG[0xd0],>__r0
    mov A,[X+1]
    mov [__r1],A
    mov A,[X+0]
    mov [__r0],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    mov A,[__r1]
    mvi [__r3],A
    .dbline 284
    mov A,[X+15]
    cmp A,5
    rrc A
    xor A,5
    xor A,[X+15]
    rlc A
    jnc L78
X20:
    .dbline 284
    .dbline 284
    xjmp L79
L78:
    .dbline 284
;                                             /* entry path must not overflow */
;             Q_ASSERT(ip < (int8_t)QEP_MAX_NEST_DEPTH_);
    mov A,1
    push A
    mov A,28
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L79:
    .dbline 286
;
;             Q_SIG(me) = (QSignal)Q_ENTRY_SIG;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,1
    mvi [__r1],A
L80:
    .dbline 287
;             do {    /* retrace the entry path in reverse (correct) order... */
    .dbline 288
;                 (void)(*path[ip])(me);                    /* enter path[ip] */
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov [__r0],7
    mov [__r1],X
    mov A,[X+15]
    mov [__r3],A
    mov [__r2],0
    tst [__r3],-128
    jz X21
    mov [__r2],-1
X21:
    mov REG[0xd0],>__r0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    mov X,A
    mov A,[__r0]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    .dbline 289
L81:
    .dbline 289
    mov REG[0xd0],>__r0
    mov A,[X+15]
    sub A,1
    mov [__r0],A
    mov [X+15],A
    tst [__r0],-128
    jz L80
    .dbline 291
    mov A,[X+1]
    mov [X+11],A
    mov A,[X+0]
    mov [X+10],A
    .dbline 292
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,3
    mvi [__r1],A
    .dbline 293
L73:
    .dbline 272
    push X
    mov A,[X-5]
    push A
    mov A,[X-4]
    push A
    mov REG[0xd0],>__r0
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov X,[__r1]
    xcall __plcall
    add SP,-2
    mov REG[0xd0],>__r0
    pop X
    cmp A,2
    jz L72
    .dbline 294
;             } while ((--ip) >= (int8_t)0);
;
;             t = path[0];
;             Q_SIG(me) = (QSignal)Q_INIT_SIG;
;         }
;     }
L22:
    .dbline 295
;     me->state = t;            /* set new state or restore the current state */
    mov REG[0xd0],>__r0
    mov A,[X+11]
    mov [__r1],A
    mov A,[X+10]
    mov [__r0],A
    mov A,[X-4]
    mov [__r3],A
    mov A,[X-5]
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    mov A,[__r1]
    mvi [__r3],A
    .dbline -2
    .dbline 296
; }
L18:
    add SP,-17
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l iq 16 C
    .dbsym l ip 15 C
    .dbsym l r 14 c
    .dbsym l s 12 pfc
    .dbsym l t 10 pfc
    .dbsym l path 0 A[10:5]pfc
    .dbsym l me -5 pX
    .dbend
    .area func_lit
PL_QHsm_top:    .word _QHsm_top

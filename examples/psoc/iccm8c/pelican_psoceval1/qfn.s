    cpu LMM
    .module qfn.c
    .area lit(rom, con, rel)
_l_this_file:
    .byte 'q,'f,'n,0
    .dbfile ./qfn.c
    .dbsym s l_this_file _l_this_file A[4:4]c
_l_pow2Lkup:
    .byte 0,1
    .byte 2,4
    .byte 8,16
    .byte 32,64
    .byte 128
    .dbsym s l_pow2Lkup _l_pow2Lkup A[9:9]c
    .area text(rom, con, rel)
    .dbfile ./qfn.c
    .dbfunc e QActive_post _QActive_post fV
;            par -> X-7
;            sig -> X-6
;             me -> X-5
_QActive_post::
    .dbline -1
    push X
    mov X,SP
    .dbline 48
; /*****************************************************************************
; * Product: QF-nano implemenation
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
; Q_DEFINE_THIS_MODULE(qfn)
;
; /**
; * \file
; * \ingroup qfn
; * QF-nano implementation.
; */
;
; /* Global-scope objects ----------------------------------------------------*/
; uint8_t volatile QF_readySet_;                      /* ready-set of QF-nano */
;
; /* local objects -----------------------------------------------------------*/
; static uint8_t const Q_ROM Q_ROM_VAR l_pow2Lkup[] = {
;     0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
; };
;
; /*..........................................................................*/
; #if (Q_PARAM_SIZE != 0)
; void QActive_post(QActive *me, QSignal sig, QParam par) {
    .dbline 52
; #else
; void QActive_post(QActive *me, QSignal sig) {
; #endif
;     QF_INT_LOCK();
        and F, FEh

    .dbline 53
;     if (me->nUsed == (uint8_t)0) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,7
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L2
    .dbline 53
    .dbline 54
;         ++me->nUsed;                             /* update number of events */
    mov A,[X-4]
    add A,7
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    dec [__r1]
    mov [__r2],A
    add [__r2],1
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    .dbline 56
;
;         Q_SIG(me) = sig;                      /* deliver the event directly */
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[X-6]
    mvi [__r1],A
    .dbline 58
; #if (Q_PARAM_SIZE != 0)
;         Q_PAR(me) = par;
    mov A,[X-4]
    add A,3
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[X-7]
    mvi [__r1],A
    .dbline 60
; #endif
;         QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);    /* set the bit */
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov [__r0],0
    add [__r1],<_l_pow2Lkup
    adc [__r0],>_l_pow2Lkup
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov REG[0xd0],>_QF_readySet_
    mov A,[_QF_readySet_]
    mov REG[0xd0],>__r0
    or A,[__r0]
    mov REG[0xd0],>_QF_readySet_
    mov [_QF_readySet_],A
    .dbline 65
;
; #ifdef QK_PREEMPTIVE
;         QK_schedule_();                 /* check for synchronous preemption */
; #endif
;     }
    xjmp L3
L2:
    .dbline 66
;     else {
    .dbline 68
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+4
    adc [__r0],>_QF_active+4
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov A,[X-4]
    add A,7
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mov A,[__r0]
    cmp A,[__r2]
    jc L4
X0:
    .dbline 68
    .dbline 68
    xjmp L5
L4:
    .dbline 68
;             /* the queue must be able to accept the event (cannot overflow) */
;         Q_ASSERT(me->nUsed <= Q_ROM_BYTE(QF_active[me->prio].end));
    mov A,0
    push A
    mov A,68
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L5:
    .dbline 69
;         ++me->nUsed;                             /* update number of events */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,7
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    dec [__r1]
    mov [__r2],A
    add [__r2],1
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    .dbline 71
;                                 /* insert event into the ring buffer (FIFO) */
;         ((QEvent *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].sig = sig;
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+2
    adc [__r0],>_QF_active+2
    mov A,[__r0]
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
    add A,5
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r2],0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd5],A
    mov A,[X-6]
    mvi [__r1],A
    .dbline 73
; #if (Q_PARAM_SIZE != 0)
;         ((QEvent *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].par = par;
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+2
    adc [__r0],>_QF_active+2
    mov A,[__r0]
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
    add A,5
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r2],0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov [__r0],A
    add [__r1],1
    adc [__r0],0
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[X-7]
    mvi [__r1],A
    .dbline 75
; #endif
;         if (me->head == (uint8_t)0) {
    mov A,[X-4]
    add A,5
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L9
    .dbline 75
    .dbline 76
;             me->head = Q_ROM_BYTE(QF_active[me->prio].end);/* wrap the head */
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+4
    adc [__r0],>_QF_active+4
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov A,[X-4]
    add A,5
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 77
;         }
L9:
    .dbline 78
;         --me->head;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,5
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
    .dbline 79
;     }
L3:
    .dbline 80
;     QF_INT_UNLOCK();
        or  F, 01h

    .dbline -2
    .dbline 81
; }
L1:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l par -7 c
    .dbsym l sig -6 c
    .dbsym l me -5 pX
    .dbend
    .dbfunc e QActive_postISR _QActive_postISR fV
;            par -> X-7
;            sig -> X-6
;             me -> X-5
_QActive_postISR::
    .dbline -1
    push X
    mov X,SP
    .dbline 88
; /*..........................................................................*/
; #if (Q_PARAM_SIZE != 0)
; void QActive_postISR(QActive *me, QSignal sig, QParam par)
; #else
; void QActive_postISR(QActive *me, QSignal sig)
; #endif
; {
    .dbline 97
; #ifdef QF_ISR_NEST
; #ifdef QF_ISR_KEY_TYPE
;     QF_ISR_KEY_TYPE key;
;     QF_ISR_LOCK(key);
; #else
;     QF_INT_LOCK();
; #endif
; #endif
;     if (me->nUsed == (uint8_t)0) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,7
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L13
    .dbline 97
    .dbline 98
;         ++me->nUsed;                             /* update number of events */
    mov A,[X-4]
    add A,7
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    dec [__r1]
    mov [__r2],A
    add [__r2],1
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    .dbline 100
;
;         Q_SIG(me) = sig;                      /* deliver the event directly */
    mov A,[X-4]
    add A,2
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[X-6]
    mvi [__r1],A
    .dbline 102
; #if (Q_PARAM_SIZE != 0)
;         Q_PAR(me) = par;
    mov A,[X-4]
    add A,3
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[X-7]
    mvi [__r1],A
    .dbline 104
; #endif
;         QF_readySet_ |= Q_ROM_BYTE(l_pow2Lkup[me->prio]);    /* set the bit */
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov [__r0],0
    add [__r1],<_l_pow2Lkup
    adc [__r0],>_l_pow2Lkup
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov REG[0xd0],>_QF_readySet_
    mov A,[_QF_readySet_]
    mov REG[0xd0],>__r0
    or A,[__r0]
    mov REG[0xd0],>_QF_readySet_
    mov [_QF_readySet_],A
    .dbline 105
;     }
    xjmp L14
L13:
    .dbline 106
;     else {
    .dbline 108
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+4
    adc [__r0],>_QF_active+4
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov A,[X-4]
    add A,7
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r2],A
    mov A,[__r0]
    cmp A,[__r2]
    jc L15
X1:
    .dbline 108
    .dbline 108
    xjmp L16
L15:
    .dbline 108
;             /* the queue must be able to accept the event (cannot overflow) */
;         Q_ASSERT(me->nUsed <= Q_ROM_BYTE(QF_active[me->prio].end));
    mov A,0
    push A
    mov A,108
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L16:
    .dbline 109
;         ++me->nUsed;                             /* update number of events */
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,7
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    dec [__r1]
    mov [__r2],A
    add [__r2],1
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    .dbline 111
;                                 /* insert event into the ring buffer (FIFO) */
;         ((QEvent *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].sig = sig;
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+2
    adc [__r0],>_QF_active+2
    mov A,[__r0]
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
    add A,5
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r2],0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov REG[0xd5],A
    mov A,[X-6]
    mvi [__r1],A
    .dbline 113
; #if (Q_PARAM_SIZE != 0)
;         ((QEvent *)Q_ROM_PTR(QF_active[me->prio].queue))[me->head].par = par;
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+2
    adc [__r0],>_QF_active+2
    mov A,[__r0]
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
    add A,5
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r2],0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov [__r0],A
    add [__r1],1
    adc [__r0],0
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[X-7]
    mvi [__r1],A
    .dbline 115
; #endif
;         if (me->head == (uint8_t)0) {
    mov A,[X-4]
    add A,5
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L20
    .dbline 115
    .dbline 116
;             me->head = Q_ROM_BYTE(QF_active[me->prio].end);/* wrap the head */
    mov A,[X-4]
    add A,4
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+4
    adc [__r0],>_QF_active+4
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov A,[X-4]
    add A,5
    mov [__r3],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 117
;         }
L20:
    .dbline 118
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,5
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
    .dbline 119
L14:
    .dbline -2
    .dbline 128
;         --me->head;
;     }
;
; #ifdef QF_ISR_NEST
; #ifdef QF_ISR_KEY_TYPE
;     QF_ISR_UNLOCK(key);
; #else
;     QF_INT_UNLOCK();
; #endif
; #endif
; }
L12:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l par -7 c
    .dbsym l sig -6 c
    .dbsym l me -5 pX
    .dbend
    .area data(ram, con, rel)
    .dbfile ./qfn.c
L24:
    .byte 0
    .area data(ram, con, rel)
    .dbfile ./qfn.c
L28:
    .byte 0,0
    .area text(rom, con, rel)
    .dbfile ./qfn.c
    .dbfunc e QF_tick _QF_tick fV
    .dbsym s a L28 pX
    .dbsym s p L24 c
_QF_tick::
    .dbline -1
    .dbline 134
;
; /*--------------------------------------------------------------------------*/
; #if (QF_TIMEEVT_CTR_SIZE != 0)
;
; /*..........................................................................*/
; void QF_tick(void) {
    .dbline 136
;     static uint8_t p;                /* declared static to save stack space */
;     p = (uint8_t)QF_MAX_ACTIVE;
    mov REG[0xd0],>L24
    mov [L24],2
L25:
    .dbline 137
;     do {
    .dbline 139
;         static QActive *a;           /* declared static to save stack space */
;         a = (QActive *)Q_ROM_PTR(QF_active[p].act);
    mov REG[0xd0],>L24
    mov A,[L24]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active
    adc [__r0],>_QF_active
    mov A,[__r0]
    push X
    push A
    mov X,[__r1]
    romx
    mov REG[0xd0],>L28
    mov [L28],A
    pop A
    inc X
    adc A,0
    romx
    mov [L28+1],A
    pop X
    .dbline 140
;         if (a->tickCtr != (QTimeEvtCtr)0) {
    mov A,[L28+1]
    add A,8
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L28
    mov A,[L28]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mvi A,[__r1]
    cmp [__r0],0
    jnz X2
    cmp A,0
    jz L29
X2:
    .dbline 140
    .dbline 141
;             if ((--a->tickCtr) == (QTimeEvtCtr)0) {
    mov REG[0xd0],>L28
    mov A,[L28+1]
    add A,8
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L28
    mov A,[L28]
    adc A,0
    mov REG[0xd0],>__r0
    mov [__r0],A
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r2],A
    mvi A,[__r1]
    sub [__r1],2
    mov [__r3],A
    sub [__r3],1
    sbb [__r2],0
    mov A,[__r0]
    mov REG[0xd5],A
    mov A,[__r2]
    mvi [__r1],A
    mov A,[__r3]
    mvi [__r1],A
    cmp [__r2],0
    jnz L31
    cmp [__r3],0
    jnz L31
X3:
    .dbline 141
    .dbline 143
; #if (Q_PARAM_SIZE != 0)
;                 QActive_postISR(a, (QSignal)Q_TIMEOUT_SIG, (QParam)0);
    mov A,0
    push A
    mov A,4
    push A
    mov REG[0xd0],>L28
    mov A,[L28]
    push A
    mov A,[L28+1]
    push A
    xcall _QActive_postISR
    add SP,-4
    .dbline 147
; #else
;                 QActive_postISR(a, (QSignal)Q_TIMEOUT_SIG);
; #endif
;             }
L31:
    .dbline 148
;         }
L29:
    .dbline 149
L26:
    .dbline 149
;     } while ((--p) != (uint8_t)0);
    mov REG[0xd0],>L24
    mov A,[L24]
    sub A,1
    mov [L24],A
    mov REG[0xd0],>__r0
    cmp A,0
    jnz L25
    .dbline -2
    .dbline 150
; }
L23:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e QActive_arm _QActive_arm fV
;           tout -> X-7
;             me -> X-5
_QActive_arm::
    .dbline -1
    push X
    mov X,SP
    .dbline 154
;
; #if (QF_TIMEEVT_CTR_SIZE > 1)
; /*..........................................................................*/
; void QActive_arm(QActive *me, QTimeEvtCtr tout) {
    .dbline 155
;     QF_INT_LOCK();
        and F, FEh

    .dbline 156
;     me->tickCtr = tout;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,8
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,[X-7]
    mvi [__r1],A
    mov A,[X-6]
    mvi [__r1],A
    .dbline 157
;     QF_INT_UNLOCK();
        or  F, 01h

    .dbline -2
    .dbline 158
; }
L33:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l tout -7 i
    .dbsym l me -5 pX
    .dbend
    .dbfunc e QActive_disarm _QActive_disarm fV
;             me -> X-5
_QActive_disarm::
    .dbline -1
    push X
    mov X,SP
    .dbline 160
; /*..........................................................................*/
; void QActive_disarm(QActive *me) {
    .dbline 161
;     QF_INT_LOCK();
        and F, FEh

    .dbline 162
;     me->tickCtr = (QTimeEvtCtr)0;
    mov REG[0xd0],>__r0
    mov A,[X-4]
    add A,8
    mov [__r1],A
    mov A,[X-5]
    adc A,0
    mov REG[0xd5],A
    mov A,0
    mvi [__r1],A
    mvi [__r1],A
    .dbline 163
;     QF_INT_UNLOCK();
        or  F, 01h

    .dbline -2
    .dbline 164
; }
L34:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l me -5 pX
    .dbend
    .area lit(rom, con, rel)
L36:
    .byte 0,1
    .byte 2,2
    .byte 3,3
    .byte 3,3
    .byte 4,4
    .byte 4,4
    .byte 4,4
    .byte 4,4
L37:
    .byte 255,254
    .byte 253,251
    .byte 247,239
    .byte 223,191
    .byte 127
    .area data(ram, con, rel)
    .dbfile ./qfn.c
L38:
    .byte 0,0
    .area data(ram, con, rel)
    .dbfile ./qfn.c
L39:
    .byte 0
    .area text(rom, con, rel)
    .dbfile ./qfn.c
    .dbfunc e QF_run _QF_run fV
    .dbsym s p L39 c
    .dbsym s a L38 pX
    .dbsym s invPow2Lkup L37 A[9:9]c
    .dbsym s log2Lkup L36 A[16:16]c
_QF_run::
    .dbline -1
    .dbline 172
; #endif                                     /* #if (QF_TIMEEVT_CTR_SIZE > 1) */
;
; #endif                                    /* #if (QF_TIMEEVT_CTR_SIZE != 0) */
;
; /*--------------------------------------------------------------------------*/
; #ifndef QK_PREEMPTIVE
;
; void QF_run(void) {
    .dbline 183
    mov REG[0xd0],>L39
    mov [L39],1
    xjmp L43
L40:
    .dbline 183
;     static uint8_t const Q_ROM Q_ROM_VAR log2Lkup[] = {
;         0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4
;     };
;     static uint8_t const Q_ROM Q_ROM_VAR invPow2Lkup[] = {
;         0xFF, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
;     };
;     static QActive *a;               /* declared static to save stack space */
;     static uint8_t p;                /* declared static to save stack space */
;
;                          /* set priorities all registered active objects... */
;     for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
    .dbline 184
;         a = (QActive *)Q_ROM_PTR(QF_active[p].act);
    mov REG[0xd0],>L39
    mov A,[L39]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active
    adc [__r0],>_QF_active
    mov A,[__r0]
    push X
    push A
    mov X,[__r1]
    romx
    mov REG[0xd0],>L38
    mov [L38],A
    pop A
    inc X
    adc A,0
    romx
    mov [L38+1],A
    pop X
    .dbline 185
    cmp [L38],0
    jnz X5
    cmp [L38+1],0
    jz L44
X5:
    .dbline 185
    .dbline 185
    xjmp L45
L44:
    .dbline 185
;         Q_ASSERT(a != (QActive *)0);    /* QF_active[p] must be initialized */
    mov A,0
    push A
    mov A,-71
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L45:
    .dbline 186
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,4
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd5],A
    mov REG[0xd0],>L39
    mov A,[L39]
    mov REG[0xd0],>__r0
    mvi [__r1],A
    .dbline 187
L41:
    .dbline 183
    mov REG[0xd0],>L39
    inc [L39]
L43:
    .dbline 183
    mov REG[0xd0],>L39
    mov A,2
    cmp A,[L39]
    jnc L40
X6:
    .dbline 189
    mov REG[0xd0],>L39
    mov [L39],1
    xjmp L49
L46:
    .dbline 189
    .dbline 190
    mov REG[0xd0],>L39
    mov A,[L39]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active
    adc [__r0],>_QF_active
    mov A,[__r0]
    push X
    push A
    mov X,[__r1]
    romx
    mov REG[0xd0],>L38
    mov [L38],A
    pop A
    inc X
    adc A,0
    romx
    mov [L38+1],A
    pop X
    .dbline 192
    mov A,[L38]
    push A
    mov A,[L38+1]
    push A
    xcall _QHsm_init
    add SP,-2
    .dbline 196
L47:
    .dbline 189
    mov REG[0xd0],>L39
    inc [L39]
L49:
    .dbline 189
;         a->prio = p;               /* set the priority of the active object */
;     }
;          /* trigger initial transitions in all registered active objects... */
;     for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
    mov REG[0xd0],>L39
    mov A,2
    cmp A,[L39]
    jnc L46
X7:
    .dbline 198
;         a = (QActive *)Q_ROM_PTR(QF_active[p].act);
; #ifndef QF_FSM_ACTIVE
;         QHsm_init((QHsm *)a);         /* take the initial transition in HSM */
; #else
;         QFsm_init((QFsm *)a);         /* take the initial transition in FSM */
; #endif
;     }
;
;     QF_onStartup();                              /* invoke startup callback */
    xcall _QF_onStartup
    .dbline 200
L50:
    .dbline 200
;
;     for (;;) {                      /* the event loop of the vanilla kernel */
    .dbline 201
;         QF_INT_LOCK();
        and F, FEh

    .dbline 202
;         if (QF_readySet_ != (uint8_t)0) {
    mov REG[0xd0],>_QF_readySet_
    cmp [_QF_readySet_],0
    jz L54
    .dbline 202
    .dbline 209
; #if (QF_MAX_ACTIVE > 4)
;             if ((QF_readySet_ & 0xF0) != 0U) {        /* upper nibble used? */
;                 p = (uint8_t)(Q_ROM_BYTE(log2Lkup[QF_readySet_ >> 4]) + 4);
;             }
;             else                    /* upper nibble of QF_readySet_ is zero */
; #endif
;             {
    .dbline 210
;                 p = Q_ROM_BYTE(log2Lkup[QF_readySet_]);
    mov A,[_QF_readySet_]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov [__r0],0
    add [__r1],<L36
    adc [__r0],>L36
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov REG[0xd0],>L39
    mov [L39],A
    .dbline 211
;             }
    .dbline 212
;             QF_INT_UNLOCK();
        or  F, 01h

    .dbline 214
;
;             a = (QActive *)Q_ROM_PTR(QF_active[p].act);
    mov A,[L39]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active
    adc [__r0],>_QF_active
    mov A,[__r0]
    push X
    push A
    mov X,[__r1]
    romx
    mov REG[0xd0],>L38
    mov [L38],A
    pop A
    inc X
    adc A,0
    romx
    mov [L38+1],A
    pop X
    .dbline 217
;
; #ifndef QF_FSM_ACTIVE
;             QHsm_dispatch((QHsm *)a);                    /* dispatch to HSM */
    mov A,[L38]
    push A
    mov A,[L38+1]
    push A
    xcall _QHsm_dispatch
    add SP,-2
    .dbline 222
; #else
;             QFsm_dispatch((QFsm *)a);                    /* dispatch to FSM */
; #endif
;
;             QF_INT_LOCK();
        and F, FEh

    .dbline 223
;             if ((--a->nUsed) == (uint8_t)0) {      /* queue becoming empty? */
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,7
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
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
    cmp [__r2],0
    jnz L56
    .dbline 223
    .dbline 224
;                 QF_readySet_ &= Q_ROM_BYTE(invPow2Lkup[p]);/* clear the bit */
    mov REG[0xd0],>L39
    mov A,[L39]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov [__r0],0
    add [__r1],<L37
    adc [__r0],>L37
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov REG[0xd0],>_QF_readySet_
    and [_QF_readySet_],A
    .dbline 225
;             }
    xjmp L57
L56:
    .dbline 226
;             else {
    .dbline 227
;                 Q_SIG(a) = ((QEvent *)Q_ROM_PTR(QF_active[a->prio].queue))
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,4
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+2
    adc [__r0],>_QF_active+2
    mov A,[__r0]
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
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r2],0
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
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,2
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 230
;                                 [a->tail].sig;
; #if (Q_PARAM_SIZE != 0)
;                 Q_PAR(a) = ((QEvent *)Q_ROM_PTR(QF_active[a->prio].queue))
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,4
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+2
    adc [__r0],>_QF_active+2
    mov A,[__r0]
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
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r3]
    mov [__r3],A
    mov [__r2],0
    asl [__r3]
    rlc [__r2]
    mov A,[__r3]
    add A,[__r1]
    mov [__r1],A
    mov A,[__r2]
    adc A,[__r0]
    mov [__r0],A
    add [__r1],1
    adc [__r0],0
    mov A,[__r0]
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r0],A
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,3
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 233
;                                 [a->tail].par;
; #endif
;                 if (a->tail == (uint8_t)0) {                /* wrap around? */
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L60
    .dbline 233
    .dbline 234
;                     a->tail = Q_ROM_BYTE(QF_active[a->prio].end);
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,4
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r1]
    mov [__r1],A
    mov A,0
    push A
    mov A,[__r1]
    push A
    mov A,0
    push A
    mov A,5
    push A
    xcall __mul16
    add SP,-4
    mov A,[__rX]
    mov [__r1],A
    mov A,[__rY]
    mov [__r0],A
    add [__r1],<_QF_active+4
    adc [__r0],>_QF_active+4
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [__r0],A
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 235
;                 }
L60:
    .dbline 236
;                 --a->tail;
    mov REG[0xd0],>L38
    mov A,[L38+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>L38
    mov A,[L38]
    adc A,0
    mov REG[0xd0],>__r0
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
    .dbline 237
;             }
L57:
    .dbline 238
;             QF_INT_UNLOCK();
        or  F, 01h

    .dbline 239
;         }
    xjmp L50
L54:
    .dbline 240
    .dbline 241
    xcall _QF_onIdle
    .dbline 242
    .dbline 243
    .dbline 200
    .dbline 200
    xjmp L50
X4:
    .dbline -2
L35:
    .dbline 0 ; func end
    ret
    .dbend
    .area data(ram, con, rel)
    .dbfile ./qfn.c
_QF_readySet_::
    .byte 0
    .dbsym e QF_readySet_ _QF_readySet_ X

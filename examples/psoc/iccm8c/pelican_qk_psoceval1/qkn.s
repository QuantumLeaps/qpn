    cpu LMM
    .module qkn.c
    .area lit(rom, con, rel)
_l_this_file:
    .byte 'q,'k,'n,0
    .dbfile ./qkn.c
    .dbsym s l_this_file _l_this_file A[4:4]c
    .area data(ram, con, rel)
    .dbfile ./qkn.c
_QK_currPrio_::
    .byte 3
    .dbsym e QK_currPrio_ _QK_currPrio_ X
    .area data(ram, con, rel)
    .dbfile ./qkn.c
    .area data(ram, con, rel)
    .dbfile ./qkn.c
L2:
    .byte 0
    .area text(rom, con, rel)
    .dbfile ./qkn.c
    .dbfunc e QF_run _QF_run fV
    .dbsym s p L2 c
_QF_run::
    .dbline -1
    .dbline 54
; /*****************************************************************************
; * Product: QK-nano implemenation
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
; Q_DEFINE_THIS_MODULE(qkn)
;
; /**
; * \file
; * \ingroup qkn
; * QK-nano implementation.
; */
;
; #ifndef QK_PREEMPTIVE
;     #error "The required header file qkn.h is not included in qpn_port.h"
; #endif
;
; /* Global-scope objects ----------------------------------------------------*/
;                                       /* start with the QK scheduler locked */
; uint8_t volatile QK_currPrio_ = (uint8_t)(QF_MAX_ACTIVE + 1);
;
; #ifdef QF_ISR_NEST
; uint8_t volatile QK_intNest_;              /* start with nesting level of 0 */
; #endif
;
; /* local objects -----------------------------------------------------------*/
; static QActive *l_act;                                         /* ptr to AO */
;
; /*..........................................................................*/
; void QF_run(void) {
    .dbline 58
    mov REG[0xd0],>L2
    mov [L2],1
    xjmp L6
L3:
    .dbline 58
;     static uint8_t p;                /* declared static to save stack space */
;
;                          /* set priorities all registered active objects... */
;     for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
    .dbline 59
;         l_act = (QActive *)Q_ROM_PTR(QF_active[p].act);
    mov REG[0xd0],>L2
    mov A,[L2]
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
    mov REG[0xd0],>_l_act
    mov [_l_act],A
    pop A
    inc X
    adc A,0
    romx
    mov [_l_act+1],A
    pop X
    .dbline 60
    cmp [_l_act],0
    jnz X1
    cmp [_l_act+1],0
    jz L7
X1:
    .dbline 60
    .dbline 60
    xjmp L8
L7:
    .dbline 60
;         Q_ASSERT(l_act != (QActive *)0);/* QF_active[p] must be initialized */
    mov A,0
    push A
    mov A,60
    push A
    mov A,>_l_this_file
    push A
    mov A,<_l_this_file
    push A
    xcall _Q_onAssert
    add SP,-4
L8:
    .dbline 61
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,4
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
    adc A,0
    mov REG[0xd5],A
    mov REG[0xd0],>L2
    mov A,[L2]
    mov REG[0xd0],>__r0
    mvi [__r1],A
    .dbline 62
L4:
    .dbline 58
    mov REG[0xd0],>L2
    inc [L2]
L6:
    .dbline 58
    mov REG[0xd0],>L2
    mov A,2
    cmp A,[L2]
    jnc L3
X2:
    .dbline 64
    mov REG[0xd0],>L2
    mov [L2],1
    xjmp L12
L9:
    .dbline 64
    .dbline 65
    mov REG[0xd0],>L2
    mov A,[L2]
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
    mov REG[0xd0],>_l_act
    mov [_l_act],A
    pop A
    inc X
    adc A,0
    romx
    mov [_l_act+1],A
    pop X
    .dbline 67
    mov A,[_l_act]
    push A
    mov A,[_l_act+1]
    push A
    xcall _QHsm_init
    add SP,-2
    .dbline 71
L10:
    .dbline 64
    mov REG[0xd0],>L2
    inc [L2]
L12:
    .dbline 64
;         l_act->prio = p;           /* set the priority of the active object */
;     }
;          /* trigger initial transitions in all registered active objects... */
;     for (p = (uint8_t)1; p <= (uint8_t)QF_MAX_ACTIVE; ++p) {
    mov REG[0xd0],>L2
    mov A,2
    cmp A,[L2]
    jnc L9
X3:
    .dbline 73
;         l_act = (QActive *)Q_ROM_PTR(QF_active[p].act);
; #ifndef QF_FSM_ACTIVE
;         QHsm_init((QHsm *)l_act);                          /* initial tran. */
; #else
;         QFsm_init((QFsm *)l_act);                          /* initial tran. */
; #endif
;     }
;
;     QF_INT_LOCK();
        and F, FEh

    .dbline 74
;     QK_currPrio_ = (uint8_t)0;     /* set the priority for the QK idle loop */
    mov REG[0xd0],>_QK_currPrio_
    mov [_QK_currPrio_],0
    .dbline 75
    mov REG[0xd0],>_QF_readySet_
    cmp [_QF_readySet_],0
    jz L13
    .dbline 75
    .dbline 75
    xcall _QK_schedule_
    .dbline 75
L13:
    .dbline 75
;     QK_SCHEDULE_();                   /* process all events produced so far */
L14:
    .dbline 76
;     QF_INT_UNLOCK();
        or  F, 01h

    .dbline 78
;
;     QF_onStartup();                             /* invoke startup callback  */
    xcall _QF_onStartup
    .dbline 80
L15:
    .dbline 80
    .dbline 81
    xcall _QK_onIdle
    .dbline 82
    .dbline 80
    .dbline 80
    xjmp L15
X0:
    .dbline -2
L1:
    .dbline 0 ; func end
    ret
    .dbend
    .area lit(rom, con, rel)
L20:
    .byte 0,1
    .byte 2,2
    .byte 3,3
    .byte 3,3
    .byte 4,4
    .byte 4,4
    .byte 4,4
    .byte 4,4
L21:
    .byte 255,254
    .byte 253,251
    .byte 247,239
    .byte 223,191
    .byte 127
    .area text(rom, con, rel)
    .dbfile ./qkn.c
    .dbfunc e QK_schedule_ _QK_schedule_ fV
    .dbsym s invPow2Lkup L21 A[9:9]c
    .dbsym s log2Lkup L20 A[16:16]c
;            pin -> X+1
;              p -> X+0
_QK_schedule_::
    .dbline -1
    push X
    mov X,SP
    add SP,2
    .dbline 86
;
;     for (;;) {                                    /* enter the QK idle loop */
;         QK_onIdle();                         /* invoke the on-idle callback */
;     }
; }
; /*..........................................................................*/
; /* NOTE: the QK scheduler is entered and exited with interrupts LOCKED */
; void QK_schedule_(void) Q_REENTRANT {
    .dbline 104
;     static uint8_t const Q_ROM Q_ROM_VAR log2Lkup[] = {
;         0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4
;     };
;     static uint8_t const Q_ROM Q_ROM_VAR invPow2Lkup[] = {
;         0xFF, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
;     };
;
;     uint8_t pin;                            /* the initial QK-nano priority */
;     uint8_t p;           /* new highest-priority active object ready to run */
;
;           /* determine the priority of the highest-priority AO ready to run */
; #if (QF_MAX_ACTIVE > 4)
;     if ((QF_readySet_ & 0xF0) != 0) {                 /* upper nibble used? */
;         p = (uint8_t)(Q_ROM_BYTE(log2Lkup[QF_readySet_ >> 4]) + 4);
;     }
;     else                            /* upper nibble of QF_readySet_ is zero */
; #endif
;     {
    .dbline 105
;         p = Q_ROM_BYTE(log2Lkup[QF_readySet_]);
    mov REG[0xd0],>_QF_readySet_
    mov A,[_QF_readySet_]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov [__r0],0
    add [__r1],<L20
    adc [__r0],>L20
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [X+0],A
    .dbline 106
;     }
    .dbline 108
;
;     if (p > QK_currPrio_) {                  /* is the new priority higher? */
    mov REG[0xd0],>_QK_currPrio_
    mov A,[_QK_currPrio_]
    cmp A,[X+0]
    jnc L22
X4:
    .dbline 108
    .dbline 109
;         pin = QK_currPrio_;                    /* save the initial priority */
    mov REG[0xd0],>_QK_currPrio_
    mov A,[_QK_currPrio_]
    mov [X+1],A
L24:
    .dbline 110
;         do {
    .dbline 111
;             QK_currPrio_ = p;  /* new priority becomes the current priority */
    mov A,[X+0]
    mov REG[0xd0],>_QK_currPrio_
    mov [_QK_currPrio_],A
    .dbline 112
;             QF_INT_UNLOCK();          /* unlock interrupts to launch a task */
        or  F, 01h

    .dbline 116
;
;                                       /* dispatch to HSM (execute RTC step) */
; #ifndef QF_FSM_ACTIVE
;             QHsm_dispatch((QHsm *)Q_ROM_PTR(QF_active[p].act));
    mov REG[0xd0],>__r0
    mov A,[X+0]
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
    mov [__r0],A
    pop A
    inc X
    adc A,0
    romx
    mov [__r1],A
    pop X
    mov A,[__r0]
    push A
    mov A,[__r1]
    push A
    xcall _QHsm_dispatch
    add SP,-2
    .dbline 121
; #else
;             QFsm_dispatch((QFsm *)Q_ROM_PTR(QF_active[p].act));
; #endif
;
;             QF_INT_LOCK();
        and F, FEh

    .dbline 123
;                /* set cb and a again, in case they change over the RTC step */
;             l_act = (QActive *)Q_ROM_PTR(QF_active[p].act);
    mov REG[0xd0],>__r0
    mov A,[X+0]
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
    mov REG[0xd0],>_l_act
    mov [_l_act],A
    pop A
    inc X
    adc A,0
    romx
    mov [_l_act+1],A
    pop X
    .dbline 125
;
;             if ((--l_act->nUsed) == (uint8_t)0) {/*is queue becoming empty? */
    mov A,[_l_act+1]
    add A,7
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
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
    jnz L27
    .dbline 125
    .dbline 127
;                                                      /* clear the ready bit */
;                 QF_readySet_ &= Q_ROM_BYTE(invPow2Lkup[p]);
    mov A,[X+0]
    mov [__r1],A
    mov [__r0],0
    add [__r1],<L21
    adc [__r0],>L21
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov REG[0xd0],>_QF_readySet_
    and [_QF_readySet_],A
    .dbline 128
;             }
    xjmp L28
L27:
    .dbline 129
;             else {
    .dbline 130
;                 Q_SIG(l_act) = ((QEvent *)Q_ROM_PTR(QF_active[p].queue))
    mov REG[0xd0],>__r0
    mov A,[X+0]
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
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
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
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,2
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 133
;                                    [l_act->tail].sig;
; #if (Q_PARAM_SIZE != 0)
;                 Q_PAR(l_act) = ((QEvent *)Q_ROM_PTR(QF_active[p].queue))
    mov A,[X+0]
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
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
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
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,3
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 136
;                                    [l_act->tail].par;
; #endif
;                 if (l_act->tail == (uint8_t)0) {            /* wrap around? */
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd4],A
    mvi A,[__r1]
    cmp A,0
    jnz L31
    .dbline 136
    .dbline 137
;                     l_act->tail = Q_ROM_BYTE(QF_active[p].end);/* wrap tail */
    mov A,[X+0]
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
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r3],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
    adc A,0
    mov REG[0xd0],>__r0
    mov REG[0xd5],A
    mov A,[__r0]
    mvi [__r3],A
    .dbline 138
;                 }
L31:
    .dbline 139
;                 --l_act->tail;
    mov REG[0xd0],>_l_act
    mov A,[_l_act+1]
    add A,6
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov REG[0xd0],>_l_act
    mov A,[_l_act]
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
    .dbline 140
;             }
L28:
    .dbline 142
;                           /* determine the highest-priority AO ready to run */
;             if (QF_readySet_ != (uint8_t)0) {
    mov REG[0xd0],>_QF_readySet_
    cmp [_QF_readySet_],0
    jz L34
    .dbline 142
    .dbline 149
; #if (QF_MAX_ACTIVE > 4)
;                 if ((QF_readySet_ & 0xF0) != 0) {     /* upper nibble used? */
;                     p = (uint8_t)(Q_ROM_BYTE(log2Lkup[QF_readySet_ >> 4])+ 4);
;                 }
;                 else                /* upper nibble of QF_readySet_ is zero */
; #endif
;                 {
    .dbline 150
;                     p = Q_ROM_BYTE(log2Lkup[QF_readySet_]);
    mov A,[_QF_readySet_]
    mov REG[0xd0],>__r0
    mov [__r1],A
    mov [__r0],0
    add [__r1],<L20
    adc [__r0],>L20
    mov A,[__r0]
    push X
    mov X,[__r1]
    romx
    pop X
    mov [X+0],A
    .dbline 151
;                 }
    .dbline 152
;             }
    xjmp L35
L34:
    .dbline 153
;             else {
    .dbline 154
;                 p = (uint8_t)0;                    /* break out of the loop */
    mov [X+0],0
    .dbline 155
;             }
L35:
    .dbline 157
L25:
    .dbline 157
    mov A,[X+1]
    cmp A,[X+0]
    jc L24
X5:
    .dbline 158
    mov A,[X+1]
    mov REG[0xd0],>_QK_currPrio_
    mov [_QK_currPrio_],A
    .dbline 159
L22:
    .dbline -2
    .dbline 160
;
;         } while (p > pin);      /* is the new priority higher than initial? */
;         QK_currPrio_ = pin;                 /* restore the initial priority */
;     }
; }
L19:
    add SP,-2
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l pin 1 c
    .dbsym l p 0 c
    .dbend
    .area data(ram, con, rel)
    .dbfile ./qkn.c
_l_act:
    .byte 0,0
    .dbsym s l_act _l_act pX

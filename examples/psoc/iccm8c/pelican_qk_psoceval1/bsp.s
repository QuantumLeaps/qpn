    cpu LMM
    .module bsp.c
    .area text(rom, con, rel)
    .dbfile ./bsp.c
    .dbfunc s delay _delay fV
;          count -> X-5
_delay:
    .dbline -1
    push X
    mov X,SP
    .dbline 45
; /*****************************************************************************
; * Product: BSP for the PSoCEVAL1 board, with QK-nano
; * Last Updated for Version: 4.0.02
; * Date of the Last Update:  Nov 04, 2008
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
; #include "pelican.h"
; #include "bsp.h"
;
; #define SW_1 0x01
;
; /*..........................................................................*/
; static void delay(uint16_t count) {
    xjmp L3
L2:
    .dbline 46
    .dbline 47
        nop

    .dbline 48
L3:
    .dbline 46
;     while ((--count) != 0) {
    mov REG[0xd0],>__r0
    mov A,[X-4]
    sub A,1
    mov [__r1],A
    mov A,[X-5]
    sbb A,0
    mov [__r0],A
    mov A,[__r1]
    mov [X-4],A
    mov A,[__r0]
    mov [X-5],A
    cmp [__r0],0
    jnz L2
    cmp [__r1],0
    jnz L2
X0:
    .dbline -2
    .dbline 49
;         asm("nop");
;     }
; }
L1:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l count -5 i
    .dbend
    .area data(ram, con, rel)
    .dbfile ./bsp.c
L6:
    .byte 0
    .area data(ram, con, rel)
    .dbfile ./bsp.c
L7:
    .byte 0
    .area text(rom, con, rel)
    .dbfile ./bsp.c
    .dbfunc e BSP_tick _BSP_tick fV
    .dbsym s debounce_state L7 c
    .dbsym s sw_debounced L6 c
;             sw -> X+0
_BSP_tick::
    .dbline -1
    or F,-64
    push A
    mov A,REG[0xd0]
    push A
    mov A,REG[0xd3]
    push A
    mov A,REG[0xd4]
    push A
    mov A,REG[0xd5]
    push A
    mov REG[0xd0],>__r0
    mov A,[__r0]
    push A
    mov A,[__r1]
    push A
    mov A,[__r2]
    push A
    mov A,[__r3]
    push A
    mov A,[__r4]
    push A
    mov A,[__r5]
    push A
    mov A,[__r6]
    push A
    mov A,[__r7]
    push A
    mov A,[__r8]
    push A
    mov A,[__r9]
    push A
    mov A,[__r10]
    push A
    mov A,[__r11]
    push A
    mov A,[__rX]
    push A
    mov A,[__rY]
    push A
    mov A,[__rZ]
    push A
    push X
    mov X,SP
    add SP,3
    .dbline 52
; /*..........................................................................*/
; #pragma interrupt_handler BSP_tick
; void BSP_tick(void) {
    .dbline 57
;     static uint8_t sw_debounced;
;     static uint8_t debounce_state;
;     uint8_t sw;
;
;     QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    .dbline 59
;
;     LED_3_Invert();                                       /* toggle the LED */
    push X
    xcall _LED_3_Invert
    pop X
    .dbline 61
;
;     QF_tick();                                      /* process armed timers */
    xcall _QF_tick
    .dbline 63
;
;     sw = (PRT1DR & SW_1);                                /* read the switch */
    mov A,REG[0x4]
    and A,1
    mov [X+0],A
    .dbline 64
    mov REG[0xd0],>L7
    mov A,[L7]
    mov [X+2],A
    mov [X+1],0
    cmp [X+1],0
    jnz X2
    cmp [X+2],0
    jz L11
X2:
    cmp [X+1],0
    jnz X3
    cmp [X+2],1
    jz L14
X3:
    cmp [X+1],0
    jnz X4
    cmp [X+2],2
    jz L17
X4:
    xjmp L8
X1:
    .dbline 64
;     switch (debounce_state) {              /* switch debounce state machine */
L11:
    .dbline 66
;         case 0:
;             if (sw != sw_debounced) {
    mov REG[0xd0],>L6
    mov A,[X+0]
    cmp A,[L6]
    jz L9
    .dbline 66
    .dbline 67
;                 debounce_state = 1;         /* transition to the next state */
    mov REG[0xd0],>L7
    mov [L7],1
    .dbline 68
;             }
    .dbline 69
;             break;
    xjmp L9
L14:
    .dbline 71
;         case 1:
;             if (sw != sw_debounced) {
    mov REG[0xd0],>L6
    mov A,[X+0]
    cmp A,[L6]
    jz L15
    .dbline 71
    .dbline 72
;                 debounce_state = 2;         /* transition to the next state */
    mov REG[0xd0],>L7
    mov [L7],2
    .dbline 73
;             }
    xjmp L9
L15:
    .dbline 74
;             else {
    .dbline 75
;                 debounce_state = 0;           /* transition back to state 0 */
    mov REG[0xd0],>L7
    mov [L7],0
    .dbline 76
;             }
    .dbline 77
;             break;
    xjmp L9
L17:
    .dbline 79
;         case 2:
;             if (sw != sw_debounced) {
    mov REG[0xd0],>L6
    mov A,[X+0]
    cmp A,[L6]
    jz L18
    .dbline 79
    .dbline 80
;                 sw_debounced = sw;       /* save the debounced switch value */
    mov A,[X+0]
    mov [L6],A
    .dbline 82
;
;                 if (sw == SW_1) {               /* is the button depressed? */
    cmp [X+0],1
    jnz L20
    .dbline 82
    .dbline 83
;                     QActive_postISR((QActive *)&AO_Pelican,
    mov A,0
    push A
    mov A,5
    push A
    mov A,>_AO_Pelican
    push A
    mov A,<_AO_Pelican
    push A
    xcall _QActive_postISR
    add SP,-4
    .dbline 85
;                                     PEDS_WAITING_SIG, 0);
;                 }
L20:
    .dbline 86
;             }
L18:
    .dbline 87
;             debounce_state = 0;               /* transition back to state 0 */
    mov REG[0xd0],>L7
    mov [L7],0
    .dbline 88
;             break;
L8:
L9:
    .dbline 91
    mov REG[0xd0],>_QF_readySet_
    cmp [_QF_readySet_],0
    jz L22
    .dbline 91
    .dbline 91
    xcall _QK_schedule_
    .dbline 91
L22:
    .dbline 91
L23:
    .dbline -2
    .dbline 92
;     }
;
;     QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
; }
L5:
    add SP,-3
    pop X
    mov REG[0xD0],>__r0
    pop A
    mov [__rZ],A
    pop A
    mov [__rY],A
    pop A
    mov [__rX],A
    pop A
    mov [__r11],A
    pop A
    mov [__r10],A
    pop A
    mov [__r9],A
    pop A
    mov [__r8],A
    pop A
    mov [__r7],A
    pop A
    mov [__r6],A
    pop A
    mov [__r5],A
    pop A
    mov [__r4],A
    pop A
    mov [__r3],A
    pop A
    mov [__r2],A
    pop A
    mov [__r1],A
    pop A
    mov [__r0],A
    pop A
    mov REG[213],A
    pop A
    mov REG[212],A
    pop A
    mov REG[211],A
    pop A
    mov REG[208],A
    pop A
    .dbline 0 ; func end
    reti
    .dbsym l sw 0 c
    .dbend
    .dbfunc e BSP_init _BSP_init fV
_BSP_init::
    .dbline -1
    .dbline 94
; /*..........................................................................*/
; void BSP_init(void) {
    .dbline 95
;     LED_1_Start();
    push X
    xcall _LED_1_Start
    .dbline 96
;     LED_2_Start();
    xcall _LED_2_Start
    .dbline 97
;     LED_3_Start();
    xcall _LED_3_Start
    .dbline 98
;     LED_4_Start();
    xcall _LED_4_Start
    pop X
    .dbline 100
;
;     PRT1DR &= ~SW_1;             /* set the output direction for the SW pin */
    and REG[0x4],-2
    .dbline 102
;
;     UART_1_Start(UART_PARITY_NONE);                          /* enable UART */
    push X
    mov A,0
    xcall _UART_1_Start
    .dbline 103
;     UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    .dbline 104
;     UART_1_CPutString("PELICAN example with QK-nano, QP-nano ");
    mov A,>L25
    push A
    mov A,<L25
    mov X,A
    pop A
    xcall _UART_1_CPutString
    pop X
    .dbline 105
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
    .dbline 106
;     UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    .dbline 108
;
;     SleepTimer_1_Start();
    xcall _SleepTimer_1_Start
    .dbline 109
;     SleepTimer_1_SetInterval(SleepTimer_1_64_HZ);     /* set interrupt rate */
    mov A,8
    xcall _SleepTimer_1_SetInterval
    pop X
    .dbline -2
    .dbline 110
; }
L24:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e BSP_showState _BSP_showState fV
;          state -> X-6
;           prio -> X-4
_BSP_showState::
    .dbline -1
    push X
    mov X,SP
    .dbline 112
    .dbline 113
    cmp [X-4],1
    jnz L27
    .dbline 113
    .dbline 114
    push X
    mov A,[X-6]
    push A
    mov A,[X-5]
    mov X,A
    pop A
    xcall _UART_1_CPutString
    .dbline 115
    xcall _UART_1_PutCRLF
    pop X
    .dbline 116
    mov A,1
    push A
    mov A,-12
    push A
    xcall _delay
    add SP,-2
    .dbline 117
L27:
    .dbline -2
    .dbline 118
; /*..........................................................................*/
; void BSP_showState(uint8_t prio, char const Q_ROM *state) {
;     if (prio == 1) {                                /* PELICAN crossing AO? */
;         UART_1_CPutString(state);                     /* output the message */
;         UART_1_PutCRLF();
;         delay(500);    /* wait for the transmission to complete, see NOTE01 */
;     }
; }
L26:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l state -6 pc
    .dbsym l prio -4 c
    .dbend
    .dbfunc e BSP_signalPeds _BSP_signalPeds fV
;            sig -> X-4
_BSP_signalPeds::
    .dbline -1
    push X
    mov X,SP
    .dbline 120
; /*..........................................................................*/
; void BSP_signalPeds(enum BSP_PedsSignal sig) {
    .dbline 121
;     if (sig == PEDS_DONT_WALK) {
    cmp [X-4],0
    jnz L30
    .dbline 121
    .dbline 122
;         LED_1_On();
    push X
    xcall _LED_1_On
    pop X
    .dbline 123
;     }
    xjmp L31
L30:
    .dbline 124
    .dbline 125
    push X
    xcall _LED_1_Off
    pop X
    .dbline 126
L31:
    .dbline -2
    .dbline 127
;     else {
;         LED_1_Off();
;     }
; }
L29:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l sig -4 c
    .dbend
    .dbfunc e BSP_signalCars _BSP_signalCars fV
;            sig -> X-4
_BSP_signalCars::
    .dbline -1
    push X
    mov X,SP
    .dbline 129
; /*..........................................................................*/
; void BSP_signalCars(enum BSP_CarsSignal sig) {
    .dbline 130
;     if (sig == CARS_GREEN) {
    cmp [X-4],2
    jnz L33
    .dbline 130
    .dbline 131
;         LED_2_On();
    push X
    xcall _LED_2_On
    pop X
    .dbline 132
;     }
    xjmp L34
L33:
    .dbline 133
    .dbline 134
    push X
    xcall _LED_2_Off
    pop X
    .dbline 135
L34:
    .dbline -2
    .dbline 136
;     else {
;         LED_2_Off();
;     }
; }
L32:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l sig -4 c
    .dbend
    .dbfunc e QF_onStartup _QF_onStartup fV
_QF_onStartup::
    .dbline -1
    .dbline 138
; /*..........................................................................*/
; void QF_onStartup(void) {
    .dbline 139
;     SleepTimer_1_EnableInt();
    push X
    xcall _SleepTimer_1_EnableInt
    pop X
    .dbline 140
;     QF_INT_UNLOCK();                                   /* unlock interrupts */
        or  F, 01h

    .dbline -2
    .dbline 141
; }
L35:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e QK_init _QK_init fV
_QK_init::
    .dbline -1
    .dbline 143
; /*..........................................................................*/
; void QK_init(void) {
    .dbline -2
    .dbline 144
; }
L36:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e QK_onIdle _QK_onIdle fV
_QK_onIdle::
    .dbline -1
    .dbline 146
; /*..........................................................................*/
; void QK_onIdle(void) {
    .dbline 148
;
;     QF_INT_LOCK();                               /* blink LED_4, see NOTE02 */
        and F, FEh

    .dbline 149
;     LED_4_On();
    push X
    xcall _LED_4_On
    .dbline 150
;     LED_4_Off();
    xcall _LED_4_Off
    pop X
    .dbline 151
;     QF_INT_UNLOCK();
        or  F, 01h

    .dbline 154
;
; #ifdef NDEBUG
;     M8C_Sleep;                   /* tread-safe transition to the sleep mode */
    or REG[0xff],8
    .dbline -2
    .dbline 156
; #endif
; }
L37:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e Q_onAssert _Q_onAssert fV
;           line -> X-7
;           file -> X-5
_Q_onAssert::
    .dbline -1
    push X
    mov X,SP
    .dbline 158
; /*..........................................................................*/
; void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    .dbline 159
;     (void)file;                                   /* avoid compiler warning */
    .dbline 160
;     (void)line;                                   /* avoid compiler warning */
    .dbline 161
;     QF_INT_LOCK();            /* make sure that all interrupts are disabled */
        and F, FEh

    .dbline 162
L39:
    .dbline 162
    .dbline 163
    push X
    xcall _LED_4_On
    pop X
    .dbline 164
    mov A,39
    push A
    mov A,16
    push A
    xcall _delay
    add SP,-2
    .dbline 165
    push X
    xcall _LED_4_Off
    pop X
    .dbline 166
    mov A,39
    push A
    mov A,16
    push A
    xcall _delay
    add SP,-2
    .dbline 167
    .dbline 162
    .dbline 162
    xjmp L39
X5:
    .dbline -2
L38:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l line -7 I
    .dbsym l file -5 ppc
    .dbend
    .area lit(rom, con, rel)
L25:
    .byte 'P,'E,'L,'I,'C,'A,'N,32,'e,'x,'a,'m,'p,'l,'e,32
    .byte 'w,'i,'t,'h,32,'Q,'K,45,'n,'a,'n,'o,44,32,'Q,'P
    .byte 45,'n,'a,'n,'o,32,0

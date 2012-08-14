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
; * Product: BSP for the PSoCEVAL1 board
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
;     LED_3_Invert();                                       /* toggle the LED */
    push X
    xcall _LED_3_Invert
    pop X
    .dbline 59
;
;     QF_tick();                                      /* process armed timers */
    xcall _QF_tick
    .dbline 61
;
;     sw = (PRT1DR & SW_1);                                /* read the switch */
    mov A,REG[0x4]
    and A,1
    mov [X+0],A
    .dbline 62
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
    .dbline 62
;     switch (debounce_state) {              /* switch debounce state machine */
L11:
    .dbline 64
;         case 0:
;             if (sw != sw_debounced) {
    mov REG[0xd0],>L6
    mov A,[X+0]
    cmp A,[L6]
    jz L9
    .dbline 64
    .dbline 65
;                 debounce_state = 1;         /* transition to the next state */
    mov REG[0xd0],>L7
    mov [L7],1
    .dbline 66
;             }
    .dbline 67
;             break;
    xjmp L9
L14:
    .dbline 69
;         case 1:
;             if (sw != sw_debounced) {
    mov REG[0xd0],>L6
    mov A,[X+0]
    cmp A,[L6]
    jz L15
    .dbline 69
    .dbline 70
;                 debounce_state = 2;         /* transition to the next state */
    mov REG[0xd0],>L7
    mov [L7],2
    .dbline 71
;             }
    xjmp L9
L15:
    .dbline 72
;             else {
    .dbline 73
;                 debounce_state = 0;           /* transition back to state 0 */
    mov REG[0xd0],>L7
    mov [L7],0
    .dbline 74
;             }
    .dbline 75
;             break;
    xjmp L9
L17:
    .dbline 77
;         case 2:
;             if (sw != sw_debounced) {
    mov REG[0xd0],>L6
    mov A,[X+0]
    cmp A,[L6]
    jz L18
    .dbline 77
    .dbline 78
;                 sw_debounced = sw;       /* save the debounced switch value */
    mov A,[X+0]
    mov [L6],A
    .dbline 80
;
;                 if (sw == SW_1) {               /* is the button depressed? */
    cmp [X+0],1
    jnz L20
    .dbline 80
    .dbline 81
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
    .dbline 83
;                                     PEDS_WAITING_SIG, 0);
;                 }
L20:
    .dbline 84
;             }
L18:
    .dbline 85
;             debounce_state = 0;               /* transition back to state 0 */
    mov REG[0xd0],>L7
    mov [L7],0
    .dbline 86
;             break;
L8:
L9:
    .dbline -2
    .dbline 88
;     }
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
    .dbline 90
; /*..........................................................................*/
; void BSP_init(void) {
    .dbline 91
;     LED_1_Start();
    push X
    xcall _LED_1_Start
    .dbline 92
;     LED_2_Start();
    xcall _LED_2_Start
    .dbline 93
;     LED_3_Start();
    xcall _LED_3_Start
    .dbline 94
;     LED_4_Start();
    xcall _LED_4_Start
    pop X
    .dbline 96
;
;     PRT1DR &= ~SW_1;             /* set the output direction for the SW pin */
    and REG[0x4],-2
    .dbline 98
;
;     UART_1_Start(UART_PARITY_NONE);                          /* enable UART */
    push X
    mov A,0
    xcall _UART_1_Start
    .dbline 99
;     UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    .dbline 100
;     UART_1_CPutString("PELICAN example, QP-nano ");
    mov A,>L23
    push A
    mov A,<L23
    mov X,A
    pop A
    xcall _UART_1_CPutString
    pop X
    .dbline 101
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
    .dbline 102
;     UART_1_PutCRLF();
    xcall _UART_1_PutCRLF
    .dbline 104
;
;     SleepTimer_1_Start();
    xcall _SleepTimer_1_Start
    .dbline 105
;     SleepTimer_1_SetInterval(SleepTimer_1_64_HZ);     /* set interrupt rate */
    mov A,8
    xcall _SleepTimer_1_SetInterval
    pop X
    .dbline -2
    .dbline 106
; }
L22:
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
    .dbline 108
    .dbline 109
    cmp [X-4],1
    jnz L25
    .dbline 109
    .dbline 110
    push X
    mov A,[X-6]
    push A
    mov A,[X-5]
    mov X,A
    pop A
    xcall _UART_1_CPutString
    .dbline 111
    xcall _UART_1_PutCRLF
    pop X
    .dbline 112
    mov A,1
    push A
    mov A,-12
    push A
    xcall _delay
    add SP,-2
    .dbline 113
L25:
    .dbline -2
    .dbline 114
; /*..........................................................................*/
; void BSP_showState(uint8_t prio, char const Q_ROM *state) {
;     if (prio == 1) {                                /* PELICAN crossing AO? */
;         UART_1_CPutString(state);                     /* output the message */
;         UART_1_PutCRLF();
;         delay(500);    /* wait for the transmission to complete, see NOTE01 */
;     }
; }
L24:
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
    .dbline 116
; /*..........................................................................*/
; void BSP_signalPeds(enum BSP_PedsSignal sig) {
    .dbline 117
;     if (sig == PEDS_DONT_WALK) {
    cmp [X-4],0
    jnz L28
    .dbline 117
    .dbline 118
;         LED_1_On();
    push X
    xcall _LED_1_On
    pop X
    .dbline 119
;     }
    xjmp L29
L28:
    .dbline 120
    .dbline 121
    push X
    xcall _LED_1_Off
    pop X
    .dbline 122
L29:
    .dbline -2
    .dbline 123
;     else {
;         LED_1_Off();
;     }
; }
L27:
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
    .dbline 125
; /*..........................................................................*/
; void BSP_signalCars(enum BSP_CarsSignal sig) {
    .dbline 126
;     if (sig == CARS_GREEN) {
    cmp [X-4],2
    jnz L31
    .dbline 126
    .dbline 127
;         LED_2_On();
    push X
    xcall _LED_2_On
    pop X
    .dbline 128
;     }
    xjmp L32
L31:
    .dbline 129
    .dbline 130
    push X
    xcall _LED_2_Off
    pop X
    .dbline 131
L32:
    .dbline -2
    .dbline 132
;     else {
;         LED_2_Off();
;     }
; }
L30:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l sig -4 c
    .dbend
    .dbfunc e QF_onStartup _QF_onStartup fV
_QF_onStartup::
    .dbline -1
    .dbline 134
; /*..........................................................................*/
; void QF_onStartup(void) {
    .dbline 135
;     SleepTimer_1_EnableInt();
    push X
    xcall _SleepTimer_1_EnableInt
    pop X
    .dbline 136
;     QF_INT_UNLOCK();                                   /* unlock interrupts */
        or  F, 01h

    .dbline -2
    .dbline 137
; }
L33:
    .dbline 0 ; func end
    ret
    .dbend
    .dbfunc e QF_onIdle _QF_onIdle fV
_QF_onIdle::
    .dbline -1
    .dbline 139
; /*..........................................................................*/
; void QF_onIdle(void) {        /* entered with interrupts LOCKED, see NOTE02 */
    .dbline 141
;
;     LED_4_On();                                  /* blink LED_4, see NOTE03 */
    push X
    xcall _LED_4_On
    .dbline 142
;     LED_4_Off();
    xcall _LED_4_Off
    pop X
    .dbline 145
;
; #ifdef NDEBUG
;     M8C_Sleep;       /* tread-safe transition to the sleep mode, see NOTE04 */
    or REG[0xff],8
    .dbline 148
; #endif
;
;     QF_INT_UNLOCK();
        or  F, 01h

    .dbline -2
    .dbline 149
; }
L34:
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
    .dbline 151
; /*..........................................................................*/
; void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    .dbline 152
;     (void)file;                                   /* avoid compiler warning */
    .dbline 153
;     (void)line;                                   /* avoid compiler warning */
    .dbline 154
;     QF_INT_LOCK();            /* make sure that all interrupts are disabled */
        and F, FEh

    .dbline 155
L36:
    .dbline 155
    .dbline 156
    push X
    xcall _LED_4_On
    pop X
    .dbline 157
    mov A,39
    push A
    mov A,16
    push A
    xcall _delay
    add SP,-2
    .dbline 158
    push X
    xcall _LED_4_Off
    pop X
    .dbline 159
    mov A,39
    push A
    mov A,16
    push A
    xcall _delay
    add SP,-2
    .dbline 160
    .dbline 155
    .dbline 155
    xjmp L36
X5:
    .dbline -2
L35:
    pop X
    .dbline 0 ; func end
    ret
    .dbsym l line -7 I
    .dbsym l file -5 ppc
    .dbend
    .area lit(rom, con, rel)
L23:
    .byte 'P,'E,'L,'I,'C,'A,'N,32,'e,'x,'a,'m,'p,'l,'e,44
    .byte 32,'Q,'P,45,'n,'a,'n,'o,32,0

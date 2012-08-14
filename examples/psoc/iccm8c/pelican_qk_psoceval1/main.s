    cpu LMM
    .module main.c
    .area lit(rom, con, rel)
_QF_active::
    .word 0
    .word 0
    .byte 0
    .word _AO_Pelican
    .word _l_pelicanQueue
    .byte 2
    .word _AO_Oper
    .word _l_operQueue
    .byte 1
    .dbfile ./main.c
    .dbsym e QF_active _QF_active A[15:3]X
    .area text(rom, con, rel)
    .dbfile ./main.c
    .dbfunc e main _main fV
_main::
    .dbline -1
    .dbline 47
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
; #include "qpn_port.h"                                       /* QP-nano port */
; #include "bsp.h"                             /* Board Support Package (BSP) */
; #include "pelican.h"                               /* application interface */
;
; /*..........................................................................*/
; static QEvent  l_pelicanQueue[2];
; static QEvent  l_operQueue[1];
;
; /* QF_active[] array defines all active object control blocks --------------*/
; QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
;     { (QActive *)0,           (QEvent *)0,    0                     },
;     { (QActive *)&AO_Pelican, l_pelicanQueue, Q_DIM(l_pelicanQueue) },
;     { (QActive *)&AO_Oper,    l_operQueue,    Q_DIM(l_operQueue)    }
; };
;
; /* make sure that the QF_active[] array matches QF_MAX_ACTIVE in qpn_port.h */
; Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);
;
; /*..........................................................................*/
; void main (void) {
    .dbline 48
;     Pelican_ctor();                           /* instantiate the Pelican AO */
    xcall _Pelican_ctor
    .dbline 49
;     Oper_ctor();                              /* instantiate the Oper    AO */
    xcall _Oper_ctor
    .dbline 51
;
;     BSP_init();                                     /* initialize the board */
    xcall _BSP_init
    .dbline 53
;
;     QF_run();                                /* transfer control to QF-nano */
    xcall _QF_run
    .dbline -2
    .dbline 54
; }
L1:
    .dbline 0 ; func end
    jmp .
    .dbend
    .area data(ram, con, rel)
    .dbfile ./main.c
_l_operQueue:
    .byte 0,0
    .dbsym s l_operQueue _l_operQueue A[2:1]X
    .area data(ram, con, rel)
    .dbfile ./main.c
_l_pelicanQueue:
    .byte 0,0,0,0
    .dbsym s l_pelicanQueue _l_pelicanQueue A[4:2]X

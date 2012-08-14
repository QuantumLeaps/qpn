/*****************************************************************************
* Product: BSP for the PSoCEVAL1 board
* Last Updated for Version: 4.0.01
* Date of the Last Update:  Jun 13, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*
*-----------------------------------------------------------------------------
* PSoCEVAL1 board custom wiring:
*     P00 -> LED1
*     P01 -> LED2
*     P02 -> LED3
*     P03 -> LED4
*     P10 -> SW
*     P16 -> RX = Serial RX
*     P27 -> TX = Serial TX
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"
#include "qhsmtst.h"

/*..........................................................................*/
void BSP_init(void) {
    LED_1_Start();
    LED_2_Start();
    LED_3_Start();
    LED_4_Start();

    UART_1_Start(UART_PARITY_NONE);                          /* enable UART */
    UART_1_PutCRLF();
    UART_1_CPutString("QHsmTst example, QP-nano ");       /* opening string */
    UART_1_CPutString(QP_getVersion());                  /* QP-nano version */
    UART_1_PutCRLF();
}
/*..........................................................................*/
void BSP_exit(void) {
    for (;;) {       /* NOTE: replace the loop with reset for final version */
        uint16_t volatile dly;
        LED_4_On();
        for (dly = 10000; dly != 0; --dly) {
        }
        LED_4_Off();
        for (dly = 10000; dly != 0; --dly) {
        }
    }
}
/*..........................................................................*/
void BSP_display(char const *msg) {
    UART_1_CPutString(msg);                           /* output the message */
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    BSP_exit();
}


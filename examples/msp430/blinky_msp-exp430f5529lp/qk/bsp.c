/*****************************************************************************
* Product: "Blinky" on MSP-EXP430F5529LP, preemptive QK-nano kernel
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-04-04
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <www.gnu.org/licenses/>.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*****************************************************************************/
#include "qpn.h"
#include "blinky.h"
#include "bsp.h"

#include <msp430f5529.h>  /* MSP430 variant used */
/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
/* 1MHz clock setting, see BSP_init() */
#define BSP_MCK     1000000U
#define BSP_SMCLK   1000000U

#define LED1        (1U << 0)
#define LED2        (1U << 7)

#define BTN_S1      (1U << 1)


/* ISRs used in this project ===============================================*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    __interrupt void TIMER0_A0_ISR (void); /* prototype */
    #pragma vector=TIMER0_A0_VECTOR
    __interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
    __attribute__ ((interrupt(TIMER0_A0_VECTOR)))
    void TIMER0_A0_ISR (void)
#else
    #error Compiler not supported!
#endif
{
    QK_ISR_ENTRY();    /* inform QK about entering the ISR */

    QF_tickXISR(0U);   /* process time events for rate 0 */

    QK_ISR_EXIT();     /* inform QK about exiting the ISR */
}


/* BSP functions ===========================================================*/
void BSP_init(void) {
    WDTCTL = WDTPW | WDTHOLD; /* stop watchdog timer */

    /* leave the MCK and SMCLK at default DCO setting */

    P1DIR |= LED1;  /* set LED1 pin to output  */
    P4DIR |= LED2;  /* set LED2 pin to output  */
}
/*..........................................................................*/
void BSP_ledOff(void) {
    P1OUT &= ~LED1; /* turn LED1 off */
}
/*..........................................................................*/
void BSP_ledOn(void) {
    P1OUT |= LED1;  /* turn LED1 on */
}


/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    TA0CCTL0 = CCIE; /* CCR0 interrupt enabled */
    TA0CCR0 = BSP_MCK / BSP_TICKS_PER_SEC;
    TA0CTL = TASSEL_2 + MC_1 + TACLR; /* SMCLK, upmode, clear TAR */
}
/*..........................................................................*/
void QK_onIdle(void) {
    /* toggle LED2 on and then off, see NOTE1 */
    QF_INT_DISABLE();
    P4OUT |=  LED2;        /* turn LED2 on */
    P4OUT &= ~LED2;        /* turn LED2 off */
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular MSP430 MCU.
    */
    __low_power_mode_1(); /* Enter LPM1; also ENABLES interrupts */
#endif
}

/*..........................................................................*/
Q_NORETURN Q_onAssert(char const Q_ROM * const file, int line) {
    /* implement the error-handling policy for your application!!! */
    QF_INT_DISABLE(); /* disable all interrupts */

    /* cause the reset of the CPU... */
    WDTCTL = WDTPW | WDTHOLD;
    __asm("    push &0xFFFE");
    /* return from function does the reset */
}

/*****************************************************************************
* NOTE1:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/


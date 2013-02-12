/*****************************************************************************
* Product: BSP for PELICAN crossing example, TMS320C2802x PICCOLO controlSTICK
* Last Updated for Version: 4.5.03
* Date of the Last Update:  Jan 18, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"

#include "DSP2802x_Device.h"                   /* defines the C28027 device */
#include <string.h>                                         /* for memcpy() */

Q_DEFINE_THIS_FILE

/* running from Flash */
#define FLASH 1

/*----------------------------------------------------------------------------
*  Target device (in DSP2802x_Device.h) determines CPU frequency
*      (for examples) - either 60 MHz (for 28026 and 28027) or 40 MHz
*      (for 28025, 28024, 28023, and 28022).
*      User does not have to change anything here.
*---------------------------------------------------------------------------*/
#if (DSP28_28026 || DSP28_28027) /* DSP28_28026 || DSP28_28027 devices only */
  #define CPU_FRQ_HZ    60000000
#else
  #define CPU_FRQ_HZ    40000000
#endif

#define CPU_TIMER_PERIOD \
    ((CPU_FRQ_HZ + BSP_TICKS_PER_SEC/2) / BSP_TICKS_PER_SEC)

                               /* LED LD2 of the PICCOLO controlSTICK board */
#define LD2_ON()   (GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1)
#define LD2_OFF()  (GpioDataRegs.GPBSET.bit.GPIO34   = 1)

static void PieInit(void);
static void PLLset(Uint16 val);

static void InitFlash(void);
static void CopyFlash(void);

/*..........................................................................*/
/* CPU Timer0 ISR is used for system clock tick */
#pragma CODE_SECTION(cpu_timer0_isr, "ramfuncs"); /* place in RAM for speed */
static interrupt void cpu_timer0_isr(void) {

    QF_tickISR();                         /* handle the QF-nano time events */

      /* Acknowledge this interrupt to receive more interrupts from group 1 */
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
/*..........................................................................*/
// Illegal operation TRAP
static interrupt void illegal_isr(void) {
    Q_ERROR();                                           /* assert an error */
}

/*..........................................................................*/
void BSP_init(void) {

    DINT;                                    // Global Disable all Interrupts
    IER = 0x0000;                            // Disable CPU interrupts
    IFR = 0x0000;                            // Clear all CPU interrupt flags

    EALLOW;
    SysCtrlRegs.WDCR = 0x0068;               // Disable the watchdog initially
    EDIS;

              /* explicitly initialize the QF-nano variables, see NOTE01... */
    QF_readySet_ = (uint8_t)0;

    // The Device_cal function, which copies the ADC & oscillator calibration
    // values from TI reserved OTP into the appropriate trim registers, occurs
    // automatically in the Boot ROM. If the boot ROM code is bypassed during
    // the debug process, the following function MUST be called for the ADC
    // and oscillators to function according to specification.
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;    // Enable ADC peripheral clock
    (*(void (*)(void))0x3D7C80)();           // Auto-calibrate from TI OTP
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0;    // Disable ADC clock
    EDIS;


    // Switch to Internal Oscillator 1 and turn off all other clock
    // sources to minimize power consumption
    EALLOW;
    SysCtrlRegs.CLKCTL.bit.INTOSC1OFF   = 0;
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL = 0; // Clk Src = INTOSC1
    SysCtrlRegs.CLKCTL.bit.XCLKINOFF    = 1; // Turn off XCLKIN
    SysCtrlRegs.CLKCTL.bit.XTALOSCOFF   = 1; // Turn off XTALOSC
    SysCtrlRegs.CLKCTL.bit.INTOSC2OFF   = 1; // Turn off INTOSC2
    EDIS;


    // SYSTEM CLOCK speed based on internal oscillator = 10 MHz
    // 12 =  60 MHz
    // 11 =  55 MHz
    // 10 =  50 MHz
    // 9  =  45 MHz
    // 8  =  40 MHz
    // 7  =  35 MHz
    // 6  =  30 MHz
    // 5  =  25 MHz
    // 4  =  20 MHz
    // 3  =  15 MHz
    // 2  =  10 MHz
    PLLset(12);                              // choose from options above


    // Only used if running from FLASH
    // Note that the macro FLASH is defined by the compiler (-d FLASH)
#ifdef FLASH
    CopyFlash();
#endif //(FLASH)


    // Initialise interrupt controller and Vector Table to defaults for now.
    // Application ISR mapping done later.
    PieInit();

    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;  // hook the CPU timer0 ISR
    /* . . . hook other ISRs */
    EDIS;


    EALLOW;
    // LOW SPEED CLOCKS prescale register settings
    SysCtrlRegs.LOSPCP.all              = 0x0002; // Sysclk / 4
    SysCtrlRegs.XCLK.bit.XCLKOUTDIV     = 2;

    // PERIPHERAL CLOCK ENABLES
    // If you are not using a peripheral you may want to switch
    // the clock off to save power, i.e. set to =0
    //
    // Note: not all peripherals are available on all 280x derivates.
    // Refer to the datasheet for your particular device.
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK    = 0; // ADC

    SysCtrlRegs.PCLKCR3.bit.COMP1ENCLK  = 0; // COMP1
    SysCtrlRegs.PCLKCR3.bit.COMP2ENCLK  = 0; // COMP2

    SysCtrlRegs.PCLKCR0.bit.I2CAENCLK   = 0; // I2C

    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK   = 0; // SPI-A

    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK   = 0; // SCI-A

    SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK  = 0; // eCAP1

    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK  = 0; // ePWM1
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK  = 0; // ePWM2
    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK  = 0; // ePWM3
    SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK  = 0; // ePWM4

    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC   = 0; // Enable TBCLK
    EDIS;


    EALLOW;                                               /* GPIO config... */
    //  GPIO-34 - PIN FUNCTION = LED for F28027 USB dongle
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;    // 0=GPIO,1=COMP2OUT,2=EMU1,3=Resv
    GpioCtrlRegs.GPBDIR .bit.GPIO34 = 1;    // 1=OUTput, 0=INput
    LD2_OFF();
    EDIS;

    // initialize the CPU Timer used for system clock tick
    CpuTimer0Regs.PRD.all      = CPU_TIMER_PERIOD;
    CpuTimer0Regs.TPR.all      = 0;   // Initialize pre-scaler to div by 1
    CpuTimer0Regs.TPRH.all     = 0;
    CpuTimer0Regs.TCR.bit.TSS  = 1;   // 1 = Stop timer initially
    CpuTimer0Regs.TCR.bit.TRB  = 1;   // 1 = reload timer
    CpuTimer0Regs.TCR.bit.SOFT = 0;
    CpuTimer0Regs.TCR.bit.FREE = 0;   // 0 = Timer Free Run Disabled
    CpuTimer0Regs.TCR.bit.TIE  = 1;   // 1 = Enable Timer Interrupt
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_BLANK:
            break;
        case CARS_RED:
            break;
        case CARS_YELLOW:
            break;
        case CARS_GREEN:
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK:
            LD2_ON();
            break;
        case PEDS_BLANK:
            LD2_OFF();
            break;
        case PEDS_WALK:
            LD2_OFF();
            break;
    }
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    (void)state;
}

/*..........................................................................*/
void QF_onStartup(void) {
    CpuTimer0Regs.TCR.bit.TSS = 0;     /* start the system clock tick timer */

    // Enable CPU INT1, which is connected to CPU-Timer 0:
    IER |= M_INT1;

    // Enable PIE: Group 1 interrupt 7 which is connected to CPU-Timer 0:
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    // Enable higher priority real-time debug events:
    ERTM;   // Enable Global realtime interrupt DBGM
}
/*..........................................................................*/
#pragma CODE_SECTION(QF_onIdle, "ramfuncs");      /* place in RAM for speed */
void QF_onIdle(void) {
#ifdef NDEBUG
    asm(" IDLE");               /* go to IDLE mode with interrupts DISABLED */
#endif
    QF_INT_ENABLE();                               /* enable the interrupts */
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    /* Next two lines for debug only to halt the processor here.
    * YOU need to change this policy for the production release!
    */
    asm (" ESTOP0");
    for(;;) {
    }
}


/*==========================================================================*/
// NOTE:
// IN MOST APPLICATIONS THE FUNCTIONS AFTER THIS POINT CAN BE LEFT UNCHANGED
// THE USER NEED NOT REALLY UNDERSTAND THE BELOW CODE TO SUCCESSFULLY RUN THIS
// APPLICATION.

/*..........................................................................*/
/* QActive constructor that explicitly clears critical members, see NOTE01  */
void QActive_ctor(QActive *me, QStateHandler initial) {
    QHsm_ctor(&me->super, initial);
    me->head    = (uint8_t)0;
    me->tail    = (uint8_t)0;
    me->nUsed   = (uint8_t)0;
#if (QF_TIMEEVT_CTR_SIZE != 0)
    me->tickCtr = (QTimeEvtCtr)0;
#endif
}
/*..........................................................................*/
// This function initializes the PLLCR register.
//void InitPll(Uint16 val, Uint16 clkindiv)
void PLLset(Uint16 val) {
   volatile Uint16 iVol;

   // Make sure the PLL is not running in limp mode
   if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0) {
      EALLOW;
      // OSCCLKSRC1 failure detected. PLL running in limp mode.
      // Re-enable missing clock logic.
      SysCtrlRegs.PLLSTS.bit.MCLKCLR = 1;
      EDIS;
      // Replace this line with a call to an appropriate
      // SystemShutdown(); function.
      asm("   ESTOP0");     // Uncomment for debugging purposes
   }

   // DIVSEL MUST be 0 before PLLCR can be changed from
   // 0x0000. It is set to 0 by an external reset XRSn
   // This puts us in 1/4
   if (SysCtrlRegs.PLLSTS.bit.DIVSEL != 0) {
       EALLOW;
       SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
       EDIS;
   }

   // Change the PLLCR
   if (SysCtrlRegs.PLLCR.bit.DIV != val) {

        EALLOW;
        // Before setting PLLCR turn off missing clock detect logic
        SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
        SysCtrlRegs.PLLCR.bit.DIV = val;
        EDIS;

        // Optional: Wait for PLL to lock.
        // During this time the CPU will switch to OSCCLK/2 until
        // the PLL is stable.  Once the PLL is stable the CPU will
        // switch to the new PLL value.
        //
        // This time-to-lock is monitored by a PLL lock counter.
        //
        // Code is not required to sit and wait for the PLL to lock.
        // However, if the code does anything that is timing critical,
        // and requires the correct clock be locked, then it is best to
        // wait until this switching has completed.

        // Wait for the PLL lock bit to be set.
        // The watchdog should be disabled before this loop, or fed within
        // the loop via ServiceDog().
        //
        while (SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1) {
        }

        EALLOW;
        SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
        EDIS;
    }

    // divide down SysClk by 2 to increase stability
    EALLOW;
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
    EDIS;
}
/*..........................................................................*/
// This function initializes the PIE control registers to a known state.
//
void PieInit(void) {
    int16  i;
    Uint32 *dest = (Uint32 *)&PieVectTable;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;         // disable the PIE Vector Table

                                              // Clear all PIEIER registers...
    PieCtrlRegs.PIEIER1.all  = 0;
    PieCtrlRegs.PIEIER2.all  = 0;
    PieCtrlRegs.PIEIER3.all  = 0;
    PieCtrlRegs.PIEIER4.all  = 0;
    PieCtrlRegs.PIEIER5.all  = 0;
    PieCtrlRegs.PIEIER6.all  = 0;
    PieCtrlRegs.PIEIER7.all  = 0;
    PieCtrlRegs.PIEIER8.all  = 0;
    PieCtrlRegs.PIEIER9.all  = 0;
    PieCtrlRegs.PIEIER10.all = 0;
    PieCtrlRegs.PIEIER11.all = 0;
    PieCtrlRegs.PIEIER12.all = 0;

                                              // Clear all PIEIFR registers...
    PieCtrlRegs.PIEIFR1.all  = 0;
    PieCtrlRegs.PIEIFR2.all  = 0;
    PieCtrlRegs.PIEIFR3.all  = 0;
    PieCtrlRegs.PIEIFR4.all  = 0;
    PieCtrlRegs.PIEIFR5.all  = 0;
    PieCtrlRegs.PIEIFR6.all  = 0;
    PieCtrlRegs.PIEIFR7.all  = 0;
    PieCtrlRegs.PIEIFR8.all  = 0;
    PieCtrlRegs.PIEIFR9.all  = 0;
    PieCtrlRegs.PIEIFR10.all = 0;
    PieCtrlRegs.PIEIFR11.all = 0;
    PieCtrlRegs.PIEIFR12.all = 0;

    EALLOW;
    for (i = 0; i < 128; ++i) {
        *dest++ = (Uint32)&illegal_isr;
    }
    EDIS;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
}

#ifdef FLASH
/*..........................................................................*/
// This function initializes the Flash Control registers
// CAUTION: This function MUST be executed out of RAM!!!
// Executing it out of OTP/Flash will yield unpredictable results
#pragma CODE_SECTION(InitFlash, "ramfuncs");
void InitFlash(void) {
    EALLOW;
    // Enable Flash Pipeline mode to improve performance of code
    // executed from Flash.
    FlashRegs.FOPT.bit.ENPIPE = 1;

    // CAUTION:
    // Minimum waitstates required for the flash operating at a given CPU
    // rate must be characterized by TI. Refer to the datasheet for the
    // latest information.

    // Set the Paged Waitstate for the Flash
    FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;

    // Set the Random Waitstate for the Flash
    FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;

    // Set the Waitstate for the OTP
    FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;

    // CAUTION:
    // ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
    FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
    FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
    EDIS;

    // Force a pipeline flush to ensure that the write to
    // the last register configured occurs before returning.
    asm(" RPT #7 || NOP");
}
/*..........................................................................*/
void CopyFlash() {
    extern uint8_t ramfuncs_loadstart, ramfuncs_loadend, ramfuncs_runstart;
    extern uint8_t ramconsts_loadstart, ramconsts_loadend, ramconsts_runstart;

    // Copy time critical code and Flash setup code to RAM
    memcpy(&ramfuncs_runstart, &ramfuncs_loadstart,
           (&ramfuncs_loadend - &ramfuncs_loadstart));

    // Call Flash Initialization to setup flash waitstates
    // This function must reside in RAM
    InitFlash();    // Call the flash wrapper init function

    // Copy RAM constants from Flash to RAM
    memcpy(&ramconsts_runstart, &ramconsts_loadstart,
           (&ramconsts_loadend - &ramconsts_loadstart));
}
#endif                                                            /* FLASH */

/*****************************************************************************
* NOTE01:
* The standard TI startup code (c_int00) does NOT zero the uninitialized
* variables, as required by the C-standard. Since QP relies on the clearing
* of the static uninitialized variables, the critical QP objects are cleared
* explicitly in this BSP.
*/









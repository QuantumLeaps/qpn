;@Id: boot.tpl#851 @
;=============================================================================
;  FILENAME:   boot.asm
;  VERSION:    4.18
;  DATE:       28 June 2007
;
;  DESCRIPTION:
;  M8C Boot Code for CY8C29xxx microcontroller family.
;
;  Copyright (C) Cypress Semiconductor 2000-2005. All rights reserved.
;
; NOTES:
; PSoC Designer's Device Editor uses a template file, BOOT.TPL, located in
; the project's root directory to create BOOT.ASM. Any changes made to
; BOOT.ASM will be  overwritten every time the project is generated; therfore
; changes should be made to BOOT.TPL not BOOT.ASM. Care must be taken when
; modifying BOOT.TPL so that replacement strings (such as @PROJECT_NAME)
; are not accidentally modified.
;
;=============================================================================

include ".\lib\GlobalParams.inc"
include "m8c.inc"
include "m8ssc.inc"
include "memory.inc"

;--------------------------------------
; Export Declarations
;--------------------------------------

export __Start
IF	(TOOLCHAIN & HITECH)
ELSE
export __bss_start
export __data_start
export __idata_start
export __func_lit_start
export __text_start
ENDIF
export  _bGetPowerSetting
export   bGetPowerSetting


;--------------------------------------
; Optimization flags
;--------------------------------------
;
; To change the value of these flags, modify the file boot.tpl, not
; boot.asm. See the notes in the banner comment at the beginning of
; this file.

; Optimization for Assembly language (only) projects and C-language projects
; that do not depend on the C compiler to initialize the values of RAM variables.
;   Set to 1: Support for C Run-time Environment initialization
;   Set to 0: Support for C not included. Faster start up, smaller code space.
;
IF	(TOOLCHAIN & HITECH)
; The C compiler will customize the startup code - it's not required here

C_LANGUAGE_SUPPORT:              equ 0
ELSE
C_LANGUAGE_SUPPORT:              equ 1
ENDIF


; The following equate is required for proper operation. Reseting its value
; is discouraged.  WAIT_FOR_32K is effective only if the crystal oscillator is
; selected.  If the designer chooses to not wait then stabilization of the ECO
; and PLL_Lock must take place within user code. See the family data sheet for
; the requirements of starting the ECO and PLL lock mode.
;
;   Set to 1: Wait for XTAL (& PLL if selected) to stabilize before
;                invoking main
;   Set to 0: Boot code does not wait; clock may not have stabilized by
;               the time code in main starts executing.
;
WAIT_FOR_32K:                    equ 1


; For historical reasons, by default the boot code uses an lcall instruction
; to invoke the user's _main code. If _main executes a return instruction,
; boot provides an infinite loop. By changing the following equate from zero
; to 1, boot's lcall will be replaced by a ljmp instruction, saving two
; bytes on the stack which are otherwise required for the return address. If
; this option is enabled, _main must not return. (Beginning with the 4.2
; release, the C compiler automatically places an infinite loop at the end
; of main, rather than a return instruction.)
;
ENABLE_LJMP_TO_MAIN:             equ 0


;-----------------------------------------------------------------------------
; Interrupt Vector Table
;-----------------------------------------------------------------------------
;
; Interrupt vector table entries are 4 bytes long.  Each one contains
; a jump instruction to an ISR (Interrupt Service Routine), although
; very short ISRs could be encoded within the table itself. Normally,
; vector jump targets are modified automatically according to the user
; modules selected. This occurs when the 'Generate Application' opera-
; tion is run causing PSoC Designer to create boot.asm and the other
; configuration files. If you need to hard code a vector, update the
; file boot.tpl, not boot.asm. See the banner comment at the beginning
; of this file.
;-----------------------------------------------------------------------------

    AREA TOP (ROM, ABS, CON)

    org   0                        ;Reset Interrupt Vector
IF	(TOOLCHAIN & HITECH)
;   jmp   __Start                  ;C compiler fills in this vector
ELSE
    jmp   __Start                  ;First instruction executed following a Reset
ENDIF

    org   04h                      ;Supply Monitor Interrupt Vector
    halt                           ;Stop execution if power falls too low

    org   08h                      ;Analog Column 0 Interrupt Vector
    `@INTERRUPT_2`
    reti

    org   0Ch                      ;Analog Column 1 Interrupt Vector
    `@INTERRUPT_3`
    reti

    org   10h                      ;Analog Column 2 Interrupt Vector
    `@INTERRUPT_4`
    reti

    org   14h                      ;Analog Column 3 Interrupt Vector
    `@INTERRUPT_5`
    reti

    org   18h                      ;VC3 Interrupt Vector
    `@INTERRUPT_6`
    reti

    org   1Ch                      ;GPIO Interrupt Vector
    `@INTERRUPT_7`
    reti

    org   20h                      ;PSoC Block DBB00 Interrupt Vector
    `@INTERRUPT_8`
    reti

    org   24h                      ;PSoC Block DBB01 Interrupt Vector
    `@INTERRUPT_9`
    reti

    org   28h                      ;PSoC Block DCB02 Interrupt Vector
    `@INTERRUPT_10`
    reti

    org   2Ch                      ;PSoC Block DCB03 Interrupt Vector
    `@INTERRUPT_11`
    reti

    org   30h                      ;PSoC Block DBB10 Interrupt Vector
    `@INTERRUPT_12`
    reti

    org   34h                      ;PSoC Block DBB11 Interrupt Vector
    `@INTERRUPT_13`
    reti

    org   38h                      ;PSoC Block DCB12 Interrupt Vector
    `@INTERRUPT_14`
    reti

    org   3Ch                      ;PSoC Block DCB13 Interrupt Vector
    `@INTERRUPT_15`
    reti

    org   40h                      ;PSoC Block DBB20 Interrupt Vector
    `@INTERRUPT_16`
    reti

    org   44h                      ;PSoC Block DBB21 Interrupt Vector
    `@INTERRUPT_17`
    reti

    org   48h                      ;PSoC Block DCB22 Interrupt Vector
    `@INTERRUPT_18`
    reti

    org   4Ch                      ;PSoC Block DCB23 Interrupt Vector
    `@INTERRUPT_19`
    reti

    org   50h                      ;PSoC Block DBB30 Interrupt Vector
    `@INTERRUPT_20`
    reti

    org   54h                      ;PSoC Block DBB31 Interrupt Vector
    `@INTERRUPT_21`
    reti

    org   58h                      ;PSoC Block DCB32 Interrupt Vector
    `@INTERRUPT_22`
    reti

    org   5Ch                      ;PSoC Block DCB33 Interrupt Vector
    `@INTERRUPT_23`
    reti

    org   60h                      ;PSoC I2C Interrupt Vector
    `@INTERRUPT_24`
    reti

    org   64h                      ;Sleep Timer Interrupt Vector
    `@INTERRUPT_25`
    reti

;-----------------------------------------------------------------------------
;  Start of Execution.
;-----------------------------------------------------------------------------
;  The Supervisory ROM SWBootReset function has already completed the
;  calibrate1 process, loading trim values for 5 volt operation.
;

IF	(TOOLCHAIN & HITECH)
 	AREA PD_startup(CODE, REL, CON)
ELSE
    org 68h
ENDIF
__Start:

    ; initialize SMP values for voltage stabilization, if required,
    ; leaving power-on reset (POR) level at the default (low) level, at
    ; least for now. 
    ;
    M8C_SetBank1
    mov   reg[VLT_CR], SWITCH_MODE_PUMP_JUST | LVD_TBEN_JUST | TRIP_VOLTAGE_JUST
    M8C_SetBank0

    ; %53%20%46%46% Apply Erratum 001-05137 workaround
    mov   A, 20h
    romx
    mov   A, 40h
    romx
    mov   A, 60h
    romx
    ; %45%20%46%46% End workaround

IF ( WATCHDOG_ENABLE )             ; WDT selected in Global Params
    M8C_EnableWatchDog
ENDIF

IF ( SELECT_32K )
    or   reg[CPU_SCR1],  CPU_SCR1_ECO_ALLOWED  ; ECO will be used in this project
ELSE
    and  reg[CPU_SCR1], ~CPU_SCR1_ECO_ALLOWED  ; Prevent ECO from being enabled
ENDIF

    ;---------------------------
    ; Set up the Temporary stack
    ;---------------------------
    ; A temporary stack is set up for the SSC instructions.
    ; The real stack start will be assigned later.
    ;
_stack_start:          equ 80h
    mov   A, _stack_start          ; Set top of stack to end of used RAM
    swap  SP, A                    ; This is only temporary if going to LMM

    ;-----------------------------------------------
    ; Set Power-related Trim & the AGND Bypass bit.
    ;-----------------------------------------------

IF ( POWER_SETTING & POWER_SET_5V0)            ; *** 5.0 Volt operation   ***
 IF ( POWER_SETTING & POWER_SET_SLOW_IMO)      ; *** 6MHZ Main Oscillator ***
    or  reg[CPU_SCR1], CPU_SCR1_SLIMO
    M8SSC_Set2TableTrims 2, SSCTBL2_TRIM_IMO_5V_6MHZ, 1, SSCTBL1_TRIM_BGR_5V, AGND_BYPASS_JUST
 ELSE                                          ; *** 12MHZ Main Oscillator ***
  IF ( AGND_BYPASS )
    ;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ; The 5V trim has already been set, but we need to update the AGNDBYP
    ; bit in the write-only BDG_TR register. Recalculate the register
    ; value using the proper trim values.
    ;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    M8SSC_SetTableVoltageTrim 1, SSCTBL1_TRIM_BGR_5V, AGND_BYPASS_JUST
  ENDIF
 ENDIF
ENDIF ; 5.0 V Operation

IF ( POWER_SETTING & POWER_SET_3V3)            ; *** 3.3 Volt operation   ***
 IF ( POWER_SETTING & POWER_SET_SLOW_IMO)      ; *** 6MHZ Main Oscillator ***
    or  reg[CPU_SCR1], CPU_SCR1_SLIMO
    M8SSC_Set2TableTrims 2, SSCTBL2_TRIM_IMO_3V_6MHZ, 1, SSCTBL1_TRIM_BGR_3V, AGND_BYPASS_JUST
 ELSE                                          ; *** 12MHZ Main Oscillator ***
    M8SSC_SetTableTrims  1, SSCTBL1_TRIM_IMO_3V_24MHZ, SSCTBL1_TRIM_BGR_3V, AGND_BYPASS_JUST
 ENDIF
ENDIF ; 3.3 Volt Operation

    mov  [bSSC_KEY1],  0           ; Lock out Flash and Supervisiory operations
    mov  [bSSC_KEYSP], 0

    ;---------------------------------------
    ; Initialize Crystal Oscillator and PLL
    ;---------------------------------------

IF ( SELECT_32K & WAIT_FOR_32K )
    ; If the user has requested the External Crystal Oscillator (ECO) then turn it
    ; on and wait for it to stabilize and the system to switch over to it. The PLL
    ; is left off. Set the SleepTimer period is set to 1 sec to time the wait for
    ; the ECO to stabilize.
    ;
    M8C_SetBank1
    mov   reg[OSC_CR0], (SELECT_32K_JUST | OSC_CR0_SLEEP_1Hz | OSC_CR0_CPU_12MHz)
    M8C_SetBank0
    M8C_ClearWDTAndSleep                  ; Reset the sleep timer to get a full second
    or    reg[INT_MSK0], INT_MSK0_SLEEP   ; Enable latching of SleepTimer interrupt
    mov   reg[INT_VC],   0                ; Clear all pending interrupts
.WaitFor1s:
    tst   reg[INT_CLR0], INT_MSK0_SLEEP   ; Test the SleepTimer Interrupt Status
    jz   .WaitFor1s                       ; Interrupt will latch but will not dispatch
                                          ;   since interrupts are not globally enabled
ELSE ; !( SELECT_32K & WAIT_FOR_32K )
    ; Either no ECO, or waiting for stable clock is to be done in main
    M8C_SetBank1
    mov   reg[OSC_CR0], (SELECT_32K_JUST | PLL_MODE_JUST | SLEEP_TIMER_JUST | OSC_CR0_CPU_12MHz)
    M8C_SetBank0
    M8C_ClearWDTAndSleep           ; Reset the watch dog

ENDIF ;( SELECT_32K & WAIT_FOR_32K )

IF ( PLL_MODE )
    ; Crystal is now fully operational (assuming WAIT_FOR_32K was enabled).
    ; Now start up PLL if selected, and wait 16 msec for it to stabilize.
    ;
    M8C_SetBank1
    mov   reg[OSC_CR0], (SELECT_32K_JUST | PLL_MODE_JUST | OSC_CR0_SLEEP_64Hz | OSC_CR0_CPU_3MHz)
    M8C_SetBank0
    M8C_ClearWDTAndSleep                  ; Reset the sleep timer to get full period
    mov   reg[INT_VC], 0                  ; Clear all pending interrupts

.WaitFor16ms:
    tst   reg[INT_CLR0],INT_MSK0_SLEEP    ; Test the SleepTimer Interrupt Status
    jz   .WaitFor16ms
    M8C_SetBank1                          ; continue boot at CPU Speed of SYSCLK/2
    mov   reg[OSC_CR0], (SELECT_32K_JUST | PLL_MODE_JUST | OSC_CR0_SLEEP_64Hz | OSC_CR0_CPU_12MHz)
    M8C_SetBank0

IF      ( WAIT_FOR_32K )
ELSE ; !( WAIT_FOR_32K )
    ; Option settings (PLL-Yes, ECO-No) are incompatible - force a syntax error
    ERROR_PSoC Disabling WAIT_FOR_32K requires that the PLL_Lock must be enabled in user code.
ENDIF ;(WAIT_FOR_32K)
ENDIF ;(PLL_MODE)

    ;------------------------
    ; Close CT leakage path.
    ;------------------------
    mov   reg[ACB00CR0], 05h
    mov   reg[ACB01CR0], 05h
    mov   reg[ACB02CR0], 05h
    mov   reg[ACB03CR0], 05h


IF	(TOOLCHAIN & HITECH)
    ;---------------------------------------------
    ; HI-TECH initialization: Enter the Large Memory Model, if applicable
    ;---------------------------------------------
	global		__Lstackps
	mov     a,low __Lstackps
	swap    a,sp

IF ( SYSTEM_LARGE_MEMORY_MODEL )
    RAM_SETPAGE_STK SYSTEM_STACK_PAGE      ; relocate stack page ...
    RAM_SETPAGE_IDX2STK            ; initialize other page pointers
    RAM_SETPAGE_CUR 0
    RAM_SETPAGE_MVW 0
    RAM_SETPAGE_MVR 0
    IF ( SYSTEM_IDXPG_TRACKS_STK_PP ); Now enable paging:
    or    F, FLAG_PGMODE_11b       ; LMM w/ IndexPage<==>StackPage
    ELSE
    or    F, FLAG_PGMODE_10b       ; LMM w/ independent IndexPage
    ENDIF ;  SYSTEM_IDXPG_TRACKS_STK_PP
ENDIF ;  SYSTEM_LARGE_MEMORY_MODEL
ELSE
    ;---------------------------------------------
    ; ImageCraft Enter the Large Memory Model, if applicable
    ;---------------------------------------------
IF ( SYSTEM_LARGE_MEMORY_MODEL )
    RAM_SETPAGE_STK SYSTEM_STACK_PAGE      ; relocate stack page ...
    mov   A, SYSTEM_STACK_BASE_ADDR        ;   and offset, if any
    swap  A, SP
    RAM_SETPAGE_IDX2STK            ; initialize other page pointers
    RAM_SETPAGE_CUR 0
    RAM_SETPAGE_MVW 0
    RAM_SETPAGE_MVR 0

  IF ( SYSTEM_IDXPG_TRACKS_STK_PP ); Now enable paging:
    or    F, FLAG_PGMODE_11b       ; LMM w/ IndexPage<==>StackPage
  ELSE
    or    F, FLAG_PGMODE_10b       ; LMM w/ independent IndexPage
  ENDIF ;  SYSTEM_IDXPG_TRACKS_STK_PP
ELSE
    mov   A, __ramareas_end        ; Set top of stack to end of used RAM
    swap  SP, A
ENDIF ;  SYSTEM_LARGE_MEMORY_MODEL
ENDIF ;	TOOLCHAIN

    ;-------------------------
    ; Load Base Configuration
    ;-------------------------
    ; Load global parameter settings and load the user modules in the
    ; base configuration. Exceptions: (1) Leave CPU Speed fast as possible
    ; to minimize start up time; (2) We may still need to play with the
    ; Sleep Timer.
    ;
    lcall LoadConfigInit

    ;-----------------------------------
    ; Initialize C Run-Time Environment
    ;-----------------------------------
IF ( C_LANGUAGE_SUPPORT )
IF ( SYSTEM_SMALL_MEMORY_MODEL )
    mov  A,0                           ; clear the 'bss' segment to zero
    mov  [__r0],<__bss_start
BssLoop:
    cmp  [__r0],<__bss_end
    jz   BssDone
    mvi  [__r0],A
    jmp  BssLoop
BssDone:
    mov  A,>__idata_start              ; copy idata to data segment
    mov  X,<__idata_start
    mov  [__r0],<__data_start
IDataLoop:
    cmp  [__r0],<__data_end
    jz   C_RTE_Done
    push A
    romx
    mvi  [__r0],A
    pop  A
    inc  X
    adc  A,0
    jmp  IDataLoop

ENDIF ; SYSTEM_SMALL_MEMORY_MODEL

IF ( SYSTEM_LARGE_MEMORY_MODEL )
    mov   reg[CUR_PP], >__r0           ; force direct addr mode instructions
                                       ; to use the Virtual Register page.

    ; Dereference the constant (flash) pointer pXIData to access the start
    ; of the extended idata area, "xidata." Xidata follows the end of the
    ; text segment and may have been relocated by the Code Compressor.
    ;
    mov   A, >__pXIData                ; Get the address of the flash
    mov   X, <__pXIData                ;   pointer to the xidata area.
    push  A
    romx                               ; get the MSB of xidata's address
    mov   [__r0], A
    pop   A
    inc   X
    adc   A, 0
    romx                               ; get the LSB of xidata's address
    swap  A, X
    mov   A, [__r0]                    ; pXIData (in [A,X]) points to the
                                       ;   XIData structure list in flash
    jmp   .AccessStruct

    ; Unpack one element in the xidata "structure list" that specifies the
    ; values of C variables. Each structure contains 3 member elements.
    ; The first is a pointer to a contiguous block of RAM to be initial-
    ; ized. Blocks are always 255 bytes or less in length and never cross
    ; RAM page boundaries. The list terminates when the MSB of the pointer
    ; contains 0xFF. There are two formats for the struct depending on the
    ; value in the second member element, an unsigned byte:
    ; (1) If the value of the second element is non-zero, it represents
    ; the 'size' of the block of RAM to be initialized. In this case, the
    ; third member of the struct is an array of bytes of length 'size' and
    ; the bytes are copied to the block of RAM.
    ; (2) If the value of the second element is zero, the block of RAM is
    ; to be cleared to zero. In this case, the third member of the struct
    ; is an unsigned byte containing the number of bytes to clear.

.AccessNextStructLoop:
    inc   X                            ; pXIData++
    adc   A, 0
.AccessStruct:                         ; Entry point for first block
    ;
    ; Assert: pXIData in [A,X] points to the beginning of an XIData struct.
    ;
    M8C_ClearWDT                       ; Clear the watchdog for long inits
    push  A
    romx                               ; MSB of RAM addr (CPU.A <- *pXIData)
    mov   reg[MVW_PP], A               ;   for use with MVI write operations
    inc   A                            ; End of Struct List? (MSB==0xFF?)
    jz    .C_RTE_WrapUp                ;   Yes, C runtime environment complete
    pop   A                            ; restore pXIData to [A,X]
    inc   X                            ; pXIData++
    adc   A, 0
    push  A
    romx                               ; LSB of RAM addr (CPU.A <- *pXIData)
    mov   [__r0], A                    ; RAM Addr now in [reg[MVW_PP],[__r0]]
    pop   A                            ; restore pXIData to [A,X]
    inc   X                            ; pXIData++ (point to size)
    adc   A, 0
    push  A
    romx                               ; Get the size (CPU.A <- *pXIData)
    jz    .ClearRAMBlockToZero         ; If Size==0, then go clear RAM
    mov   [__r1], A                    ;             else downcount in __r1
    pop   A                            ; restore pXIData to [A,X]

.CopyNextByteLoop:
    ; For each byte in the structure's array member, copy from flash to RAM.
    ; Assert: pXIData in [A,X] points to previous byte of flash source;
    ;         [reg[MVW_PP],[__r0]] points to next RAM destination;
    ;         __r1 holds a non-zero count of the number of bytes remaining.
    ;
    inc   X                            ; pXIData++ (point to next data byte)
    adc   A, 0
    push  A
    romx                               ; Get the data value (CPU.A <- *pXIData)
    mvi   [__r0], A                    ; Transfer the data to RAM
    tst   [__r0], 0xff                 ; Check for page crossing
    jnz   .CopyLoopTail                ;   No crossing, keep going
    mov   A, reg[ MVW_PP]              ;   If crossing, bump MVW page reg
    inc   A
    mov   reg[ MVW_PP], A
.CopyLoopTail:
    pop   A                            ; restore pXIData to [A,X]
    dec   [__r1]                       ; End of this array in flash?
    jnz   .CopyNextByteLoop            ;   No,  more bytes to copy
    jmp   .AccessNextStructLoop        ;   Yes, initialize another RAM block

.ClearRAMBlockToZero:
    pop   A                            ; restore pXIData to [A,X]
    inc   X                            ; pXIData++ (point to next data byte)
    adc   A, 0
    push  A
    romx                               ; Get the run length (CPU.A <- *pXIData)
    mov   [__r1], A                    ; Initialize downcounter
    mov   A, 0                         ; Initialize source data

.ClearRAMBlockLoop:
    ; Assert: [reg[MVW_PP],[__r0]] points to next RAM destination and
    ;         __r1 holds a non-zero count of the number of bytes remaining.
    ;
    mvi   [__r0], A                    ; Clear a byte
    tst   [__r0], 0xff                 ; Check for page crossing
    jnz   .ClearLoopTail               ;   No crossing, keep going
    mov   A, reg[ MVW_PP]              ;   If crossing, bump MVW page reg
    inc   A
    mov   reg[ MVW_PP], A
    mov   A, 0                         ; Restore the zero used for clearing
.ClearLoopTail:
    dec   [__r1]                       ; Was this the last byte?
    jnz   .ClearRAMBlockLoop           ;   No,  continue
    pop   A                            ;   Yes, restore pXIData to [A,X] and
    jmp   .AccessNextStructLoop        ;        initialize another RAM block

.C_RTE_WrapUp:
    pop   A                            ; balance stack

ENDIF ; SYSTEM_LARGE_MEMORY_MODEL

C_RTE_Done:

ENDIF ; C_LANGUAGE_SUPPORT

    ;-------------------------------
    ; Voltage Stabilization for SMP
    ;-------------------------------

IF ( POWER_SETTING & POWER_SET_5V0)    ; 5.0V Operation
IF ( SWITCH_MODE_PUMP ^ 1 )            ; SMP is operational
    ;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ; When using the SMP at 5V, we must wait for Vdd to slew from 3.1V to
    ; 5V before enabling the Precision Power-On Reset (PPOR).
    ;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    or   reg[INT_MSK0],INT_MSK0_SLEEP
    M8C_SetBank1
    and   reg[OSC_CR0], ~OSC_CR0_SLEEP
    or    reg[OSC_CR0],  OSC_CR0_SLEEP_512Hz
    M8C_SetBank0
    M8C_ClearWDTAndSleep                   ; Restart the sleep timer
    mov   reg[INT_VC], 0                   ; Clear all pending interrupts
.WaitFor2ms:
    tst   reg[INT_CLR0], INT_MSK0_SLEEP    ; Test the SleepTimer Interrupt Status
    jz   .WaitFor2ms                       ; Branch fails when 2 msec has passed
ENDIF ; SMP is operational
ENDIF ; 5.0V Operation

    ;-------------------------------
    ; Set Power-On Reset (POR) Level
    ;-------------------------------
    M8C_SetBank1

IF (POWER_SETTING & POWER_SET_5V0)          ; 5.0V Operation?
 IF (POWER_SETTING & POWER_SET_SLOW_IMO)    ; and Slow Mode?
 ELSE                                       ;    No, fast mode
  IF ( CPU_CLOCK_JUST ^ OSC_CR0_CPU_24MHz ) ;    As fast as 24MHz?
                                            ;       no, set midpoint POR in user code, if desired
  ELSE ; 24HMz                              ;
    or    reg[VLT_CR],  VLT_CR_POR_HIGH     ;      yes, highest POR trip point required
  ENDIF ; 24MHz
 ENDIF ; Slow Mode
ENDIF ; 5.0V Operation

    M8C_SetBank0

    ;----------------------------
    ; Wrap up and invoke "main"
    ;----------------------------

    ; Disable the Sleep interrupt that was used for timing above.  In fact,
    ; no interrupts should be enabled now, so may as well clear the register.
    ;
    mov  reg[INT_MSK0],0

    ; Everything has started OK. Now select requested CPU & sleep frequency.
    ; And put decimator in full mode so it does not consume too much current.
    ;
    M8C_SetBank1
    mov  reg[OSC_CR0],(SELECT_32K_JUST | PLL_MODE_JUST | SLEEP_TIMER_JUST | CPU_CLOCK_JUST)
    or   reg[DEC_CR2],80h                    ; Put decimator in full mode
    M8C_SetBank0

    ; Global Interrupt are NOT enabled, this should be done in main().
    ; LVD is set but will not occur unless Global Interrupts are enabled.
    ; Global Interrupts should be enabled as soon as possible in main().
    ;
    mov  reg[INT_VC],0             ; Clear any pending interrupts which may
                                   ; have been set during the boot process.
IF	(TOOLCHAIN & HITECH)
	ljmp  startup                  ; Jump to C compiler startup code
ELSE
IF ENABLE_LJMP_TO_MAIN
    ljmp  _main                    ; goto main (no return)
ELSE
    lcall _main                    ; call main
.Exit:
    jmp  .Exit                     ; Wait here after return till power-off or reset
ENDIF
ENDIF ; TOOLCHAIN

    ;---------------------------------
    ; Library Access to Global Parms
    ;---------------------------------
    ;
 bGetPowerSetting:
_bGetPowerSetting:
    ; Returns value of POWER_SETTING in the A register.
    ; No inputs. No Side Effects.
    ;
    mov   A, POWER_SETTING
    ret

IF	(TOOLCHAIN & HITECH)
ELSE
    ;---------------------------------
    ; Order Critical RAM & ROM AREAs
    ;---------------------------------
    ;  'TOP' is all that has been defined so far...

    ;  ROM AREAs for C CONST, static & global items
    ;
    AREA lit               (ROM, REL, CON)   ; 'const' definitions
    AREA idata             (ROM, REL, CON)   ; Constants for initializing RAM
__idata_start:

    AREA func_lit          (ROM, REL, CON)   ; Function Pointers
__func_lit_start:

IF ( SYSTEM_LARGE_MEMORY_MODEL )
    ; We use the func_lit area to store a pointer to extended initialized
    ; data (xidata) area that follows the text area. Func_lit isn't
    ; relocated by the code compressor, but the text area may shrink and
    ; that moves xidata around.
    ;
__pXIData:         word __text_end           ; ptr to extended idata
ENDIF

    AREA psoc_config       (ROM, REL, CON)   ; Configuration Load & Unload
    AREA UserModules       (ROM, REL, CON)   ; User Module APIs

    ; CODE segment for general use
    ;
    AREA text (ROM, REL, CON)
__text_start:

    ; RAM area usage
    ;
    AREA data              (RAM, REL, CON)   ; initialized RAM
__data_start:

    AREA virtual_registers (RAM, REL, CON)   ; Temp vars of C compiler
    AREA InterruptRAM      (RAM, REL, CON)   ; Interrupts, on Page 0
    AREA bss               (RAM, REL, CON)   ; general use
__bss_start:

ENDIF ; TOOLCHAIN

; end of file boot.asm

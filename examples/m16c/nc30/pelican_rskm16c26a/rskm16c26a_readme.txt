***********************************************************************************
DESCRIPTION		Renesas Starter Kit Project Readme File
***********************************************************************************

/***********************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
11.04.2006 RTE-VNA Created.
***********************************************************************************/

This project can be debugged with HEW and an E8.
The Quick Start Guide supplied with the RSK can be referred to for help in
connecting to the target. This code has been designed so that the E8 Firmware
Program can be located at address h'FF000 and the E8 Firmware RAM at h'0B00.

Details of the sample project can be found at the top of the C file which contains
the projects main() function.

Support
Online technical support and information is available at: www.renesas.com/rsk

Technical Contact Details:

America: techsupport.rta@renesas.com 
Europe:  tools.support.eu@renesas.com 
Japan:   csc@renesas.com 

RSKM16C26A MCU Port - Board Connector Cross-Reference/*

*********************************************************************************** 
PORT	 	PIN NAME/								RING 		COMMON BUS	
NUMBER		FUNCTION								BUS PIN		PIN	Hardware
***********************************************************************************			

			Vcc - 5V								J1-11		JA1-1
			VCC - 3V											JA1-3
			AVcc									J4-11		JA1-5	
			VSS										J1-4		JA2-4, JA1-2
			AVss									J4-8		JA1-6
			Vref									J4-10		JA1-7
			Res										J1-7		JA2-1

P1_5		INT3/ADTRG/IDV							J3-12	
P1_6		INT4/IDW								J3-11		JA2-23			
P1_7		INT5/IDU								J3-10		JA1-23				SW3

P6_0		CTS0/RTS0								J3-9		JA2-12
P6_1		CLK0									J3-8		JA2-10	
P6_2		RxD0									J3-7		JA2-8	
P6_3		TxD0									J3-6		JA2-6
P6_4		CTS1/RTS1/CTS0/CLKS1					J3-5				-----		E8 BUSY
P6_5		CLK1									J3-4						
P6_6		RxD1									J3-3				-----		E8 RX DATA
P6_7		TxD1									J3-2				-----		E8 TX DATA

P7_0		TxD2/SDA/TA0OUT/CTS1/RTS1/CTS0/CLKS1	J3-1		JA1-25, JA2-19
P7_1		RxD2/SCL/TA0IN/CLK1						J2-12		JA1-26, JA2-21
P7_2		CLK2/TA1OUT/V/RxD1						J2-11		JA2-15
P7_3		CTS2/RTS2/TA1IN/V/TxD1					J2-10		JA2-16
P7_4		TA2OUT/W								J2-9		JA2-17	
P7_5		TA2IN/W									J2-8		JA2-18
P7_6		TA3OUT									J2-7		JA2-20
P7_7		TA3IN									J2-6		JA2-22

P8_0		TA4OUT/U								J2-5		JA2-13
P8_1		TA4IN/U									J2-4		JA2-14
P8_2		INT0									J2-3		JA2-11
P8_3		INT1									J2-2		JA2-7				SW1
P8_4		INT2/ZP									J2-1		JA2-9				SW2
P8_5		NMI/SD									J1-12						
P8_6		XCOUT									J1-6
P8_7		XCIN									J1-5

P9_0		TB0IN/AN30/CLKout						J1-3		JA1-15
P9_1		TB1IN/AN31								J1-2		JA1-16	
P9_2		TB2IN/AN32								J1-1		JA1-17
P9_3		AN24									J4-12		JA1-18				AD_POT

P10_0		AN0										J4-9		JA1-9
P10_1		AN1										J4-7		JA1-10
P10_2		AN2										J4-6		JA1-11
P10_3		AN3										J4-5		JA1-12	
P10_4		AN4/KI0									J4-4		JA1-19, JA2-24		LED0
P10_5		AN5/KI1									J4-3		JA1-20				LED1
P10_6		AN6/KI2									J4-2		JA1-21				LED2
P10_7		AN7/KI3									J4-1		JA1-22				LED3

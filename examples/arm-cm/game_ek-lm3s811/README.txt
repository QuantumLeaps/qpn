About this Example
==================
This directory contains the simple "Flyn 'n' Shoot" game example application
for the EK-LM3S811 board. This directory contains portable code that should
compile with any C compiler.

The sub-directories contain the Board Support Packages (BSPs) for the
preemptive QK-nano kernel (sub-directory qk), the cooperative QV kernel
(sub-directory qv), and the Win32-QV emulation (Windows).

The native kernels (QK-nano and QV-nano) further provide code and project
files for the particular ARM toolchains, such as ARM (MDK-ARM), GNU, and IAR.

Please refer to the README files in the sub-directories for instructions
how to use and customize the example to your needs.


Support Code for EK-LM3S811 Board 
=================================
The directory qpn\3rd_party\ek-lm3d811 contains the CMSIS-compliant
device code for the LM3S811 MCU. Please see the README.txt file in
this folder for more details.


Win32 Emulation 
===============
The sub-directory win32-qv provides the emulation of the example on Windows
as a GUI application. The sub-directory contains the Makefile for the
MinGW toolset and a Visual Studio solution file (game-gui.sln) for Visual C++.

The Win32 emulation uses exactly the same code as the embedded board and
differs only in the Board Support Package (bsp.c). This example demonstrates
the "dual targeting" development approach, where most of the embedded code
is developed on the desktop machine (Windows), but is intended for a deeply
embedded target (EK-LM3S811 here).
About this Example
==================
This directory contains a QP example application for the
EFM32-SLSTK3401A board (Pearl Gecko Starter Kit). This directory
contains portable code that should compile with any C compiler for
ARM Cortex-M.

The sub-directories contain code and project files, which are specific
to the particular ARM toolchains, such as ARM (MDK-ARM), GNU, and IAR.

Please refer to the README files in the sub-directories for specific
instructions how to use and customize the example to your needs.


Support Code for EFM32-SLSTK3401A Board
=======================================
The directory qpc\3rd_party\efm32pg1b contains the CMSIS-compliant
device code for the EFM32PG1B200F256GM48 MCU. Please see the README.txt
file in this folder for more details.


Win32-QV Emulation
==================
The sub-directory win32-qv provides the emulation of the example
on Windows GUI (with the cooperative QV scheduler). This sub-directory
contains the Makefile for the MinGW toolset and Visual Studio solution
file (game-gui.sln) for Visual C++.

The Win32-QV emulation uses exactly the same code as the embedded board and
differ only in the Board Support Package (bsp.c). This example demonstrates
the "dual targeting" development approach, where most of the embedded code
is developed on the desktop machine (Windows), but is intended for a deeply
embedded target (EFM32-SLSTK3401A here).

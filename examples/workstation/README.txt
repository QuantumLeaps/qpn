ABOUT THE "Workstation" EXAMPLES
================================
The examples in the "workstation" directory can be built on any workstation
(Windows, Linux, and MacOS). The provided Makefiles support the
GNU GCC toolchain.

*** NOTE ***
For Windows, the make utility and the GNU GCC toolchain (MinGW) are provided
in the QTools collection, which is available for a separate download from:

http://sourceforge.net/projects/qpc/files/QTools/

*** NOTE ***
The code can be also built with other tools as well, such as the
Microsoft Visual Studio 2013 and newer.


The QP-nano Code
=============
The QP-nano framework is built from sources in every provided project.


Debug and Release Build Configurations
============================================
The Makefiles for the examples generally support the following two build
configurations:

Debug Configuration
-------------------
This is the default build configuration, with full debugging information and
minimal optimization. To build this configuration, type:

make

To clean this build, type

make clean

The object files and the executable is located in the 'build' sub-directory.


Release Configuration
---------------------
This configuration is built with no debugging information and high
optimization. Single-stepping and debugging might be difficult due
to the lack of debugging information and optimized code. To build
this configuration, type:

make CONF=rel

To clean this build, type

make CONF=rel clean

The object files and the executable is located in the 'build_rel' directory.


Support and Contact Information
===============================
https://sourceforge.net/p/qpc/discussion/668726/
https://www.state-machine.com
info@state-machine.com

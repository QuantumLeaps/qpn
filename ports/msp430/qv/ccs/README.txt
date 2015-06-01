About The QP-MSP430 Port
========================
This directory contains the QP-nano port to the MSP430 processor family
with the preemptive QK kernel using the TI CCS-430 compiler.


Using the Port in CCS
=====================
The recommended CCS project setup procedure is illustrated in the provided
CCS example projects in the qpn/examples/msp430/ directory, is to create a
folder for QP-nano source. To do so, right-click on your project and choose 
the New->Folder option.

It is important to create the QP folder as "Linked folder", by expanding
the "Advanced" folder option and selecting the "Linked to alternate
location (Linked Folder)" radio button.

The QP folder should be linked to qpn/source

Once the folders are added to your project, it is very important to exclude
specific source files in the QP folder from the specific build configurations.

Here is the list of QP-nano source files that need to be EXCLUDED from the
bulid configurations:

QK-nano kernel, you need to exclude:
qvn.c

QV-nano kernel, you need to exclude:
qvn.c

Finally, to build QP-nano framework from sources, the following directories
should be present in the compiler "Include Options":
 
<qpn>\include
<qpn>\source
<qpn>\ports\msp430\qk\ccs

Where, <qpn> is either relative or absolute path to the QP-nano framework
folder on your machine.

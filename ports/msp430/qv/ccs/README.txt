About The QP-MSP430 Port
========================
This directory contains the QP-nano port to the MSP430 processor family
with the preemptive QK kernel using the TI CCS-430 compiler.


Using the Port in CCS
=====================
The recommended CCS project setup procedure is illustrated in the provided
CCS example projects in the qpn/examples/msp430/ directory, is to create a
folders for QP-nano source. These folders are "qfn" and "qkn" or "qvn",
depending on which kernel you choose

It is important to create the "qfn" and "qkn" or "qvn" folders as
"Linked folders", by expanding the "Advanced" folder option and selecting
the "Linked to alternate location (Linked Folder)" radio button.

The "qfn" folder should be linked to "qpn\src\qfn"

If you choose the QK-nano kernel, the "qkn" folder should be linked to
"qpn\src\qkn"

If you choose the QV-nano kernel, the "qvn" folder should be linked to
"qpn\src\qvn"

Finally, to build QP-nano framework from sources, the following directories
should be present in the compiler "Include Options":
 
<qpn>\include
<qpn>\ports\msp430\qk\ccs - If you choose the QK-nano kernel,
<qpn>\ports\msp430\qv\ccs - If you choose the QV-nano kernel,

Where, <qpn> is either relative or absolute path to the QP-nano framework
folder on your machine.

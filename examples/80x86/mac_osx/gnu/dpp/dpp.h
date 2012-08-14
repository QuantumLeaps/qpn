/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 05, 2008
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
*****************************************************************************/
#ifndef dpp_h
#define dpp_h

enum DPPSignals {
   EAT_SIG = Q_USER_SIG,     /* published by Table to let a philosopher eat */
   DONE_SIG,                   /* published by Philosopher when done eating */
   TERMINATE_SIG,          /* published by BSP to terminate the application */
   HUNGRY_SIG                    /* posted from hungry Philosopher to Table */
};

enum { N_PHILO = 5 };                             /* number of philosophers */

/* active objects ..........................................................*/
#define PHILO_0_PRIO    1

extern struct PhiloTag  AO_Philo0;
extern struct PhiloTag  AO_Philo1;
extern struct PhiloTag  AO_Philo2;
extern struct PhiloTag  AO_Philo3;
extern struct PhiloTag  AO_Philo4;
extern struct TableTag  AO_Table;

void Philo_ctor(void);                           /* instantiates all Philos */
void Table_ctor(void);

#endif                                                             /* dpp_h */

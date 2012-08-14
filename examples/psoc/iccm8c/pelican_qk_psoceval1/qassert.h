/*****************************************************************************
* Product:  QP-nano
* Last Updated for Version: 4.0.04
* Date of the Last Update:  Feb 27, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
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
#ifndef qassert_h
#define qassert_h

/**
* \file
* \ingroup qepn qfn
* \brief Customizable assertions.
*
* Defines customizable and memory-efficient assertions applicable to
* embedded systems. This header file can be used in C, C++, and mixed C/C++
* programs.
*
* \note The preprocessor switch Q_NASSERT disables checking assertions.
* In particular macros \ref Q_ASSERT, \ref Q_REQUIRE, \ref Q_ENSURE,
* \ref Q_INVARIANT, and \ref Q_ERROR do NOT evaluate the test condition
* passed as the argument to these macros. One notable exception is the
* macro \ref Q_ALLEGE, that still evaluates the test condition, but does
* not report assertion failures when the switch Q_NASSERT is defined.
*/
#ifdef Q_NASSERT          /* Q_NASSERT defined--assertion checking disabled */

    #define Q_DEFINE_THIS_FILE
    #define Q_DEFINE_THIS_MODULE(name_)
    #define Q_ASSERT(test_)    ((void)0)
    #define Q_ALLEGE(test_)    ((void)(test_))
    #define Q_ERROR()          ((void)0)

#else                  /* Q_NASSERT not defined--assertion checking enabled */

    #ifdef __cplusplus
        extern "C" {
    #endif

    /** callback invoked in case the condition passed to \ref Q_ASSERT,
    * \ref Q_REQUIRE, \ref Q_ENSURE, \ref Q_ERROR, or \ref Q_ALLEGE
    * evaluates to FALSE.
    *
    * \param file file name where the assertion failed
    * \param line line number at which the assertion failed
    */
    /*lint -sem(Q_onAssert, r_no)                Q_onAssert() never returns */
    void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line);

    #ifdef __cplusplus
        }
    #endif

    /** Place this macro at the top of each C/C++ module to define the file
    * name string using __FILE__ (NOTE: __FILE__ might contain lengthy path
    * name). This file name will be used in reporting assertions in this file.
    */
    #define Q_DEFINE_THIS_FILE \
        static char const Q_ROM Q_ROM_VAR l_this_file[] = __FILE__;

    /** Place this macro at the top of each C/C++ module to define the module
    * name as the argument \a name_. This file name will be used in reporting
    * assertions in this file.
    */
    #define Q_DEFINE_THIS_MODULE(name_) \
        static char const Q_ROM Q_ROM_VAR l_this_file[] = #name_;

    /** General purpose assertion that makes sure the \a test_ argument is
    * TRUE. Calls the Q_onAssert() callback if the \a test_ evaluates
    * to FALSE.
    * \note the \a test_ is NOT evaluated if assertions are
    * disabled with the Q_NASSERT switch.
    */
    #define Q_ASSERT(test_) \
        if (test_) { \
        } \
        else (Q_onAssert(l_this_file, __LINE__))

    /** General purpose assertion that ALWAYS evaluates the \a test_
    * argument and calls the Q_onAssert() callback if the \a test_
    * evaluates to FALSE.
    * \note the \a test_ argument IS always evaluated even when assertions are
    * disabled with the Q_NASSERT macro. When the Q_NASSERT macro is
    * defined, the Q_onAssert() callback is NOT called, even if the
    * \a test_ evaluates to FALSE.
    */
    #define Q_ALLEGE(test_)    Q_ASSERT(test_)

    /** Assertion that always calls the Q_onAssert() callback if
    * ever executed.
    * \note can be disabled with the Q_NASSERT switch.
    */
    #define Q_ERROR() \
        (Q_onAssert(l_this_file, __LINE__))

#endif                                                         /* Q_NASSERT */

/** Assertion that checks for a precondition. This macro is equivalent to
* \ref Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
*/
#define Q_REQUIRE(test_)   Q_ASSERT(test_)

/** Assertion that checks for a postcondition. This macro is equivalent to
* \ref Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
*/
#define Q_ENSURE(test_)    Q_ASSERT(test_)

/** Assertion that checks for an invariant. This macro is equivalent to
* \ref Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
*/
#define Q_INVARIANT(test_) Q_ASSERT(test_)

/** Compile-time assertion exploits the fact that in C/C++ a dimension of
* an array cannot be negative. The following declaration causes a compilation
* error if the compile-time expression (\a test_) is not TRUE. The assertion
* has no runtime side effects.
*/
#define Q_ASSERT_COMPILE(test_) \
    extern char Q_assert_compile[(test_) ? 1 : -1]

#endif                                                         /* qassert_h */

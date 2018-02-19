/*!
* @file
* @ingroup qepn qfn qkn qvn
* macros for porting QP-nano, and command-line macros
*/

/*! The preprocessor switch to disable checking assertions */
/**
* @description
* When defined, Q_NASSERT disables the following macros #Q_ASSERT,
* #Q_REQUIRE, #Q_ENSURE, #Q_INVARIANT, #Q_ERROR as well as
* #Q_ASSERT_ID, #Q_REQUIRE_ID, #Q_ENSURE_ID, #Q_INVARIANT_ID, and
* #Q_ERROR_ID do NOT evaluate the test condition passed as the
* argument to these macros.
*
* @note One notable exception is the macro #Q_ALLEGE, that still
* evaluates the test condition, but does not report assertion
* failures when the switch #Q_NASSERT is defined.
*/
#define Q_NASSERT

/*! @deprecated macro to  enable keeping track of QF time event usage;
* instead use: #QF_TIMEEVT_USAGE */
#define Q_TIMERSET

/*! This macro enables calling the QK-nano context-switch callback
* QK_onContextSw() */
#define QK_ON_CONTEXT_SW

/*! Macro that should be defined (typically on the compiler's command line)
in the Win32-GUI applications that use the @ref win32-qv port.
*/
#define WIN32_GUI

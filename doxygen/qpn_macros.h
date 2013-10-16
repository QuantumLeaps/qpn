/**
* \file
* \ingroup qepn qfn qkn
* macros for porting QP-nano, and command-line macros
*/

/** \brief The preprocessor switch to disable checking assertions
*
* When defined, Q_NASSERT disables the following macros #Q_ASSERT,
* #Q_REQUIRE, #Q_ENSURE, #Q_INVARIANT, #Q_ERROR as well as
* #Q_ASSERT_ID, #Q_REQUIRE_ID, #Q_ENSURE_ID, #Q_INVARIANT_ID, and
* #Q_ERROR_ID do NOT evaluate the test condition passed as the
* argument to these macros.
*
* \note One notable exception is the macro #Q_ALLEGE, that still
* evaluates the test condition, but does not report assertion
* failures when the switch Q_NASSERT is defined.
*/
#define Q_NASSERT


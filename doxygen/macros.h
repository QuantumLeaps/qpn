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

/*! The preprocessor switch to eliminate the QMsm-style state machine
* implementation.
*/
/**
* @description
* When defined, this macro will eliminate the following functions:
* QMsm_ctor(), QMsm_stateObj(), QMsm_init_(), and QMsm_dispatch_(),
* as well as QMActive_ctor().
*
* @note The macro will not eliminate the declaration of the QMsm class,
* because this is a base class for derivation of QHsm and QActive.
*
* @sa #Q_NHSM
*/
#define Q_NMSM

/*! The preprocessor switch to eliminate the QHsm-style state machine
* implementation.
*/
/**
* @description
* When defined, this macro will suppress the declaration of the QHsm
* data type and will eliminate the following functions:
* QHsm_ctor(), QHsm_state(), QHsm_top(), QHsm_init_(), and QHsm_dispatch_(),
* as well as QActive_ctor().
*
* @sa #Q_NMSM
*/
#define Q_NHSM

/*! The preprocessor switch to eliminate the "transition-to-history" feature
* in QMsm-style state machines.
*/
#define Q_NMSM_HIST

/*! @deprecated macro to  enable keeping track of QF time event usage;
* instead use: #QF_TIMEEVT_USAGE */
#define Q_TIMERSET
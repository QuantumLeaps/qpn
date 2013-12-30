/*****************************************************************************
* Product: QEP-nano
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 30, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qepn_h
#define qepn_h

/**
* \file
* \ingroup qepn qfn qkn
* \brief Public QEP-nano interface.
*
* This header file must be included in all modules that use QP-nano.
* Typically, this header file is included indirectly through the
* header file qpn_port.h.
*/

/****************************************************************************/
/** \brief The current QP version number
*
* version of the QP as a decimal constant XYZ, where X is a 1-digit
* major version number, Y is a 1-digit minor version number, and Z is
* a 1-digit release number.
*/
#define QP_VERSION      520

/** \brief The current QP version string */
#define QP_VERSION_STR  "5.2.0"

/** \brief Tamperproof current QP release (5.2.0) and date (13-12-30) */
#define QP_RELEASE      0xB1C7E217U


/****************************************************************************/
#ifndef Q_ROM             /* if NOT defined, provide the default definition */

    /** \brief Macro to specify compiler-specific directive for placing a
    * constant object in ROM.
    *
    * Many compilers for 8-bit Harvard-architecture MCUs provide non-standard
    * extensions to support placement of objects in different memories.
    * In order to conserve the precious RAM, QP-nano uses the Q_ROM macro for
    * all constant objects that can be allocated in ROM.
    *
    * To override the following empty definition, you need to define the
    * Q_ROM macro in the qpn_port.h header file. Some examples of valid
    * Q_ROM macro definitions are: __code (IAR 8051 compiler), code (Keil
    * 8051 compiler), PROGMEM (gcc for AVR), __flash (IAR for AVR).
    */
    #define Q_ROM
#endif

/****************************************************************************/
/** helper macro to calculate static dimension of a 1-dim array \a array_ */
#define Q_DIM(array_) (sizeof(array_) / sizeof(array_[0]))

/****************************************************************************/
/* typedefs for basic numerical types; MISRA-C 2004 rule 6.3(req).          */
/** \brief typedef for character strings.
*
* This typedef specifies character type for exclusive use in character
* strings. Use of this type, rather than plain 'char', is in compliance
* with the MISRA-C 2004 Rules 6.1(req), 6.3(adv).
*/
typedef char char_t;

/** typedef for 32-bit IEEE 754 floating point numbers  */
typedef float float32_t;

/** typedef for 64-bit IEEE 754 floating point numbers  */
typedef double float64_t;

/** \brief Scalar type describing the signal of an event.
*/
typedef uint8_t QSignal;

/** typedef for enumerations used for event signals */
typedef int enum_t;

/** typedef for ints used for line numbers */
typedef int int_t;

/** typedef for temporary variables, like fast loop counters */
typedef unsigned int uint_t;

#ifndef Q_PARAM_SIZE
    /** \brief macro to define the size of event parameter.
    * Valid values 0, 1, 2, or 4; default 0
    */
    #define Q_PARAM_SIZE 0
#endif
#if (Q_PARAM_SIZE == 0)
#elif (Q_PARAM_SIZE == 1)

    /** \brief type of the event parameter.
    *
    * This typedef is configurable via the preprocessor switch #Q_PARAM_SIZE.
    * The other possible values of this type are as follows: \n
    * none when (Q_PARAM_SIZE == 0); \n
    * uint8_t when (Q_PARAM_SIZE == 1); \n
    * uint16_t when (Q_PARAM_SIZE == 2); and \n
    * uint32_t when (Q_PARAM_SIZE == 4).
    */
    typedef uint8_t QParam;
#elif (Q_PARAM_SIZE == 2)
    typedef uint16_t QParam;
#elif (Q_PARAM_SIZE == 4)
    typedef uint32_t QParam;
#else
    #error "Q_PARAM_SIZE defined incorrectly, expected 0, 1, 2, or 4"
#endif

/** \brief Event structure.
*
* QEvt represents events, optionally with a single scalar parameter.
* \sa Q_PARAM_SIZE
* \sa ::QParam
*/
typedef struct QEvtTag {
    QSignal sig;                                   /**< signal of the event */
#if (Q_PARAM_SIZE != 0)
    QParam par;                          /**< scalar parameter of the event */
#endif
} QEvt;

/****************************************************************************/

/** \brief Type returned from  a state-handler function. */
typedef uint8_t QState;

                       /** \brief the signature of a state handler function */
typedef QState (*QStateHandler)(void * const me);

                      /** \brief the signature of a action handler function */
typedef QState (*QActionHandler)(void * const me);

/** \brief State object for the QMsm class (Meta State Machine).
*
* This class groups together the attributes of a QMsm state, such as the
* parent state (state nesting), the associated state handler function and
* the exit action handler function.
*
* \note The QMStateObj class is only intended for the QM code generator
* and should not be used in hand-crafted code.
*/
typedef struct QMStateTag {
    struct QMStateTag const *parent;      /**< parent state (state nesting) */
    QStateHandler     const stateHandler;      /**<  state handler function */
    QActionHandler    const exitAction;   /**< exit action handler function */
} QMState;

/** \brief Attribute of for the QMsm class (Meta State Machine).
*
* This union represents possible values stored in the 'state' and 'temp'
* attributes of the QMsm class.
*/
typedef union QMAttrTag {
    QMState        const *obj;               /**< pointer to QMState object */
    QActionHandler const *act;                /**< array of action handlers */
    QStateHandler  fun;            /**< pointer to a state handler function */
} QMAttr;

/** \brief macro to access the signal of the current event of a state machine
*
* \sa ::QMsm ::QFsm ::QHsm
*/
#define Q_SIG(me_)  (((QMsm *)(me_))->evt.sig)

#if (Q_PARAM_SIZE != 0)
/** \brief macro to access the parameter of the current event of
* a state machine
*
* \sa ::QFsm ::QHsm Q_PARAM_SIZE
*/
#define Q_PAR(me_)  (((QMsm *)(me_))->evt.par)
#endif                                               /* (Q_PARAM_SIZE != 0) */


/****************************************************************************/
typedef struct QMsmVtblTag QMsmVtbl;     /**< declaration (needed for QMsm) */

/** \brief Meta State Machine
*
* QMsm represents the most fundamental State Machine in QP. The application-
* level state machines derived directly from QMsm typically require the use
* of the QM modeling tool, but are the fastest and need the least run-time
* support (the smallest event-processor taking up the least code space).
*
* QMsm is also the base class for the QFsm and QHsm state machines, which
* can be coded and maintained by hand (as well as by QM), but aren't as fast
* and require significantly more run-time code (0.5-1KB) to execute.
*
* \note QMsm is not intended to be instantiated directly, but rather serves
* as the base structure for derivation of state machines in the application
* code.
*
* The following example illustrates how to derive a state machine structure
* from QMsm. Please note that the QMsm member 'super' is defined as the
* _first_ member of the derived struct.
* \include qepn_qmsm.c
*
* \sa \ref derivation
*/
typedef struct QMsmTag {
    QMsmVtbl const *vptr;                       /**< \brief virtual pointer */
    QMAttr state;         /**< \brief current active state (state-variable) */
    QMAttr temp;     /**< \brief temporary: tran. chain, target state, etc. */
    QEvt evt;   /**< \brief currently processed event in the SM (protected) */
} QMsm;

/** \brief Virtual table for the QMsm class
*
* The QMsm class provides two virtual functions: init() and dispatch().
*/
struct QMsmVtblTag {
    /** \brief virtual function to initialize a state machine (take the
    * top-most initial transition.
    */
    void (*init)(QMsm * const me);

    /** \brief virtual function to dispatch an event to a state machine.
    */
    void (*dispatch)(QMsm * const me);
};

/** \brief Performs the second step of QMsm initialization by triggering
* the top-most initial transition.
*
* \param me_ pointer the state machine structure derived from QMsm.
* \note Must be called only ONCE after the "constructor" QMsm_ctor().
*
* The following example illustrates how to initialize a FSM, and dispatch
* events to it:
* \include qepn_qmsm_use.c
*/
#define QMSM_INIT(me_) ((*(me_)->vptr->init)((me_)))

/** \brief Dispatches an event to a state machine
*
* Processes one event at a time in Run-to-Completion fashion. The argument
* \a me is the pointer the state machine structure derived from QMsm.
*
* \note Must be called after the "constructor" QMsm_ctor() and QMsm_init_().
*
* \sa example for QMsm_init_() \n \ref derivation
*/
#define QMSM_DISPATCH(me_) ((*(me_)->vptr->dispatch)((me_)))

#ifndef Q_NMSM

/** \brief Protected "constructor" of a QM SM.
*
* Performs the first step of QMsm initialization by assigning the initial
* pseudostate to the currently active state of the state machine.
*
* \note Must be called only by the "constructors" of the derived state
* machines.
* \note Must be called only ONCE before QMsm_init_().
*/
void QMsm_ctor(QMsm * const me, QStateHandler initial);

/** \brief Implementation of the top-most initial transition in QMsm. */
void QMsm_init_(QMsm * const me);

/** \brief Implementation of disparching events to QMsm. */
void QMsm_dispatch_(QMsm * const me);

/** \brief Empty action used in transition tables without any
* entry/exit/initial actions
*/
extern QActionHandler const QMsm_emptyAction_[1];

#endif                                                            /* Q_NMSM */


/****************************************************************************/
#ifndef Q_NFSM

/** \brief Finite State Machine
*
* QFsm represents a traditional non-hierarchical Finite State Machine (FSM)
* without state hierarchy, but with entry/exit actions. QFsm inherits QMsm
* "as is" without adding new attributes, so it is typedef'ed as QMsm.
*
* \note QFsm is not intended to be instantiated directly, but rather serves
* as the base structure for derivation of state machines in the application
* code.
*
* The following example illustrates how to derive a state machine structure
* from QFsm. Please note that the QFsm member super is defined as the FIRST
* member of the derived struct.
* \include qepn_qfsm.c
*
* \sa \ref derivation
*/
typedef QMsm QFsm;

/** \brief Protected "constructor" of a FSM.
*
* Performs the first step of FSM initialization by assigning the
* initial pseudostate to the currently active state of the state machine.
* \note Must be called only by the "constructors" of the derived state
* machines.
* \note Must be called only ONCE before QFsm_init_().
*
* The following example illustrates how to invoke QFsm_ctor() in the
* "constructor" of a derived state machine:
* \include qepn_qfsm_ctor.c
*/
void QFsm_ctor(QFsm * const me, QStateHandler initial);

/** \brief Implementation of the top-most initial transition in QFsm. */
void QFsm_init_(QFsm * const me);

/** \brief Implementation of disparching events to QFsm. */
void QFsm_dispatch_(QFsm * const me);

#endif                                                            /* Q_NFSM */


/****************************************************************************/
#ifndef Q_NHSM

/** \brief Hierarchical State Machine
*
* QHsm represents a Hierarchical Finite State Machine (HSM) with full
* support for hierarchical nesting of states, entry/exit actions,
* and initial transitions in any composite state. QHsm inherits QMsm
* "as is" without adding new attributes, so it is typedef'ed as QMsm.
*
* \note QHsm is not intended to be instantiated directly, but rather serves
* as the base structure for derivation of state machines in the application
* code.
*
* The following example illustrates how to derive a state machine structure
* from QHsm. Please note that the QHsm member super is defined as the FIRST
* member of the derived struct.
* \include qepn_qhsm.c
*
* \sa \ref derivation
*/
typedef QMsm QHsm;

/* public methods */

/** \brief protected "constructor" of a HSM.
* Performs the first step of HSM initialization by assigning the
* initial pseudostate to the currently active state of the state machine.
*
* \note Must be called only by the "constructors" of the derived state
* machines.
* \note Must be called before QHsm_init_().
*
* The following example illustrates how to invoke QHsm_ctor() in the
* "constructor" of a derived state machine:
* \include qepn_qhsm_ctor.c
*/
void QHsm_ctor(QHsm * const me, QStateHandler initial);

/** \brief Obtain the current active state from a HSM (read only).
*/
#define QHsm_state(me_) (Q_STATE_CAST((me_)->state.fun))

/** \brief Implementation of the top-most initial transition in QHsm.
*/
void QHsm_init_(QHsm * const me);

/** \brief Implementation of dispatching events to QHsm. */
void QHsm_dispatch_(QHsm * const me);

/** \brief Tests if a given state is part of the current active state
* configuration
*
* \param me is the pointer the state machine structure derived from ::QHsm.
* \param state is a pointer to the state handler function, e.g., &QCalc_on.
*/
uint8_t QHsm_isIn(QHsm * const me, QStateHandler const state);

/* protected methods */

/** \brief the top-state.
*
* QHsm_top() is the ultimate root of state hierarchy in all HSMs derived
* from ::QHsm. This state handler always returns (QState)0, which means
* that it "handles" all events.
*
* \sa Example of the QCalc_on() state handler for Q_INIT().
*/
QState QHsm_top(void const * const me);

#endif                                                            /* Q_NHSM */

/****************************************************************************/

/** \brief All possible values returned from state/action handlers */
enum QHandlerReturnValues {
    Q_RET_SUPER,       /**< \brief event passed to the superstate to handle */
    Q_RET_HANDLED,     /**< \brief event handled (internal transition)      */
    Q_RET_UNHANDLED,   /**< \brief event unhandled                          */
    Q_RET_IGNORED,     /**< \brief event ignored (bubbled up to top)        */
    Q_RET_TRAN,        /**< \brief regular transition taken                 */
    Q_RET_ENTRY,       /**< \brief entry action executed                    */
    Q_RET_EXIT,        /**< \brief exit action executed                     */
    Q_RET_INITIAL      /**< \brief initial transition taken                 */
};


/** \brief Perform cast to QStateHandler.
*
* This macro encapsulates the cast of a specific state handler function
* pointer to QStateHandler, which violates MISRA-C 2004 rule 11.4(advisory).
* This macro helps to localize this deviation.
*/
#define Q_STATE_CAST(handler_)  ((QStateHandler)(handler_))

/** \brief Perform cast to QActionHandler.
*
* This macro encapsulates the cast of a specific action handler function
* pointer to QActionHandler, which violates MISRA-C 2004 rule 11.4(advisory).
* This macro helps to localize this deviation.
*/
#define Q_ACTION_CAST(action_)  ((QActionHandler)(action_))


/** \brief Macro to call in a QM action-handler when it executes
* an entry action. Applicable only to QMSMs.
*/
#define QM_ENTRY(dummy) ((QState)Q_RET_ENTRY)

/** \brief Macro to call in a QM action-handler when it executes
* an exit action. Applicable only to QMSMs.
*/
#define QM_EXIT(dummy)  ((QState)Q_RET_EXIT)

/** \brief Macro to call in a QM state-handler when it executes a regular
* transition. Applicable only to QMSMs.
*/
#define QM_TRAN(target_, act_) \
    (((((QMsm *)me)->state.obj = (target_)), \
      (((QMsm *)me)->temp.act = (act_))), (QState)Q_RET_TRAN)

/** \brief Macro to call in a QM state-handler when it executes an initial
* transition. Applicable only to QMSMs.
*/
#define QM_INITIAL(target_, act_) \
    (((((QMsm *)me)->state.obj = (target_)), \
      (((QMsm *)me)->temp.act = (act_))), Q_RET_INITIAL)

/** \brief Macro to call in a QM state-handler when it handled an event.
* Applicable only to QMSMs.
*/
#define QM_HANDLED()     ((QState)Q_RET_HANDLED)

/** \brief Macro to call in a QM state-handler when when it attempts to
* handle an event but a guard condition evaluates to false and there is
* no other explicit way of handling the event. Applicable only to QMSMs.
*/
#define QM_UNHANDLED()   ((QState)Q_RET_UNHANDLED)
/** \brief Macro to call in a QM state-handler when it designates the
* superstate to handle an event. Applicable only to QMSMs.
*/
#define QM_SUPER()       ((QState)Q_RET_SUPER)


/** \brief Macro to call in a state-handler when it executes a regular
* or and initial transition. Applicable to both HSMs and FSMs.
* \include qepn_qtran.c
*/
#define Q_TRAN(target_)  \
    (((QMsm *)me)->temp.fun = Q_STATE_CAST(target_), (QState)Q_RET_TRAN)

/** \brief Macro to call in a state-handler when it designates the
* superstate of a given state. Applicable only to HSMs.
* \include qepn_qhsm.c
*/
#define Q_SUPER(super_)  \
    (((QMsm *)me)->temp.fun = Q_STATE_CAST(super_),  (QState)Q_RET_SUPER)

/** \brief Macro to call in a state-handler when it handles an event.
*  Applicable to both HSMs and FSMs.
* \include qepn_qfsm.c
*/
#define Q_HANDLED()      ((QState)Q_RET_HANDLED)

/** \brief Macro to call in a state-handler when it attempts to handle
* an event but a guard condition evaluates to false and there is no other
* explicit way of handling the event. Applicable only to HSMs.
*/
#define Q_UNHANDLED()    ((QState)Q_RET_UNHANDLED)

/** \brief Macro to
 call in a non-hierarchical state-handler when it
* ignores (does not handle) an event. Applicable only to FSMs.
* \include qepn_qfsm.c
*/
#define Q_IGNORED()      ((QState)Q_RET_IGNORED)

/** \brief QP reserved signals */
enum QReservedSignals {
    Q_ENTRY_SIG = 1,                   /**< signal for coding entry actions */
    Q_EXIT_SIG,                         /**< signal for coding exit actions */
    Q_INIT_SIG,           /**< signal for coding nested initial transitions */
    Q_TIMEOUT_SIG, /**< signal used by time events at the default tick rate */
    Q_TIMEOUT1_SIG,          /**< signal used by time events at tick rate 1 */
    Q_TIMEOUT2_SIG,          /**< signal used by time events at tick rate 2 */
    Q_TIMEOUT3_SIG,          /**< signal used by time events at tick rate 3 */
    Q_USER_SIG      /**< first signal that can be used in user applications */
};

/** \brief Perform cast from unsigned integer \a uint_ to pointer
* of type \a type_.
*
* This macro encapsulates the cast to (type_ *), which QP ports or
* application might use to access embedded hardware registers.
* Such uses can trigger PC-Lint "Note 923: cast from int to pointer" and
* this macro helps to encapsulate this deviation.
*/
#define Q_UINT2PTR_CAST(type_, uint_)  ((type_ *)(uint_))


/****************************************************************************/
/** \brief obtain the current QP-nano version number string
*
* version of QP-nano as a constant 5-character string of the form X.Y.Z,
* where X is a 1-digit major version number, Y is a 1-digit minor
* version number, and Z is a 1-digit release number.
*/
#define QP_getVersion() (QP_VERSION_STR)

/****************************************************************************/
#include "qassert.h"            /* include the QP-nano assertions (for DbC) */

/****************************************************************************/
/* QP API compatibility layer */
#ifndef QP_API_VERSION

/** \brief Macro that specifies the backwards compatibility with the
* QP-nano API version.
*
* For example, QP_API_VERSION=450 will cause generating the compatibility
* layer with QP-nano version 4.5.0 and newer, but not older than 4.5.0.
* QP_API_VERSION=0 causes generation of the compatibility layer "from the
* begining of time", which is the maximum backwards compatibilty. This is
* the default.
*
* Conversely, QP_API_VERSION=9999 means that no compatibility layer should
* be generated. This setting is useful for checking if an application
* complies with the latest QP-nano API.
*/
#define QP_API_VERSION 0
#endif                                            /* #ifndef QP_API_VERSION */

/*..........................................................................*/
#if (QP_API_VERSION < 500)

/** \brief Deprecated macro for odd 8-bit CPUs. */
#define Q_ROM_VAR

/** \brief Deprecated macro for odd 8-bit CPUs. */
#define Q_REENTRANT

/** \brief Deprecated API defined for backwards-compatibility */
#define QMsm_init(me_)      QMSM_INIT((me_))

/** \brief Deprecated API defined for backwards-compatibility */
#define QMsm_dispatch(me_)  QMSM_DISPATCH((me_))

/** \brief Deprecated API defined for backwards-compatibility */
#define QHsm_init(me_)      QMSM_INIT((me_))

/** \brief Deprecated API defined for backwards-compatibility */
#define QHsm_dispatch(me_)  QMSM_DISPATCH((me_))

/** \brief Deprecated API defined for backwards-compatibility */
#define QFsm_init(me_)      QMSM_INIT((me_))

/** \brief Deprecated API defined for backwards-compatibility */
#define QFsm_dispatch(me_)  QMSM_DISPATCH((me_))

/*..........................................................................*/
#if (QP_API_VERSION < 450)

/** \brief deprecated typedef for backwards compatibility */
typedef QEvt QEvent;

#endif                                              /* QP_API_VERSION < 450 */
#endif                                              /* QP_API_VERSION < 500 */
/****************************************************************************/

#endif                                                            /* qepn_h */


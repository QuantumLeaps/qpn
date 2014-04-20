/**
* \file
* \brief Public QEP-nano interface.
* \ingroup qepn
* \cond
******************************************************************************
* Product: QEP-nano
* Last updated for version 5.3.0
* Last updated on  2014-04-18
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
******************************************************************************
* \endcond
*/
#ifndef qepn_h
#define qepn_h

/**
* \description
* This header file must be included in all modules that use QEP-nano.
* Typically, this header file is included indirectly through the
* header file qpn_port.h.
*/

/****************************************************************************/
/*! The current QP version number */
/**
* \description
* version of the QP as a decimal constant XYZ, where X is a 1-digit
* major version number, Y is a 1-digit minor version number, and Z is
* a 1-digit release number.
*/
#define QP_VERSION      530

/*! The current QP version string */
#define QP_VERSION_STR  "5.3.0"

/*! Tamperproof current QP-nano release (5.3.0) and date (14-04-18) */
#define QP_RELEASE      0xAC4DE7CDU

/****************************************************************************/
/*! obtain the current QP-nano version number string */
/**
* \description
* version of QP-nano as a constant 5-character string of the form X.Y.Z,
* where X is a 1-digit major version number, Y is a 1-digit minor
* version number, and Z is a 1-digit release number.
*/
#define QP_getVersion() (QP_VERSION_STR)

/****************************************************************************/
/* typedefs for basic numerical types; MISRA-C 2004 rule 6.3(req). */

/*! typedef for character strings. */
/**
* \description
* This typedef specifies character type for exclusive use in character
* strings. Use of this type, rather than plain 'char', is in compliance
* with the MISRA-C 2004 Rules 6.1(req), 6.3(adv).
*/
typedef char char_t;

/*! typedef for line numbers in assertions and return from QF_run() */
typedef int int_t;

/*! typedef for enumerations used for event signals */
typedef int enum_t;

/*! IEEE 754 32-bit floating point number, MISRA-C 2004 rule 6.3(req) */
/**
* \note QP-nano does not use floating-point types anywhere in the internal
* implementation.
*/
typedef float float32_t;

/*! IEEE 754 64-bit floating point number, MISRA-C 2004 rule 6.3(req) */
/**
* \note QP-nano does not use floating-point types anywhere in the internal
* implementation.
*/
typedef double float64_t;

/*! Scalar type describing the signal of an event. */
typedef uint8_t QSignal;


#ifndef Q_PARAM_SIZE
    /*! The size of event parameter Valid values 0, 1, 2, or 4; default 0 */
    #define Q_PARAM_SIZE 0
#endif
#if (Q_PARAM_SIZE == 0)
#elif (Q_PARAM_SIZE == 1)

    /*! type of the event parameter. */
    /**
    * \description
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

/*! Event structure. */
/**
* \description
* ::QEvt represents events, optionally with a single scalar parameter.
* \sa Q_PARAM_SIZE
* \sa ::QParam
*/
typedef struct {
    QSignal sig; /*!< signal of the event */
#if (Q_PARAM_SIZE != 0)
    QParam par;  /*!< scalar parameter of the event */
#endif
} QEvt;

/****************************************************************************/
/*! Type returned from  a state-handler function. */
typedef uint_fast8_t QState;

/*! the signature of a state handler function */
typedef QState (*QStateHandler)(void * const me);

/*! the signature of a action handler function */
typedef QState (*QActionHandler)(void * const me);

/*! State object for the ::QMsm class (Meta State Machine). */
/**
* \description
* This class groups together the attributes of a ::QMsm state, such as the
* parent state (state nesting), the associated state handler function and
* the exit action handler function. These attributes are used inside the
* QMsm_dispatch() and QMsm_init() functions.
*
* \attention
* The ::QMState class is only intended for the QM code generator and should
* not be used in hand-crafted code.
*/
typedef struct QMState {
    struct QMState const *superstate;   /*!< the superstate */
    QStateHandler  const stateHandler;  /*!< state handler function */
    QActionHandler const entryAction;   /*!< entry action handler function */
    QActionHandler const exitAction;    /*!< exit action handler function */
    QActionHandler const initAction;    /*!< init action handler function */
} QMState;

/*! Transition-Action Table for the Meta State Machine. */
typedef struct {
    QMState        const *target;
    QActionHandler const act[1];
} QMTranActTable;

/*! Attribute of for the ::QMsm class (Meta State Machine). */
/**
* \description
* This union represents possible values stored in the 'state' and 'temp'
* attributes of the ::QMsm class.
*/
union QMAttr {
    QMState        const *obj;   /*!< pointer to QMState object */
    QMTranActTable const *tatbl; /*!< transition-action table */
    QStateHandler  fun;          /*!< pointer to a state-handler function */
    QActionHandler act;          /*!< pointer to an action-handler function */
};

/****************************************************************************/
/*! Macro to access the signal of the current event of a state machine */
/**
* \arguments
* \arg[in,out] \c me_ pointer to a subclass of ::QMsm (see \ref derivation)
*/
#define Q_SIG(me_)  (((QMsm *)(me_))->evt.sig)

#if (Q_PARAM_SIZE != 0)
/*! Macro to access the parameter of the current event of a state machine */
/**
* \arguments
* \arg[in,out] \c me_ pointer to a subclass of ::QMsm (see \ref derivation)
*/
#define Q_PAR(me_)  (((QMsm *)(me_))->evt.par)
#endif  /* (Q_PARAM_SIZE != 0) */


/****************************************************************************/
/*! virtual table for the ::QMsm class. */
typedef struct QMsmVtbl QMsmVtbl;

/*! Meta State Machine */
/**
* \description
* QMsm represents the most fundamental State Machine in QP. The application-
* level state machines derived directly from QMsm typically require the use
* of the QM modeling tool, but are the fastest and need the least run-time
* support (the smallest event-processor taking up the least code space).\n
* \n
* QMsm is also the base class for the QFsm and QHsm state machines, which
* can be coded and maintained by hand (as well as by QM), but aren't as fast
* and require significantly more run-time code (0.5-1KB) to execute.
*
* \note QMsm is not intended to be instantiated directly, but rather serves
* as the base structure for derivation of state machines in the application
* code.
*
* \usage
* The following example illustrates how to derive a state machine structure
* from QMsm. Please note that the QMsm member 'super' is defined as the
* _first_ member of the derived struct.
* \include qepn_qmsm.c
*
* \sa \ref derivation
*/
typedef struct {
    QMsmVtbl const *vptr; /*!< virtual pointer */
    union QMAttr state;   /*!< current active state (state-variable) */
    union QMAttr temp;    /*!< temporary: tran. chain, target state, etc. */
    QEvt evt;  /*!< currently processed event in the SM (protected) */
} QMsm;

/*! Virtual table for the QMsm class */
struct QMsmVtbl {
    /*! Triggers the top-most initial transition in a SM. */
    void (*init)(QMsm * const me);

    /*! Dispatches an event to a SM. */
    void (*dispatch)(QMsm * const me);
};

/*! Polymorphically executes the top-most initial transition in a SM. */
/**
* \arguments
* \arg[in,out] \c me_ pointer (see \ref derivation)
*
* \note Must be called only ONCE after the SM "constructor".
*
* \usage
* The following example illustrates how to initialize a SM, and dispatch
* events to it:
* \include qep_qmsm_use.c
*/
#define QMSM_INIT(me_) ((*(me_)->vptr->init)((me_)))

/*! Polymorphically dispatches an event to a SM. */
/**
* \description
* Processes one event at a time in Run-to-Completion fashion.
*
* \arguments
* \arg[in,out] \c me_ pointer (see \ref derivation)
*
* \note Must be called after the "constructor" and after QMSM_INIT().
*/
#define QMSM_DISPATCH(me_) ((*(me_)->vptr->dispatch)((me_)))

#ifndef Q_NMSM

/*! Protected "constructor" of a ::QMsm */
void QMsm_ctor(QMsm * const me, QStateHandler initial);

/*! Obtain the current active state from a MSM (read only). */
#define QMsm_stateObj(me_) (Q_MSM_UPCAST(me_)->state.obj)

/*! Implementation of the top-most initial transition in QMsm. */
void QMsm_init_(QMsm * const me);

/*! Implementation of disparching events to QMsm. */
void QMsm_dispatch_(QMsm * const me);

#endif  /* Q_NMSM */


/****************************************************************************/
/****************************************************************************/
#ifndef Q_NHSM

/*! Hierarchical State Machine */
/**
* \description
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
*/
typedef QMsm QHsm;

/* public methods */

/*! protected "constructor" of a HSM. */
void QHsm_ctor(QHsm * const me, QStateHandler initial);

/*! Obtain the current active state from a HSM (read only). */
/**
* \arguments
* \arg[in] \c me_ pointer (see \ref derivation)
* \returns the current active state of a HSM
*/
#define QHsm_state(me_) (Q_STATE_CAST(Q_MSM_UPCAST(me_)->state.fun))

/*! Implementation of the top-most initial transition in QHsm. */
void QHsm_init_(QHsm * const me);

/*! Implementation of dispatching events to QHsm. */
void QHsm_dispatch_(QHsm * const me);

/*! the top-state. */
QState QHsm_top(void const * const me);

#endif  /* Q_NHSM */


/****************************************************************************/
/****************************************************************************/
#ifndef Q_NFSM

/*! Non-hierarchical Finite State Machine */
/**
* \description
* QFsm represents a traditional non-hierarchical Finite State Machine (FSM)
* without state hierarchy, but with entry/exit actions. QFsm inherits QMsm
* "as is" without adding new attributes, so it is typedef'ed as QMsm.
*
* \note QFsm is not intended to be instantiated directly, but rather serves
* as the base structure for derivation of state machines in the application
* code.
*
* \usage
* The following example illustrates how to derive a state machine structure
* from QFsm. Please note that the QFsm member super is defined as the FIRST
* member of the derived struct.
* \include qepn_qfsm.c
*/
typedef QMsm QFsm;

/*! Protected "constructor" of a FSM. */
void QFsm_ctor(QFsm * const me, QStateHandler initial);

/*! Implementation of the top-most initial transition in QFsm. */
void QFsm_init_(QFsm * const me);

/*! Implementation of disparching events to QFsm. */
void QFsm_dispatch_(QFsm * const me);

#endif  /* Q_NFSM */


/****************************************************************************/

/*! All possible values returned from state/action handlers */
enum {
    /* unhandled and need to "bubble up" */
    Q_RET_SUPER,     /*!< event passed to superstate to handle */
    Q_RET_SUPER_SUB, /*!< event passed to submachine superstate */
    Q_RET_UNHANDLED, /*!< event unhandled due to a guard */

    /* handled and do not need to "bubble up" */
    Q_RET_HANDLED,   /*!< event handled (internal transition) */
    Q_RET_IGNORED,   /*!< event silently ignored (bubbled up to top) */

    /* entry/exit */
    Q_RET_ENTRY,     /*!< state entry action executed */
    Q_RET_EXIT,      /*!< state exit  action executed */

    /* no side effects */
    Q_RET_NULL,      /*!< return value without any effect */

    /* transitions need to execute transition-action table in QMsm */
    Q_RET_TRAN,      /*!< event handled (regular transition) */
    Q_RET_TRAN_INIT, /*!< initial transition in a state or submachine */
    Q_RET_TRAN_HIST  /*!< event handled (transition to history) */
};

/*! Perform upcast from a subclass of ::QMsm to the base class ::QMsm */
/**
* \description
* Upcasting from a subclass to superclass is a very frequent and __safe__
* operation in object-oriented programming and object-oriented languages
* (such as C++) perform such upcasting automatically. However, OOP is
* implemented in C just as a set of coding conventions (see \ref derivation),
* and the C compiler does not "know" that certain types are related by
* inheritance. Therefore for C, the upcast must be performed explicitly.
* Unfortunately, pointer casting violates the advisory MISRA-C 2004 rule 11.4
* "cast pointer to pointer". This macro encapsulates this deviation and
* provides a descriptive name for the reason of this cast.
*/
#define Q_MSM_UPCAST(ptr_) ((QMsm *)(ptr_))

/*! Perform cast to ::QStateHandler. */
/**
* \description
* This macro encapsulates the cast of a specific state handler function
* pointer to QStateHandler, which violates MISRA-C 2004 rule 11.4(advisory).
* This macro helps to localize this deviation.
*
* \usage
* \include qep_qhsm_ctor.c
*/
#define Q_STATE_CAST(handler_)  ((QStateHandler)(handler_))

/*! Perform cast to QActionHandler. */
/**
* \description
* This macro encapsulates the cast of a specific action handler function
* pointer to ::QActionHandler, which violates MISRA-C 2004 rule 11.4(adv).
* This macro helps to localize this deviation.
*/
#define Q_ACTION_CAST(action_)  ((QActionHandler)(action_))

/*! Macro to call in a QM action-handler when it executes
* an entry action. Applicable only to QMSMs.
*/
#define QM_ENTRY(dummy)         ((QState)Q_RET_ENTRY)

/*! Macro to call in a QM action-handler when it executes
* an exit action. Applicable only to QMSMs.
*/
#define QM_EXIT(dummy)          ((QState)Q_RET_EXIT)

/*! Macro to call in a QM state-handler when it executes a regular
* transition. Applicable only to QMSMs.
*/
#define QM_TRAN(tatbl_) \
    ((Q_MSM_UPCAST(me))->temp.tatbl = (QMTranActTable const *)(tatbl_), \
        (QState)Q_RET_TRAN)

/*! Macro to call in a QM state-handler when it executes an initial
* transition. Applicable only to QMSMs.
*/
#define QM_TRAN_INIT(tatbl_) \
    ((Q_MSM_UPCAST(me))->temp.tatbl = (QMTranActTable const *)(tatbl_), \
        (QState)Q_RET_TRAN_INIT)

/*! Macro to call in a QM state-handler when it executes a transition
* to history. Applicable only to QMSMs.
*/
#define QM_TRAN_HIST(history_, tatbl_) \
    ((((Q_MSM_UPCAST(me))->state.obj = (history_)), \
      ((Q_MSM_UPCAST(me))->temp.tatbl = (QMTranActTable const *)(tatbl_))), \
       (QState)Q_RET_TRAN_HIST)

/*! Macro to call in a QM state-handler when it handled an event.
* Applicable only to QMSMs.
*/
#define QM_HANDLED()     ((QState)Q_RET_HANDLED)

/*! Macro to call in a QM state-handler when when it attempts to
* handle an event but a guard condition evaluates to 'false' and there is
* no other explicit way of handling the event. Applicable only to QMSMs.
*/
#define QM_UNHANDLED()   ((QState)Q_RET_UNHANDLED)

/*! Macro to call in a QM state-handler when it designates the
* superstate to handle an event. Applicable only to QMSMs.
*/
#define QM_SUPER()       ((QState)Q_RET_SUPER)

/*! Macro to call in a state-handler when it executes a regular
* or and initial transition. Applicable to both HSMs and FSMs.
* \include qep_qtran.c
*/
#define Q_TRAN(target_)  \
    ((Q_MSM_UPCAST(me))->temp.fun = Q_STATE_CAST(target_), (QState)Q_RET_TRAN)

/*! Macro to call in a state-handler when it executes a transition
* to history. Applicable only to HSMs.
*
* \usage
* \include qepn_qhist.c
*/
#define Q_TRAN_HIST(hist_)  \
    ((Q_MSM_UPCAST(me))->temp.fun = (hist_), (QState)Q_RET_TRAN_HIST)

/*! Macro to call in a state-handler when it designates the
* superstate of a given state. Applicable only to HSMs.
*
* \usage
* \include qepn_qtran.c
*/
#define Q_SUPER(super_)  \
    ((Q_MSM_UPCAST(me))->temp.fun = Q_STATE_CAST(super_), (QState)Q_RET_SUPER)

/*! Macro to call in a state-handler when it handles an event.
*  Applicable to both HSMs and FSMs.
*/
#define Q_HANDLED()      ((QState)Q_RET_HANDLED)

/*! Macro to call in a state-handler when it attempts to handle
* an event but a guard condition evaluates to 'false' and there is no other
* explicit way of handling the event. Applicable only to HSMs.
*/
#define Q_UNHANDLED()    ((QState)Q_RET_UNHANDLED)

/*! Macro to call in a non-hierarchical state-handler when it
* ignores (does not handle) an event. Applicable only to FSMs.
*/
#define Q_IGNORED()      ((QState)Q_RET_IGNORED)


/*! QP reserved signals */
enum {
    Q_ENTRY_SIG = 1,  /*!< signal for coding entry actions */
    Q_EXIT_SIG,       /*!< signal for coding exit actions */
    Q_INIT_SIG,       /*!< signal for coding nested initial transitions */
    Q_TIMEOUT_SIG,    /*!< timeout signal at the default tick rate 0 */
    Q_TIMEOUT1_SIG,   /*!< timeout signal at tick rate 1 */
    Q_TIMEOUT2_SIG,   /*!< timeout signal at tick rate 2 */
    Q_TIMEOUT3_SIG,   /*!< timeout signal at tick rate 3 */
    Q_USER_SIG        /*!< first signal for the user applications */
};

/*! Perform cast from unsigned integer to a pointer of type \a type_ */
/**
* \description
* This macro encapsulates the cast to (type_ *), which QP ports or
* application might use to access embedded hardware registers.
* Such uses can trigger PC-Lint "Note 923: cast from int to pointer" and
* this macro helps to encapsulate this deviation.
*/
#define Q_UINT2PTR_CAST(type_, uint_)  ((type_ *)(uint_))

/****************************************************************************/
/* macros for accessing data in ROM */
#ifndef Q_ROM /* if NOT defined, provide the default definition */

    /*! Macro to specify compiler-specific directive for placing a
    * constant object in ROM. */
    /**
    * \description
    * Many compilers for 8-bit Harvard-architecture MCUs provide non-standard
    * extensions to support placement of objects in different memories.
    * In order to conserve the precious RAM, QP-nano uses the Q_ROM macro for
    * all constant objects that can be allocated in ROM.
    *
    * \note
    * To override the following empty definition, you need to define the
    * Q_ROM macro in the qpn_port.h header file. Some examples of valid
    * Q_ROM macro definitions are: __code (IAR 8051 compiler), code (Keil
    * 8051 compiler), PROGMEM (gcc for AVR), __flash (IAR for AVR).
    */
    #define Q_ROM
#endif

#ifndef Q_ROM_BYTE
    /*! Macro to access a byte allocated in ROM */
    /**
    * Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
    * not generate correct code for accessing data allocated in the program
    * space (ROM). The workaround for such compilers is to explicitly add
    * assembly code to access each data element allocated in the program
    * space. The macro Q_ROM_BYTE() retrieves a byte from the given ROM
    * address.
    *
    * \note
    * The Q_ROM_BYTE() macro should be defined in the qpn_port.h header file
    * for each compiler that cannot handle correctly data allocated in ROM
    * (such as the gcc). If the macro is left undefined, the default
    * definition simply returns the argument and lets the compiler synthesize
    * the correct code.
    */
    #define Q_ROM_BYTE(rom_var_)   (rom_var_)
#endif
#ifndef Q_ROM_PTR
    /*! Macro to access a pointer allocated in ROM */
    /**
    * Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
    * not generate correct code for accessing data allocated in the program
    * space (ROM). The workaround for such compilers is to explicitly add
    * assembly code to access each data element allocated in the program
    * space. The macro Q_ROM_PTR() retrieves an object-pointer from the given
    * ROM address. Please note that the pointer can be pointing to the object
    * in RAM or ROM.
    *
    * \note
    * The Q_ROM_PTR() macro should be defined in the qpn_port.h header file
    * for each compiler that cannot handle correctly data allocated in ROM
    * (such as the gcc). If the macro is left undefined, the default
    * definition simply returns the argument and lets the compiler synthesize
    * the correct code.
    */
    #define Q_ROM_PTR(rom_var_)    (rom_var_)
#endif


/****************************************************************************/
/*! Helper macro to calculate static dimension of a 1-dim array \a array_ */
#define Q_DIM(array_) (sizeof(array_) / sizeof((array_)[0]))


/****************************************************************************/
/****************************************************************************/
/* QP API compatibility layer */
#ifndef QP_API_VERSION

/*! Macro that specifies the backwards compatibility with the
* QP-nano API version.
*/
/**
* \description
* For example, QP_API_VERSION=450 will cause generating the compatibility
* layer with QP-nano version 4.5.0 and newer, but not older than 4.5.0.
* QP_API_VERSION=0 causes generation of the compatibility layer "from the
* begining of time", which is the maximum backwards compatibilty. This is
* the default.\n
* \n
* Conversely, QP_API_VERSION=9999 means that no compatibility layer should
* be generated. This setting is useful for checking if an application
* complies with the latest QP-nano API.
*/
#define QP_API_VERSION 0

#endif  /* QP_API_VERSION */

/****************************************************************************/
#if (QP_API_VERSION < 500)

/*! \deprecated macro for odd 8-bit CPUs. */
#define Q_ROM_VAR

/*! \deprecated macro for odd 8-bit CPUs. */
#define Q_REENTRANT

/*! \deprecated API defined for backwards-compatibility */
#define QMsm_init(me_)      QMSM_INIT((me_))

/*! \deprecated API defined for backwards-compatibility */
#define QMsm_dispatch(me_)  QMSM_DISPATCH((me_))

/*! \deprecated API defined for backwards-compatibility */
#define QHsm_init(me_)      QMSM_INIT((me_))

/*! \deprecated API defined for backwards-compatibility */
#define QHsm_dispatch(me_)  QMSM_DISPATCH((me_))

/*! \deprecated API defined for backwards-compatibility */
#define QFsm_init(me_)      QMSM_INIT((me_))

/*! \deprecated API defined for backwards-compatibility */
#define QFsm_dispatch(me_)  QMSM_DISPATCH((me_))

/****************************************************************************/
#if (QP_API_VERSION < 450)

/*! \deprecated typedef for backwards compatibility */
typedef QEvt QEvent;

#endif /* QP_API_VERSION < 450 */
#endif /* QP_API_VERSION < 500 */
/****************************************************************************/

#endif /* qepn_h */


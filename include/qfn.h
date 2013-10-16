/*****************************************************************************
* Product: QP-nano
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 14, 2013
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
#ifndef qfn_h
#define qfn_h

/**
* \file
* \ingroup qepn qfn qkn
* \brief Public QF-nano interface.
*
* This header file must be included in all modules that use QP-nano.
* Typically, this header file is included indirectly through the
* header file qpn_port.h.
*/

/****************************************************************************/
#if (QF_MAX_ACTIVE < 1) || (8 < QF_MAX_ACTIVE)
    #error "QF_MAX_ACTIVE not defined or out of range. Valid range is 1..8."
#endif


/****************************************************************************/
/* default macros for accessing data in ROM */
#ifndef Q_ROM_BYTE
    /** \brief Macro to access a byte allocated in ROM
    *
    * Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
    * not generate correct code for accessing data allocated in the program
    * space (ROM). The workaround for such compilers is to explicitly add
    * assembly code to access each data element allocated in the program
    * space. The macro Q_ROM_BYTE() retrieves a byte from the given ROM
    * address.
    *
    * The Q_ROM_BYTE() macro should be defined in the qpn_port.h header file
    * for each compiler that cannot handle correctly data allocated in ROM
    * (such as the gcc). If the macro is left undefined, the default
    * definition simply returns the argument and lets the compiler synthesize
    * the correct code.
    */
    #define Q_ROM_BYTE(rom_var_)   (rom_var_)
#endif
#ifndef Q_ROM_PTR
    /** \brief Macro to access a pointer allocated in ROM
    *
    * Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
    * not generate correct code for accessing data allocated in the program
    * space (ROM). The workaround for such compilers is to explicitly add
    * assembly code to access each data element allocated in the program
    * space. The macro Q_ROM_PTR() retrieves an object-pointer from the given
    * ROM address. Please note that the pointer can be pointing to the object
    * in RAM or ROM.
    *
    * The Q_ROM_PTR() macro should be defined in the qpn_port.h header file
    * for each compiler that cannot handle correctly data allocated in ROM
    * (such as the gcc). If the macro is left undefined, the default
    * definition simply returns the argument and lets the compiler synthesize
    * the correct code.
    */
    #define Q_ROM_PTR(rom_var_)    (rom_var_)
#endif

#ifndef QF_TIMEEVT_CTR_SIZE
    /** \brief macro to override the default QTimeEvtCtr size.
    * Valid values 0, 1, 2, or 4; default 0
    */
    #define QF_TIMEEVT_CTR_SIZE 0
#endif
#if (QF_TIMEEVT_CTR_SIZE == 0)
#elif (QF_TIMEEVT_CTR_SIZE == 1)
    typedef uint8_t QTimeEvtCtr;
#elif (QF_TIMEEVT_CTR_SIZE == 2)
    /** \brief type of the Time Event counter, which determines the dynamic
    * range of the time delays measured in clock ticks.
    *
    * This typedef is configurable via the preprocessor switch
    * #QF_TIMEEVT_CTR_SIZE. The other possible values of this type are
    * as follows: \n
    * none when (QF_TIMEEVT_CTR_SIZE not defined or == 0), \n
    * uint8_t when (QF_TIMEEVT_CTR_SIZE == 1); \n
    * uint16_t when (QF_TIMEEVT_CTR_SIZE == 2); and \n
    * uint32_t when (QF_TIMEEVT_CTR_SIZE == 4).
    */
    typedef uint16_t QTimeEvtCtr;
#elif (QF_TIMEEVT_CTR_SIZE == 4)
    typedef uint32_t QTimeEvtCtr;
#else
    #error "QF_TIMER_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

#ifndef QF_MAX_TICK_RATE
    /** \brief Default value of the macro configurable value in qpn_port.h
    */
    #define QF_MAX_TICK_RATE     1
#elif (QF_MAX_TICK_RATE > 4)
    #error "QF_MAX_TICK_RATE exceeds the 4 limit"
#endif

/****************************************************************************/
/** \brief Active Object struct
*
* QActive is the base structure for derivation of active objects. Active
* objects in QF-nano are encapsulated tasks (each embedding a state machine
* and an event queue) that communicate with one another asynchronously by
* sending and receiving events. Within an active object, events are
* processed sequentially in a run-to-completion (RTC) fashion, while QF
* encapsulates all the details of thread-safe event exchange and queuing.
*
* \note ::QActive is not intended to be instantiated directly, but rather
* serves as the base structure for derivation of active objects in the
* application code.
*
* The following example illustrates how to derive an active object from
* QActive. Please note that the QActive member super_ is defined as the
* FIRST member of the derived struct.
* \include qfn_qactive.c
*
* \sa ::QActiveTag for the description of the data members \n \ref derivation
*/
typedef struct QActiveTag {
    QMsm super;                     /**< derives from the ::QMsm base class */

#if (QF_TIMEEVT_CTR_SIZE != 0)
    /** \brief Time Event tick counter(s) for the active object
    */
    QTimeEvtCtr tickCtr[QF_MAX_TICK_RATE];
#endif

    /** \brief priority of the active object (1..QF_MAX_ACTIVE)
    */
    uint8_t prio;

    /** \brief offset to where next event will be inserted into the buffer
    */
    uint8_t volatile head;

    /** \brief offset of where next event will be extracted from the buffer
    */
    uint8_t volatile tail;

    /** \brief number of events currently present in the queue
    * (events in the ring buffer + 1 event in the state machine)
    */
    uint8_t volatile nUsed;

} QActive;

/** \brief Virtual table for the QActive class
*/
typedef struct QActiveVtblTag {
    QMsmVtbl super;                                  /**< inherits QMsmVtbl */

#if (Q_PARAM_SIZE != 0)
    /** virtual function to asynchronously post (FIFO) an event to an AO
    * from an active object context (task context).
    */
    uint8_t (*post)(QActive * const me, uint8_t const margin,
                    enum_t const sig, QParam const par);

    /** virtual function to asynchronously post (FIFO) an event to an AO
    * from an interrupt context (ISR context).
    */
    uint8_t (*postISR)(QActive * const me, uint8_t const margin,
                       enum_t const sig, QParam const par);
#else
    uint8_t (*post)    (QActive * const me, uint8_t const margin,
                        enum_t const sig);
    uint8_t (*postISR) (QActive * const me, uint8_t const margin,
                        enum_t const sig);
#endif
} QActiveVtbl;

/** \brief Active object constructor.
*
* \a me pointer the active object structure derived from ::QActive.
* \a initial is the pointer to the initial state of the active object.
*
* \note Must be called exactly ONCE for each active object
* in the application before calling QF_run().
*/
void QActive_ctor(QActive * const me, QStateHandler initial);

#if (Q_PARAM_SIZE != 0)
    /** \brief Polymorphically posts an event \a e directly to the event
    * queue of the active object \a prio using the FIFO policy. This
    * function briefly disables and enables interrupts to protect the
    * queue integrity.
    *
    * Direct event posting is the only asynchronous communication method
    * available in QF-nano. The following example illustrates how the
    * Ped active object posts directly the PED_WAITING event to the PELICAN
    * crossing active object.
    * \include qfn_post.c
    *
    * The producer of the event (Ped in this case) must only "know"
    * a pointer recipient (&AO_Pelican), but the specific definition of
    * the Pelican structure is not required.
    *
    * Direct event posting should not be confused with direct event
    * dispatching. In contrast to asynchronous event posting through event
    * queues, direct event dispatching is synchronous. Direct event
    * dispatching occurs when you call QHsm_dispatch(), or QFsm_dispatch()
    * function.
    *
    * \note This function is intended only to be used at the task level
    * and should never be used inside ISRs.
    *
    * \sa #QACTIVE_POST_X, QActive_postX().
    */
    #define QACTIVE_POST(me_, sig_, par_) \
        ((void)(*((QActiveVtbl const *)((me_)->super.vptr))->post)((me_), \
                                        (uint8_t)0, (sig_), (par_)))

    /** \brief Polymorphically posts an event to an active object (FIFO)
    * without delivery guarantee from the task context.
    *
    * The macro takes the argument \a margin_, which specifies the
    * minimum free slots in the queue, which must still be available
    * after posting the event. The macro returns 1 (TRUE) if the posting
    * succeeded, and 0 (FALSE) if the posting failed due to insufficient
    * margin of free slots available in the queue.
    *
    * \sa #QACTIVE_POST, QActive_postX().
    * \include qfn_postx.c
    */
    #define QACTIVE_POST_X(me_, margin_, sig_, par_) \
        ((*((QActiveVtbl const *)((me_)->super.vptr))->post)((me_), \
                                  (margin_), (sig_), (par_)))

    /** \brief Posts an event \a e directly to the event queue of the acitve
    * object \a prio using the First-In-First-Out (FIFO) policy. This
    * function does NOT disable/enable interrupts when nesting of interrupts
    * is not allowed. Also, this function never calls the QK-nano scheduler,
    * because synchronous task preemption is never necessary inside ISRs.
    *
    * \note This function is intended only to be used inside ISRs and you
    * should never use at the task level.
    */
    #define QACTIVE_POST_ISR(me_, sig_, par_) \
        ((void)(*((QActiveVtbl const *)((me_)->super.vptr))->postISR)((me_), \
                                        (uint8_t)0, (sig_), (par_)))

    /** \brief Polymorphically posts an event to an active object (FIFO)
    * without delivery guarantee from the ISR context.
    *
    * The macro takes the argument \a margin_, which specifies the
    * minimum free slots in the queue, which must still be available
    * after posting the event. The macro returns 1 (TRUE) if the posting
    * succeeded, and 0 (FALSE) if the posting failed due to insufficient
    * margin of free slots available in the queue.
    *
    * \sa #QACTIVE_POST, QActive_postX().
    * \include qfn_postx.c
    */
    #define QACTIVE_POST_X_ISR(me_, margin_, sig_, par_) \
        ((*((QActiveVtbl const *)((me_)->super.vptr))->postISR)((me_), \
                                  (margin_), (sig_), (par_)))

    /** \brief Implementation of the task-level posting */
    uint8_t QActive_postX(QActive * const me, uint8_t margin,
                          enum_t const sig, QParam const par);

    /** \brief Implementation of the ISR-level posting */
    uint8_t QActive_postXISR(QActive * const me, uint8_t margin,
                             enum_t const sig, QParam const par);

#else                                                 /* no event parameter */
    #define QACTIVE_POST(me_, sig_) \
        ((void)(*((QActiveVtbl const *)((me_)->super.vptr))->post)((me_), \
                                        (uint8_t)0, (sig_)))

    #define QACTIVE_POST_X(me_, margin_, sig_) \
        ((*((QActiveVtbl const *)((me_)->super.vptr))->post)((me_), \
                                  (margin_), (sig_)))

    uint8_t QActive_postX(QActive * const me, uint8_t margin,
                          enum_t const sig);

    #define QACTIVE_POST_ISR(me_, sig_) \
        ((void)(*((QActiveVtbl const*)((me_)->super.vptr))->postISR)((me_),\
                                       (uint8_t)0, (sig_)))

    #define QACTIVE_POST_X_ISR(me_, margin_, sig_) \
        ((*((QActiveVtbl const *)((me_)->super.vptr))->postISR)((me_), \
                                  (margin_), (sig_)))

    uint8_t QActive_postXISR(QActive * const me, uint8_t margin,
                             enum_t const sig);
#endif

#if (QF_TIMEEVT_CTR_SIZE != 0)

    /** \brief Processes all armed time events at every clock tick.
    *
    * \note This function can be only called from the ISR-level. You must also
    * guarantee that QF_tickXISR() runs to completion before it is called
    * again.
    *
    * The following example illustrates the call to QF_tickXISR():
    * \include qfn_tickx.c
    */
    void QF_tickXISR(uint8_t const tickRate);

    /** \brief Arm the QP-nano one-shot time event. Since the tick counter
    * is a multi-byte variable in this case, the operation must be
    * performed inside a critical section.
    *
    * Arms the time event of the active object \a me to expire in
    * \a tout clock ticks (one-shot time event). Upon the expiration,
    * the time event posts the reserved signal Q_TIMEOUT_SIG directly
    * into the event queue of the active object \a me.
    *
    * After posting, the time event gets automatically disarmed.
    *
    * The time event can be disarmed (stopped) at any time by calling the
    * QActive_disarm() function. Also, a one-shot time event can be re-armed
    * to fire in a different number of clock ticks by calling
    * QActive_armX() again.
    *
    * The following example shows how to arm a one-shot time event from
    * a state machine of an active object:
    * \include qfn_arm.c
    */
    void QActive_armX(QActive * const me, uint8_t const tickRate,
                      QTimeEvtCtr const ticks);

    /** \brief Disarm a time event. Since the tick counter
    * is a multi-byte variable in this case, the operation must be
    * performed inside a critical section.
    *
    * The time event of the active object \a me gets disarmed (stopped).
    *
    * \note You should not assume that the Q_TIMEOUT_SIG event will not
    * arrive after you disarm the time event. The timeout event could be
    * already in the event queue.
    */
    void QActive_disarmX(QActive * const me, uint8_t const tickRate);

#endif                                        /* (QF_TIMEEVT_CTR_SIZE != 0) */

/****************************************************************************/

#ifndef Q_NMSM
/** \brief QM Active Object
*
* QMActive represents an active object version for the QM modeling tool.
* The application-level active object derived from QMActive typically
* require the use of QM, but are the fastest and need the least run-time
* support (the smallest event-processor taking up the least code space).
* QMActive inherits QActive "as is" without adding new attributes, so it
* is typedef'ed as QActive.
*
* \sa \ref derivation
*/
typedef QActive QMActive;

/** \brief protected "constructor" of an QMActive active object.
* Performs the first step of active object initialization by assigning the
* initial pseudostate to the currently active state of the state machine.
*
* \note Must be called only by the "constructors" of the derived active
* objects, as shown in the following example:
*
* \note Must be called before QActive_start().
*/
void QMActive_ctor(QMActive * const me, QStateHandler initial);

#endif                                                            /* Q_NMSM */


/****************************************************************************/
/* QF-nano protected methods ...*/

/** \brief QF-nano initialization.
*
* This function initializes the internal QF variables as well as all
* registered active objects to zero. In the C startup code compliant with
* the C Standard this clearing of internal variables is unnecessary, because
* all static variables are supposed to be cleared in the startup code.
* However in non-compliant implementations calling QF_init() can be very
* handy.
*
* \note Function QF_init() is defined in the separate module qf_init.c, which
* needs to be included in the build only if the non-standard initialization
* is required.
*/
void QF_init(void);

/** \brief QF-nano termination.
*
* This function terminates QF and performs any necessary cleanup.
* In QF-nano this function is defined in the BSP. Many QF ports might not
* require implementing QF_stop() at all, because many embedded applications
* don't have anything to exit to.
*/
void QF_stop(void);

/** \brief Startup QF-nano callback.
*
* The time line for calling QF_onStartup() depends on the particular
* QF port. In most cases, QF_onStartup() is called from QF_run(), right
* before starting any multitasking kernel or the background loop.
*
* \sa QF initialization example for ::QActiveCB.
*/
void QF_onStartup(void);

/** \brief Transfers control to QF to run the application.
*
* QF_run() implements the simple non-preemptive scheduler. QF_run() must be
* called from main() after you initialize the QF and define at least one
* active object control block in QF_active[]. QF_run() typically never
* returns to the caller (except in QP-nano emulations), but when it does,
* it returns the error code (0 for success).
*
*
* \note When the preemptive QK-nano is used as the underlying real-time
* kernel for the QF, all platform dependencies are handled in the QK, so
* no porting of QF is necessary. In other words, you only need to recompile
* the QF platform-independent code with the compiler for your platform, but
* you don't need to provide any platform-specific implementation (so, no
* qf_port.c file is necessary). Moreover, QK implements the function QF_run()
* in a platform-independent way, in the module qk.c.
*/
int16_t QF_run(void);


#ifndef QK_PREEMPTIVE
    /** \brief QF idle callback (customized in BSPs for QF)
    *
    * QF_onIdle() is called by the non-preemptive scheduler built into QF-nano
    * when the QF-nano detects that no events are available for active objects
    * (the idle condition). This callback gives the application an opportunity
    * to enter a power-saving CPU mode, or perform some other idle processing.
    *
    * \note QF_onIdle() is invoked with interrupts DISABLED because the idle
    * condition can be asynchronously changed at any time by an interrupt.
    * QF_onIdle() MUST re-enable the interrupts internally, but not before
    * putting the CPU into the low-power mode. (Ideally, re-enabling interrupts
    * and low-power mode should happen atomically). At the very least, the
    * function MUST unlock interrupts, otherwise interrupts will be disabled
    * permanently.
    *
    * \note QF_onIdle() is not used in the PREEMPTIVE configuration. When
    * QK_PREEMPTIVE macro is defined, the preemptive kernel QK-nano is used
    * instead of the non-preemptive QF-nano scheduler. QK-nano
 uses a
    * different idle callback \sa QK_onIdle().
    */
void QF_onIdle(void);
#endif

/****************************************************************************/
/** \brief QActive Control Block
*
* QActiveCB represents the constant information that the QF-nano needs
* to manage the active object. QActiveCB objects are grouped in the
* array QF_active[], which typically can be placed in ROM.
*
* The following example illustrates how to allocate and initialize the
* QActive control blocks in the array QF_active[].
* \include qfn_main.c
*/
typedef struct QActiveCBTag {
    QActive *act;        /**< \brief pointer to the active object structure */
    QEvt    *queue;           /**< \brief pointer to the event queue buffer */
    uint8_t  end;                 /**< \brief the length of the ring buffer */
} QActiveCB;
                                           /** active object control blocks */
/*lint -save -e960    MISRA-C:2004 8.12, extern array declared without size */
extern QActiveCB const Q_ROM Q_ROM_VAR QF_active[];
/*lint -restore */

/** \brief Ready set of QF-nano.
*
* The QF-nano ready set keeps track of active objects that are ready to run.
* The ready set represents each active object as a bit, with the bits
* assigned according to priorities of the active objects. The bit is set
* if the corresponding active object is ready to run (i.e., has one or
* more events in its event queue) and zero if the event queue is empty.
* The QF-nano ready set is one byte-wide, which corresponds to 8 active
* objects maximum.
*/
extern uint8_t volatile QF_readySet_;

#ifndef QF_LOG2

    /** \brief Lookup table for (log2(n) + 1), where n is the index
    * into the table.
    *
    * This lookup delivers the 1-based number of the most significant 1-bit
    * of a nibble.
    */
    extern uint8_t const Q_ROM Q_ROM_VAR QF_log2Lkup[16];

#endif


#ifdef Q_TIMERSET

    /** \brief Timer set of QF-nano.
    *
    * The QF-nano timer set keeps track of the armed time event set. The
    * timer set represents the timeout down-counter of each active object as
    * a bit, with the bits assigned according to priorities of the active
    * objects. The bit is set if the corresponding timeout down-counter is
    * not zero (i.e., is counting down) and zero if the down-counter is zero.
    * The QF-nano time event set is one byte-wide, which corresponds to 8
    * active objects maximum.
    *
    * The main use of the QF_timerSetX_ is to quickly determine that all time
    * events are disarmed by testing (QF_timerSetX_[tickRate] == (uint8_t)0).
    * If so, the CPU can go to longer sleep mode, in which the system clock
    * tick ISR is turned off.
    *
    * \note The test (QF_timerSet_[tickRate] == (uint8_t)0) must always be
    * performed inside a CRITICAL SECTION.
    */
    extern uint8_t volatile QF_timerSetX_[QF_MAX_TICK_RATE];

#endif                                                        /* Q_TIMERSET */

/****************************************************************************/
/** \brief This macro encapsulates accessing the active object queue at a
* given index, which violates MISRA-C 2004 rules 17.4(req) and 11.4(adv).
* This macro helps to localize this deviation.
*/
#define QF_ROM_QUEUE_AT_(ao_, i_) (((QEvt *)Q_ROM_PTR((ao_)->queue))[(i_)])

/** \brief This macro encapsulates accessing the active object control block,
* which violates MISRA-C 2004 rule 11.4(adv). This macro helps to localize
* this deviation.
*/
#define QF_ROM_ACTIVE_GET_(p_) ((QActive *)Q_ROM_PTR(QF_active[(p_)].act))

/** \brief cast to QActive *.
*
* This macro encapsulates up-casting a pointer to a subclass of QActive
* to the base class QActive, which violates MISRA-C 2004 rule 11.4(advisory).
* This macro helps to localize this deviation.
*/
#define QF_ACTIVE_CAST(a_)     ((QActive *)(a_))

/****************************************************************************/
/* Depricated definitions provided for backwards compatibility */

#if (Q_PARAM_SIZE != 0)
    /** \brief Deprecated API defined for backwards-compatibility */
    #define QActive_post(me_, sig_, par_) \
        QACTIVE_POST((me_), (sig_), (par_))

    /** \brief Deprecated API defined for backwards-compatibility */
    #define QActive_postISR(me_, sig_, par_) \
        QACTIVE_POST_ISR((me_), (sig_), (par_))
#else
    #define QActive_post(me_, sig_)    QACTIVE_POST((me_), (sig_))
    #define QActive_postISR(me_, sig_) QACTIVE_POST_ISR((me_), (sig_))
#endif

#ifdef QF_INT_LOCK
    #define QF_INT_DISABLE()  QF_INT_LOCK()
#endif

#ifdef QF_INT_UNLOCK
    #define QF_INT_ENABLE()   QF_INT_UNLOCK()
#endif

#ifdef QF_ISR_NEST
    #ifdef QF_ISR_KEY_TYPE
        #define QF_ISR_STAT_TYPE  QF_ISR_KEY_TYPE
    #endif
    #ifdef QF_ISR_LOCK
        #define QF_ISR_DISABLE(stat_)  QF_ISR_LOCK(stat_)
    #endif
    #ifdef QF_ISR_UNLOCK
        #define QF_ISR_RESTORE(stat_)  QF_ISR_UNLOCK(stat_)
    #endif
#endif                                                       /* QF_ISR_NEST */

#if (QF_TIMEEVT_CTR_SIZE != 0)
    /** \brief System clock tick processing for the default tick rate 0 */
    #define QF_tickISR() (QF_tickXISR((uint8_t)0))

    /** \brief Legacy API defined for backwards compatibility */
    #define QF_tick()    (QF_tickXISR((uint8_t)0))
#endif

/** \brief Arming a time event for the default tick rate (rate 0) */
#define QActive_arm(me_, ticks_) \
    (QActive_armX((me_), (uint8_t)0, (ticks_)))

/** \brief Disarming a time event for the default tick rate (rate 0) */
#define QActive_disarm(me_)     (QActive_disarmX((me_), (uint8_t)0))


#endif                                                             /* qfn_h */


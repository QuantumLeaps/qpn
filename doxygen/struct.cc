/*! @page struct Structure and Concepts

@tableofcontents

The QP-nano framework brings together three **most effective techniques** of structuring embedded systems:

- <a href="http://www.state-machine.com/doc/concepts.html#Active" target="_blank" class="extern">active objects</a>;

- <a href="http://www.state-machine.com/doc/concepts.html#HSM" target="_blank" class="extern">hierarchical state machines</a> (UML statecharts); and

- <a href="http://www.state-machine.com/doc/concepts.html#Modeling" target="_blank" class="extern">graphical modeling</a> and <a href="http://www.state-machine.com/doc/concepts.html#Code" target="_blank" class="extern">automatic code generation</a>.

The following sections describe the main components and structure of the framework.


------------------------------------------------------------------------------
@section comp Components of QP-nano
The QP-nano active object framework is comprised of the following components:

@image html qpn_components.jpg "Components of the QP-nano Framework"

<div class="separate"></div>
@subsection comp_qep QEP-nano Hierarchical Event Processor
QEP-nano is a universal, UML-compliant event processor that enables developers to code UML state machines in highly readable ANSI-C, in which every state machine element is mapped to code precisely, unambiguously, and exactly once (traceability). QEP-nano fully supports hierarchical state nesting, which is the fundamental mechanism for reusing behavior across many states instead of repeating the same actions and transitions over and over again. (See also @ref qepn).

<div class="separate"></div>
@subsection comp_qf QF-nano Active-Object Framework
QF-nano is a portable, event-driven, active-object (actor) framework for execution of **active objects** (concurrent state machines) specifically designed for real-time embedded (RTE) systems. (See also @ref qfn).

<div class="separate"></div>
@subsection comp_qv  QV-nano Cooperative Kernel
QV-nano is a simple **cooperative** kernel (previously called "Vanilla" kernel). This kernel executes active objects one at a time, with priority-based scheduling performed before processing of each event. Due to naturally short duration of event processing in state machines, the simple QV kernel is often adequate for many real-time systems. (See also @ref qvn).

<div class="separate"></div>
@subsection comp_qk QK-nano Preemptive Kernel
QK-nano is a tiny **preemptive**, priority-based, non-blocking, real-time kernel designed specifically for executing active objects. QK-nano meets all the requirement of the <a class="extern" target="_blank" href="http://en.wikipedia.org/wiki/Rate-monotonic_scheduling"><strong>Rate Monotonic Scheduling</strong></a> (a.k.a. Rate Monotonic Analysis &mdash; RMA) and can be used in hard real-time systems. (See also @ref qkn).


------------------------------------------------------------------------------
@section oop Object-Orientation

Even though QP/C is implemented in standard ANSI-C, it extensively uses object-oriented design principles such as **encapsulation** (classes), single **inheritance**, and, starting with QP5, **polymorphism** (late binding). At the C language level, these proven ways of software design become **design-patterns** and coding idioms.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_OOP_in_C.pdf"><img border="0" src="img/AN_OOP_in_C.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: Object-Oriented Programming in C
</div>
</div>
@endhtmlonly

The Quantum Leaps Application Note <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_OOP_in_C.pdf"><strong>Object-Oriented Programming in C</strong></a> describes how the OOP design patterns are implemented in QP/C and how you should code them in your own applications.
<div class="clear"></div>


------------------------------------------------------------------------------
@section classes Classes in QP-nano
The figure below shows the main classes comprising the QP-nano framework and their relation to the application-level code, such as the @ref game example application.

@image html qpn_classes.gif "Main Classes in the QP-nano Framework"

<ul class="tag">
  <li><span class="tag">0</span> The ::QEvt class represents events, possibly with a parameter (the existence of event parameter is configured by the macro #Q_PARAM_SIZE).
  </li>

  <li><span class="tag">1</span> The abstract ::QMsm class represents the most fundamental State Machine in QP-nano. This class implements the fastest and the most efficient strategy for coding hierarchical state machines, but this strategy is not human-maintainable and requires the use of the <a class="extern" target="_blank" href="http://www.state-machine.com/qm">QM modeling tool</a>. The class is abstract, meaning that it is not designed to be instantiated directly, but rather only for inheritance. The @ref game application provides an example of application-level classes deriving directly from ::QMsm (see [7]).
  </li>

  <li><span class="tag">2</span> The abstract ::QHsm class derives from ::QMsm and implements the state machine coding strategy suitable for manual coding and maintaining the code. The ::QHsm strategy is also supported by the <a class="extern" target="_blank" href="http://www.state-machine.com/qm">QM modeling tool</a>, but is not as fast or efficient as the ::QMsm strategy.
  </li>

  <li><span class="tag">3</span> The abstract ::QMActive class represents an active object that uses the ::QMsm style state machine implementation strategy. This strategy requires the use of the QM modeling tool to generate state machine code automatically, but the code is faster than in the ::QHsm style implementation strategy and needs less run-time support (smaller event-processor).
  </li>

  <li><span class="tag">4</span> The abstract ::QActive class represents an active object that uses the ::QHsm style implementation strategy for state machines. This strategy is tailored to manual coding, but it is also supported by the QM modeling tool. The resulting code is slower than in the ::QMsm-style implementation strategy.
  </li>

  <li><span class="tag">6</span> ::QActiveCB (QActive Control Block) represents the read-only (constant) information that the QF-nano needs to manage the active object. ::QActiveCB objects are grouped in the array ::QF_active, which typically can be placed in ROM.
  </li>

  <li><span class="tag">7</span> Applications can also use classes derived directly from the ::QMsm or ::QHsm base classes to represent "raw" state machines that are not active objects, because they don't have event queue and execution thread. Such "raw" state machines are typically used as "Orthogonal Components".
  </li>

</ul>

------------------------------------------------------------------------------
@section sm State Machines

The behavior of each active object in QP-nano is specified by means of a <a href="http://www.state-machine.com/doc/concepts.html#HSM" target="_blank" class="extern">hierarchical state machine</a> (UML statechart), which is the most effective and elegant technique of decomposing event-driven behavior. The most important innovation of UML state machines over classical finite state machines (FSMs) is the hierarchical state nesting. The value of state nesting lies in avoiding repetitions, which are inevitable in the traditional "flat" FSM formalism and are the main reason for the "state-transition explosion" in FSMs. The semantics of state nesting allow substates to define only the differences of behavior from the superstates, thus promoting sharing and reusing behavior.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_Crash_Course_in_UML_State_Machines.pdf"><img border="0" src="img/AN_Crash_Course_in_UML_State_Machines.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: A Crash Course in UML State Machines
</div>
</div>
@endhtmlonly

The Quantum Leaps Application Note <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_Crash_Course_in_UML_State_Machines.pdf"><strong>A Crash Course in UML State Machines</strong></a> introduces the main state machine concepts backed up by examples.
<div class="clear"></div>

@note
The hallmark of the QP-nano implementation of UML state machines is **traceability**, which is direct, precise, and unambiguous mapping of every state machine element to human-readable, portable, MISRA-compliant C code. Preserving the traceability from requirements through design to code is essential for mission-critical systems, such as medical devices or avionic systems.


------------------------------------------------------------------------------
@section coding Coding Standard
The QP/C framework has been developed in strict adherence to the documented <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_QL_Coding_Standard.pdf"><strong>Quantum Leaps Coding Standard</strong></a>.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_QL_Coding_Standard.pdf"><img border="0" src="img/AN_Coding_Standard.jpg " title="Download PDF"></a>
<div class="caption">
Application Note: Quantum Leaps C/C++ Coding Standard
</div>
</div>
@endhtmlonly


------------------------------------------------------------------------------
@section misra MISRA Compliance

<p>The QP-nano framework complies with most of the Motor Industry Software Reliability Association (MISRA) MISRA-C:2004 rules.
</p>

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_QP-nano_MISRA.pdf"><img border="0" src="img/AN_MISRA-QPn.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: QP-nano MISRA-C:2004 Compliance Matrix
</div>
</div>
@endhtmlonly

All deviations are carefully limited into very specific contexts and are documented with the Application Note: <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_QP-nano_MISRA.pdf"><strong>QP-nano MISRA-C:2004 Compliance Matrix</strong></a>.

@note
MISRA and MISRA C are registered trademarks of MIRA Ltd, held on behalf of the MISRA Consortium.

The MISRA guidelines place great emphasis on the use of static code analysts tools to check compliance with the MISRA-C language subset. To this end, QP-nano comes with an extensive support for automatic rule checking with @ref lint "PC-Lint". The QP frameworks go even beyond MISRA, by complying with the <strong>strict type checking</strong> of PC-Lint.


------------------------------------------------------------------------------
@section pc-lint PC-Lint Support

The QP-nano framework comes with extensive support for automatic rule checking by means of <a class="extern" target="_blank" href="http://www.gimpel.com/">PC-Lint</a>, which is designed not just for proving compliance of the QP-nano framework code, but more importantly, to aid in checking compliance of the application-level code. Any organization engaged in designing safety-related embedded software could benefit from the unprecedented quality infrastructure built around the QP-nano framework.

@sa @ref lint "Lint Port"

*/


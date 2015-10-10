/*! @page concepts Concepts and Structure

@tableofcontents

@section oop Object-Orientation

Even though QP-nano is implemented in standard ANSI-C, it extensively uses object-oriented design principles such as **encapsulation** (classes), single **inheritance**, and, starting with QP5, **polymorphism** (late binding). At the C language level, these proven ways of software design become **design-patterns** and coding idioms.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/resources/AN_Simple_OOP_in_C.pdf"><img border="0" src="img/AN_Simple_OOP_in_C.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: Simple OOP in C
</div>
</div>
@endhtmlonly

The Quantum Leaps Application Note <a class="extern" target="_blank" href="http://www.state-machine.com/resources/AN_Simple_OOP_in_C.pdf"><strong>Simple Object-Oriented Programming in C</strong></a> describes how the OOP design patterns are implemented in QP-nano and how you should code them in your own applications.
<div class="clear"></div>


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
@subsection comp_qk QK-nano Preemptive Kernel
QK-nano is a tiny **preemptive**, priority-based, non-blocking, real-time kernel designed specifically for executing active objects. QK-nano meets all the requirement of the <a class="extern" target="_blank" href="http://en.wikipedia.org/wiki/Rate-monotonic_scheduling"><strong>Rate Monotonic Scheduling</strong></a> (a.k.a. Rate Monotonic Analysis &mdash; RMA) and can be used in hard real-time systems. (See also @ref qkn). 

<div class="separate"></div>
@subsection comp_qv  QV-nano Cooperative Kernel
QV-nano is a simple **cooperative** kernel (previously called "Vanilla" kernel). This kernel executes active objects one at a time, with priority-based scheduling performed before processing of each event. Due to naturally short duration of event processing in state machines, the simple QV kernel is often adequate for many real-time systems. (See also @ref qvn). 


------------------------------------------------------------------------------
@section classes Classes in QP-nano
The figure below shows the main classes comprising the QP-nano framework and their relation to the application-level code, such as the @ref game example application.

@image html qpn_classes.png "Main Classes in the QP Framework"

<ul class="tag">
  <li><span class="tag">0</span> The ::QEvt class represents events, possibly with a parameter (the existence of event parameter is configured by the macro #Q_PARAM_SIZE). 
  </li>
  
  <li><span class="tag">1</span> The abstract ::QMsm class represents the most fundamental State Machine in QP-nano. This class implements the fastest and the most efficient strategy for coding hierarchical state machines, but this strategy is not human-maintainable and requires the use of the <a class="extern" target="_blank" href="http://www.state-machine.com/qm">QM modeling tool</a>. The class is abstract, meaning that it is not designed to be instantiated directly, but rather only for inheritance. The @ref game application provides an example of application-level classes deriving directly from ::QMsm (see <span class="tag">7</span>).
  </li>

  <li><span class="tag">2</span> The abstract ::QHsm class derives from ::QMsm and implements the state machine coding strategy suitable for manual coding and maintaining the code. The ::QHsm strategy is also supported by the <a class="extern" target="_blank" href="http://www.state-machine.com/qm">QM modeling tool</a>, but is not as fast or efficient as the ::QMsm strategy.
  </li>
  
  <li><span class="tag">3</span> The abstract ::QMActive class represents an active object that uses the ::QMsm style state machine implementation strategy. This strategy requires the use of the QM modeling tool to generate state machine code automatically, but the code is faster than in the ::QHsm style implementation strategy and needs less run-time support (smaller event-processor).
  </li>

  <li><span class="tag">4</span> The abstract ::QActive class represents an active object that uses the ::QHsm style implementation strategy for state machines. This strategy is tailored to manual coding, but it is also supported by the QM modeling tool. The resulting code is slower than in the ::QMsm-style implementation strategy.
  </li>

  <li><span class="tag">6</span> QActiveCB (QActive Control Block) represents the read-only (constant) information that the QF-nano needs to manage the active object. QActiveCB objects are grouped in the array QF_active[], which typically can be placed in ROM. 
  </li>

  <li><span class="tag">7</span> Applications can also use classes derived directly from the ::QMsm or ::QHsm base classes to represent "raw" state machines that are not active objects, because they don't have event queue and execution thread. Such "raw" state machines are typically used as "Orthogonal Components".
  </li>
  
</ul>

@next{exa}
*/


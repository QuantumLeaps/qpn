/** @page metrics Code Metrics 

@code 
                   Standard Code Metrics for QP-nano 5.4.2 

           Resource Standard Metrics (TM) for C, C++, C# and Java
                   Version 7.75 - mSquaredTechnologies.com

  License Type: Windows Single User License
  Licensed To : Quantum Leaps, LLC
  License No. : WS2975                          License Date: Dec 15, 2013
  Build Date  : Sep  2 2009                         Run Date: Jun 12, 2015
  (C)1996-2009 M Squared Technologies LLC
  ________________________________________________________________________

                           ~~ Function Metrics ~~
                      ~~ Complexity Detail Analysis ~~

  File: ..\include\qassert.h
  ________________________________________________________________________

                          ~~ Total File Summary ~~

  LOC 43       eLOC 42       lLOC 3        Comment 240      Lines      303
  ________________________________________________________________________
  End of File: ..\include\qassert.h


  File: ..\include\qepn.h
  ________________________________________________________________________

                          ~~ Total File Summary ~~

  LOC 144      eLOC 140      lLOC 49       Comment 400      Lines      586
  ________________________________________________________________________
  End of File: ..\include\qepn.h


  File: ..\include\qfn.h
  ________________________________________________________________________

                          ~~ Total File Summary ~~

  LOC 142      eLOC 142      lLOC 44       Comment 309      Lines      503
  ________________________________________________________________________
  End of File: ..\include\qfn.h


  File: ..\include\qkn.h
  ________________________________________________________________________

                          ~~ Total File Summary ~~

  LOC 32       eLOC 32       lLOC 14       Comment 101      Lines      146
  ________________________________________________________________________
  End of File: ..\include\qkn.h


  File: ..\include\qpn.h
  ________________________________________________________________________

                          ~~ Total File Summary ~~

  LOC 78       eLOC 77       lLOC 3        Comment 122      Lines      228
  ________________________________________________________________________
  End of File: ..\include\qpn.h


  File: ..\include\qvn.h
  ________________________________________________________________________

                          ~~ Total File Summary ~~

  LOC 6        eLOC 6        lLOC 2        Comment 61       Lines       72
  ________________________________________________________________________
  End of File: ..\include\qvn.h


  Error: Problem opening input file:
         ..\source\*.h
         For read, Invalid argument


  File: ..\source\qepn.c
  ________________________________________________________________________

  Function: QMsm_ctor
  Parameters: (QMsm * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 9        eLOC 7        lLOC 4        Comment 16       Lines        9

  Function: QMsm_init_
  Parameters: (QMsm * const me)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 1     
      Logical and ( && )      : 2     
  Complexity   Param 1       Return 1      Cyclo Vg 4       Total        6
  LOC 11       eLOC 10       lLOC 6        Comment 16       Lines       21

  Function: QMsm_dispatch_
  Parameters: (QMsm * const me)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 1     
      Loops for / foreach     : 1     
      Conditional if / else if: 5     
  Complexity   Param 1       Return 1      Cyclo Vg 8       Total       10
  LOC 38       eLOC 30       lLOC 20       Comment 26       Lines       52

  Function: QMsm_execTatbl_
  Parameters: (QMsm * const me, QMTranActTable const *tatbl)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops for / foreach     : 1     
      Conditional if / else if: 1     
  Complexity   Param 2       Return 1      Cyclo Vg 3       Total        6
  LOC 15       eLOC 11       lLOC 8        Comment 18       Lines       19

  Function: QMsm_exitToTranSource_
  Parameters: (QMsm * const me, QMState const *s, QMState const * const ts
              )
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 1     
      Conditional if / else if: 2     
  Complexity   Param 3       Return 1      Cyclo Vg 4       Total        8
  LOC 16       eLOC 9        lLOC 4        Comment 17       Lines       20

  Function: QMsm_enterHistory_
  Parameters: (QMsm * const me, QMState const * const hist)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 1     
      Loops for / foreach     : 1     
      Conditional if / else if: 2     
  Complexity   Param 2       Return 1      Cyclo Vg 5       Total        8
  LOC 27       eLOC 20       lLOC 16       Comment 20       Lines       33

  Function: QHsm_ctor
  Parameters: (QHsm * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 9        eLOC 7        lLOC 4        Comment 30       Lines       10

  Function: QHsm_top
  Parameters: (void const * const me)
  Complexity   Param 1       Return 1      Cyclo Vg 1       Total        3
  LOC 4        eLOC 3        lLOC 2        Comment 16       Lines        4

  Function: QHsm_init_
  Parameters: (QHsm * const me)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 3     
      Logical and ( && )      : 2     
  Complexity   Param 1       Return 1      Cyclo Vg 6       Total        8
  LOC 33       eLOC 31       lLOC 25       Comment 22       Lines       49

  Function: QHsm_dispatch_
  Parameters: (QHsm * const me)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 4     
      Loops for / foreach     : 2     
      Conditional if / else if: 3     
  Complexity   Param 1       Return 1      Cyclo Vg 10      Total       12
  LOC 61       eLOC 53       lLOC 45       Comment 42       Lines       86

  Function: QHsm_tran_
  Parameters: (QHsm * const me, QStateHandler path[QHSM_MAX_NEST_DEPTH_])
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 4     
      Conditional if / else if: 10    
  Complexity   Param 2       Return 1      Cyclo Vg 15      Total       18
  LOC 98       eLOC 78       lLOC 57       Comment 65       Lines      134

  ------------------------------------------------------------------------

                          ~~ Total File Summary ~~

  LOC 350      eLOC 288      lLOC 196      Comment 355      Lines      697
  ------------------------------------------------------------------------

                        ~~ File Functional Summary ~~

  File Function Count....:         11
  Total Function LOC.....:        321  Total Function Pts LOC :        2.7
  Total Function eLOC....:        259  Total Function Pts eLOC:        2.3
  Total Function lLOC....:        191  Total Function Pts lLOC:        1.5
  Total Function Params .:         18  Total Function Return .:         11
  Total Cyclo Complexity :         58  Total Function Complex.:         87
         ------     -----     -----     ------     ------     -----       
  Max Function LOC ......:         98  Average Function LOC ..:      29.18
  Max Function eLOC .....:         78  Average Function eLOC .:      23.55
  Max Function lLOC .....:         57  Average Function lLOC .:      17.36
         ------     -----     -----     ------     ------     -----       
  Max Function Parameters:          3  Avg Function Parameters:       1.64
  Max Function Returns ..:          1  Avg Function Returns ..:       1.00
  Max Interface Complex. :          4  Avg Interface Complex. :       2.64
  Max Cyclomatic Complex.:         15  Avg Cyclomatic Complex.:       5.27
  Max Total Complexity ..:         18  Avg Total Complexity ..:       7.91
  ________________________________________________________________________
  End of File: ..\source\qepn.c


  File: ..\source\qfn.c
  ________________________________________________________________________

  Function: QMActive_ctor
  Parameters: (QMActive * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 10       eLOC 8        lLOC 3        Comment 18       Lines       11

  Function: QActive_ctor
  Parameters: (QActive * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 10       eLOC 8        lLOC 3        Comment 11       Lines       20

  Function: QActive_postX_
  Parameters: (QMActive * const me, uint_fast8_t margin, enum_t const sig,
               QParam const par)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Conditional if / else if: 4     
  Complexity   Param 4       Return 1      Cyclo Vg 5       Total       10
  LOC 35       eLOC 28       lLOC 18       Comment 36       Lines       49

  Function: QActive_postXISR_
  Parameters: (QMActive * const me, uint_fast8_t margin, enum_t const sig,
               QParam const par)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Conditional if / else if: 3     
  Complexity   Param 4       Return 1      Cyclo Vg 4       Total        9
  LOC 43       eLOC 37       lLOC 17       Comment 33       Lines       53

  Function: QF_tickXISR
  Parameters: (uint_fast8_t const tickRate)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 1     
      Conditional if / else if: 3     
  Complexity   Param 1       Return 1      Cyclo Vg 5       Total        7
  LOC 28       eLOC 24       lLOC 10       Comment 24       Lines       32

  Function: QActive_armX
  Parameters: (QMActive * const me, uint_fast8_t const tickRate, QTimeEvtC
              tr const nTicks, QTimeEvtCtr const interval)
  Complexity   Param 4       Return 1      Cyclo Vg 1       Total        6
  LOC 11       eLOC 9        lLOC 5        Comment 32       Lines       13

  Function: QActive_disarmX
  Parameters: (QMActive * const me, uint_fast8_t const tickRate)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 12       eLOC 11       lLOC 5        Comment 14       Lines       14

  ------------------------------------------------------------------------

                          ~~ Total File Summary ~~

  LOC 202      eLOC 175      lLOC 66       Comment 248      Lines      468
  ------------------------------------------------------------------------

                        ~~ File Functional Summary ~~

  File Function Count....:          7
  Total Function LOC.....:        149  Total Function Pts LOC :        1.6
  Total Function eLOC....:        125  Total Function Pts eLOC:        1.4
  Total Function lLOC....:         61  Total Function Pts lLOC:        0.5
  Total Function Params .:         19  Total Function Return .:          7
  Total Cyclo Complexity :         18  Total Function Complex.:         44
         ------     -----     -----     ------     ------     -----       
  Max Function LOC ......:         43  Average Function LOC ..:      21.29
  Max Function eLOC .....:         37  Average Function eLOC .:      17.86
  Max Function lLOC .....:         18  Average Function lLOC .:       8.71
         ------     -----     -----     ------     ------     -----       
  Max Function Parameters:          4  Avg Function Parameters:       2.71
  Max Function Returns ..:          1  Avg Function Returns ..:       1.00
  Max Interface Complex. :          5  Avg Interface Complex. :       3.71
  Max Cyclomatic Complex.:          5  Avg Cyclomatic Complex.:       2.57
  Max Total Complexity ..:         10  Avg Total Complexity ..:       6.29
  ________________________________________________________________________
  End of File: ..\source\qfn.c


  File: ..\source\qfn_init.c
  ________________________________________________________________________

  Function: QF_init
  Parameters: (void)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops for / foreach     : 3     
  Complexity   Param 0       Return 1      Cyclo Vg 4       Total        5
  LOC 35       eLOC 31       lLOC 18       Comment 18       Lines       46

  ------------------------------------------------------------------------

                          ~~ Total File Summary ~~

  LOC 39       eLOC 35       lLOC 18       Comment 60       Lines      103
  ------------------------------------------------------------------------

                        ~~ File Functional Summary ~~

  File Function Count....:          1
  Total Function LOC.....:         35  Total Function Pts LOC :        0.3
  Total Function eLOC....:         31  Total Function Pts eLOC:        0.3
  Total Function lLOC....:         18  Total Function Pts lLOC:        0.1
  Total Function Params .:          0  Total Function Return .:          1
  Total Cyclo Complexity :          4  Total Function Complex.:          5
         ------     -----     -----     ------     ------     -----       
  Max Function LOC ......:         35  Average Function LOC ..:      35.00
  Max Function eLOC .....:         31  Average Function eLOC .:      31.00
  Max Function lLOC .....:         18  Average Function lLOC .:      18.00
         ------     -----     -----     ------     ------     -----       
  Max Function Parameters:          0  Avg Function Parameters:       0.00
  Max Function Returns ..:          1  Avg Function Returns ..:       1.00
  Max Interface Complex. :          1  Avg Interface Complex. :       1.00
  Max Cyclomatic Complex.:          4  Avg Cyclomatic Complex.:       4.00
  Max Total Complexity ..:          5  Avg Total Complexity ..:       5.00
  ________________________________________________________________________
  End of File: ..\source\qfn_init.c


  File: ..\source\qkn.c
  ________________________________________________________________________

  Function: initialize
  Parameters: (void)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops for / foreach     : 2     
      Conditional if / else if: 1     
  Complexity   Param 0       Return 1      Cyclo Vg 4       Total        5
  LOC 23       eLOC 19       lLOC 15       Comment 21       Lines       32

  Function: QF_run
  Parameters: (void)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops for / foreach     : 1     
  Complexity   Param 0       Return 1      Cyclo Vg 2       Total        3
  LOC 10       eLOC 8        lLOC 5        Comment 17       Lines       12

  Function: QK_schedPrio_
  Parameters: (void)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Conditional if / else if: 3     
  Complexity   Param 0       Return 1      Cyclo Vg 4       Total        5
  LOC 29       eLOC 22       lLOC 7        Comment 21       Lines       38

  Function: QK_sched_
  Parameters: (uint_fast8_t p)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops while / do        : 1     
      Conditional if / else if: 5     
  Complexity   Param 1       Return 1      Cyclo Vg 7       Total        9
  LOC 54       eLOC 45       lLOC 25       Comment 29       Lines       76

  Function: QK_mutexLock
  Parameters: (uint_fast8_t const prioCeiling)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Conditional if / else if: 1     
  Complexity   Param 1       Return 1      Cyclo Vg 2       Total        4
  LOC 10       eLOC 8        lLOC 6        Comment 18       Lines       10

  Function: QK_mutexUnlock
  Parameters: (QMutex mutex)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Conditional if / else if: 2     
  Complexity   Param 1       Return 1      Cyclo Vg 3       Total        5
  LOC 11       eLOC 8        lLOC 5        Comment 17       Lines       11

  ------------------------------------------------------------------------

                          ~~ Total File Summary ~~

  LOC 154      eLOC 127      lLOC 67       Comment 173      Lines      337
  ------------------------------------------------------------------------

                        ~~ File Functional Summary ~~

  File Function Count....:          6
  Total Function LOC.....:        137  Total Function Pts LOC :        1.2
  Total Function eLOC....:        110  Total Function Pts eLOC:        1.0
  Total Function lLOC....:         63  Total Function Pts lLOC:        0.5
  Total Function Params .:          3  Total Function Return .:          6
  Total Cyclo Complexity :         22  Total Function Complex.:         31
         ------     -----     -----     ------     ------     -----       
  Max Function LOC ......:         54  Average Function LOC ..:      22.83
  Max Function eLOC .....:         45  Average Function eLOC .:      18.33
  Max Function lLOC .....:         25  Average Function lLOC .:      10.50
         ------     -----     -----     ------     ------     -----       
  Max Function Parameters:          1  Avg Function Parameters:       0.50
  Max Function Returns ..:          1  Avg Function Returns ..:       1.00
  Max Interface Complex. :          2  Avg Interface Complex. :       1.50
  Max Cyclomatic Complex.:          7  Avg Cyclomatic Complex.:       3.67
  Max Total Complexity ..:          9  Avg Total Complexity ..:       5.17
  ________________________________________________________________________
  End of File: ..\source\qkn.c


  File: ..\source\qvn.c
  ________________________________________________________________________

  Function: QF_run
  Parameters: (void)
    Cyclomatic Complexity Vg Detail
      Function Base           : 1
      Loops for / foreach     : 3     
      Conditional if / else if: 4     
  Complexity   Param 0       Return 1      Cyclo Vg 8       Total        9
  LOC 58       eLOC 47       lLOC 29       Comment 35       Lines       85

  ------------------------------------------------------------------------

                          ~~ Total File Summary ~~

  LOC 65       eLOC 54       lLOC 29       Comment 77       Lines      148
  ------------------------------------------------------------------------

                        ~~ File Functional Summary ~~

  File Function Count....:          1
  Total Function LOC.....:         58  Total Function Pts LOC :        0.5
  Total Function eLOC....:         47  Total Function Pts eLOC:        0.4
  Total Function lLOC....:         29  Total Function Pts lLOC:        0.2
  Total Function Params .:          0  Total Function Return .:          1
  Total Cyclo Complexity :          8  Total Function Complex.:          9
         ------     -----     -----     ------     ------     -----       
  Max Function LOC ......:         58  Average Function LOC ..:      58.00
  Max Function eLOC .....:         47  Average Function eLOC .:      47.00
  Max Function lLOC .....:         29  Average Function lLOC .:      29.00
         ------     -----     -----     ------     ------     -----       
  Max Function Parameters:          0  Avg Function Parameters:       0.00
  Max Function Returns ..:          1  Avg Function Returns ..:       1.00
  Max Interface Complex. :          1  Avg Interface Complex. :       1.00
  Max Cyclomatic Complex.:          8  Avg Cyclomatic Complex.:       8.00
  Max Total Complexity ..:          9  Avg Total Complexity ..:       9.00
  ________________________________________________________________________
  End of File: ..\source\qvn.c


  ------------------------------------------------------------------------

                     ~~ Project Analysis For 11 Files ~~

  ------------------------------------------------------------------------

                         ~~ Total Project Summary ~~

  LOC 1255     eLOC 1118     lLOC 491      Comment 2146     Lines     3591
  Average per File, metric/11 files
  LOC 114      eLOC 101      lLOC 44       Comment 195      Lines      326

  ------------------------------------------------------------------------

                      ~~ Project Functional Metrics ~~

  Function: QMsm_ctor
  Parameters: (QMsm * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 9        eLOC 7        lLOC 4        Comment 16       Lines        9

  Function: QMsm_init_
  Parameters: (QMsm * const me)
  Complexity   Param 1       Return 1      Cyclo Vg 4       Total        6
  LOC 11       eLOC 10       lLOC 6        Comment 16       Lines       21

  Function: QMsm_dispatch_
  Parameters: (QMsm * const me)
  Complexity   Param 1       Return 1      Cyclo Vg 8       Total       10
  LOC 38       eLOC 30       lLOC 20       Comment 26       Lines       52

  Function: QMsm_execTatbl_
  Parameters: (QMsm * const me, QMTranActTable const *tatbl)
  Complexity   Param 2       Return 1      Cyclo Vg 3       Total        6
  LOC 15       eLOC 11       lLOC 8        Comment 18       Lines       19

  Function: QMsm_exitToTranSource_
  Parameters: (QMsm * const me, QMState const *s, QMState const * const ts
              )
  Complexity   Param 3       Return 1      Cyclo Vg 4       Total        8
  LOC 16       eLOC 9        lLOC 4        Comment 17       Lines       20

  Function: QMsm_enterHistory_
  Parameters: (QMsm * const me, QMState const * const hist)
  Complexity   Param 2       Return 1      Cyclo Vg 5       Total        8
  LOC 27       eLOC 20       lLOC 16       Comment 20       Lines       33

  Function: QHsm_ctor
  Parameters: (QHsm * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 9        eLOC 7        lLOC 4        Comment 30       Lines       10

  Function: QHsm_top
  Parameters: (void const * const me)
  Complexity   Param 1       Return 1      Cyclo Vg 1       Total        3
  LOC 4        eLOC 3        lLOC 2        Comment 16       Lines        4

  Function: QHsm_init_
  Parameters: (QHsm * const me)
  Complexity   Param 1       Return 1      Cyclo Vg 6       Total        8
  LOC 33       eLOC 31       lLOC 25       Comment 22       Lines       49

  Function: QHsm_dispatch_
  Parameters: (QHsm * const me)
  Complexity   Param 1       Return 1      Cyclo Vg 10      Total       12
  LOC 61       eLOC 53       lLOC 45       Comment 42       Lines       86

  Function: QHsm_tran_
  Parameters: (QHsm * const me, QStateHandler path[QHSM_MAX_NEST_DEPTH_])
  Complexity   Param 2       Return 1      Cyclo Vg 15      Total       18
  LOC 98       eLOC 78       lLOC 57       Comment 65       Lines      134

  Function: QMActive_ctor
  Parameters: (QMActive * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 10       eLOC 8        lLOC 3        Comment 18       Lines       11

  Function: QActive_ctor
  Parameters: (QActive * const me, QStateHandler initial)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 10       eLOC 8        lLOC 3        Comment 11       Lines       20

  Function: QActive_postX_
  Parameters: (QMActive * const me, uint_fast8_t margin, enum_t const sig,
               QParam const par)
  Complexity   Param 4       Return 1      Cyclo Vg 5       Total       10
  LOC 35       eLOC 28       lLOC 18       Comment 36       Lines       49

  Function: QActive_postXISR_
  Parameters: (QMActive * const me, uint_fast8_t margin, enum_t const sig,
               QParam const par)
  Complexity   Param 4       Return 1      Cyclo Vg 4       Total        9
  LOC 43       eLOC 37       lLOC 17       Comment 33       Lines       53

  Function: QF_tickXISR
  Parameters: (uint_fast8_t const tickRate)
  Complexity   Param 1       Return 1      Cyclo Vg 5       Total        7
  LOC 28       eLOC 24       lLOC 10       Comment 24       Lines       32

  Function: QActive_armX
  Parameters: (QMActive * const me, uint_fast8_t const tickRate, QTimeEvtC
              tr const nTicks, QTimeEvtCtr const interval)
  Complexity   Param 4       Return 1      Cyclo Vg 1       Total        6
  LOC 11       eLOC 9        lLOC 5        Comment 32       Lines       13

  Function: QActive_disarmX
  Parameters: (QMActive * const me, uint_fast8_t const tickRate)
  Complexity   Param 2       Return 1      Cyclo Vg 1       Total        4
  LOC 12       eLOC 11       lLOC 5        Comment 14       Lines       14

  Function: QF_init
  Parameters: (void)
  Complexity   Param 0       Return 1      Cyclo Vg 4       Total        5
  LOC 35       eLOC 31       lLOC 18       Comment 18       Lines       46

  Function: initialize
  Parameters: (void)
  Complexity   Param 0       Return 1      Cyclo Vg 4       Total        5
  LOC 23       eLOC 19       lLOC 15       Comment 21       Lines       32

  Function: QF_run
  Parameters: (void)
  Complexity   Param 0       Return 1      Cyclo Vg 2       Total        3
  LOC 10       eLOC 8        lLOC 5        Comment 17       Lines       12

  Function: QK_schedPrio_
  Parameters: (void)
  Complexity   Param 0       Return 1      Cyclo Vg 4       Total        5
  LOC 29       eLOC 22       lLOC 7        Comment 21       Lines       38

  Function: QK_sched_
  Parameters: (uint_fast8_t p)
  Complexity   Param 1       Return 1      Cyclo Vg 7       Total        9
  LOC 54       eLOC 45       lLOC 25       Comment 29       Lines       76

  Function: QK_mutexLock
  Parameters: (uint_fast8_t const prioCeiling)
  Complexity   Param 1       Return 1      Cyclo Vg 2       Total        4
  LOC 10       eLOC 8        lLOC 6        Comment 18       Lines       10

  Function: QK_mutexUnlock
  Parameters: (QMutex mutex)
  Complexity   Param 1       Return 1      Cyclo Vg 3       Total        5
  LOC 11       eLOC 8        lLOC 5        Comment 17       Lines       11

  Function: QF_run
  Parameters: (void)
  Complexity   Param 0       Return 1      Cyclo Vg 8       Total        9
  LOC 58       eLOC 47       lLOC 29       Comment 35       Lines       85

  Total: Functions  
  LOC 700      eLOC 572      lLOC 362      InCmp 66       CycloCmp     110
  Function Points         FP(LOC) 5.5     FP(eLOC) 4.5    FP(lLOC)     2.8

  ------------------------------------------------------------------------

                      ~~ Project Functional Analysis ~~

  Total Functions .......:         26  Total Physical Lines ..:        939
  Total LOC .............:        700  Total Function Pts LOC :        5.5
  Total eLOC ............:        572  Total Function Pts eLOC:        4.5
  Total lLOC.............:        362  Total Function Pts lLOC:        2.8
  Total Cyclomatic Comp. :        110  Total Interface Comp. .:         66
  Total Parameters ......:         40  Total Return Points ...:         26
  Total Comment Lines ...:        632  Total Blank Lines .....:        116
         ------     -----     -----     ------     ------     -----       
  Avg Physical Lines ....:      36.12
  Avg LOC ...............:      26.92  Avg eLOC ..............:      22.00
  Avg lLOC ..............:      13.92  Avg Cyclomatic Comp. ..:       4.23
  Avg Interface Comp. ...:       2.54  Avg Parameters ........:       1.54
  Avg Return Points .....:       1.00  Avg Comment Lines .....:      24.31
         ------     -----     -----     ------     ------     -----       
  Max LOC ...............:         98
  Max eLOC ..............:         78  Max lLOC ..............:         57
  Max Cyclomatic Comp. ..:         15  Max Interface Comp. ...:          5
  Max Parameters ........:          4  Max Return Points .....:          1
  Max Comment Lines .....:         65  Max Total Lines .......:        134
         ------     -----     -----     ------     ------     -----       
  Min LOC ...............:          4
  Min eLOC ..............:          3  Min lLOC ..............:          2
  Min Cyclomatic Comp. ..:          1  Min Interface Comp. ...:          1
  Min Parameters ........:          0  Min Return Points .....:          1
  Min Comment Lines .....:         11  Min Total Lines .......:          4

  ------------------------------------------------------------------------

                             ~~ File Summary ~~

  C Source Files *.c ....:          5  C/C++ Include Files *.h:          6
  C++ Source Files *.c* .:          0  C++ Include Files *.h* :          0
  C# Source Files *.cs ..:          0  Java Source File *.jav*:          0
  Other File Count ......:          0  Total File Count ......:         11
  ________________________________________________________________________

@endcode 
*/ 

54:                QK_ISR(auto_psv) _T2Interrupt() {
 007B6  F80036     push.w 0x0036
 007B8  BE9F80     mov.d 0x0000,[0x001e++]
 007BA  97B84F     mov.w [0x001e-8],0x0000
 007BC  DE00CD     lsr 0x0000,#13,0x0002
 007BE  200010     mov.w #0x1,0x0000
 007C0  DD0001     sl 0x0000,0x0002,0x0000
 007C2  B76874     ior.b 0x0874
 007C4  370002     bra 0x0007ca
 007C6  F80036     push.w 0x0036
 007C8  BE9F80     mov.d 0x0000,[0x001e++]
 007CA  BE9F82     mov.d 0x0004,[0x001e++]
 007CC  BE9F84     mov.d 0x0008,[0x001e++]
 007CE  BE9F86     mov.d 0x000c,[0x001e++]
 007D0  781F88     mov.w 0x0010,[0x001e++]
 007D2  F80034     push.w 0x0034
 007D4  B3C000     mov.b #0x0,0x0000
 007D6  8801A0     mov.w 0x0000,0x0034
55:                    _T2IF = 0;                              /* clear Timer 2 interrupt flag */
 007D8  A9E084     bclr.b 0x0084,#7
56:                    QF_tick();                   /* handle all armed time events in QF-nano */
 007DA  07014B     rcall 0x000a72
57:
58:                    QK_ISR_EXIT();                  /* inform QK-nano about exiting the ISR */
 007DC  800218     mov.w 0x0042,0x0010
 007DE  FC3FFF     disi #16383
 007E0  DE4045     lsr 0x0010,#5,0x0000
 007E2  600067     and.w 0x0000,#7,0x0000
 007E4  200011     mov.w #0x1,0x0002
 007E6  DD0800     sl 0x0002,0x0000,0x0000
 007E8  EAC000     com.b 0x0000,0x0000
 007EA  B66874     and.b 0x0874
 007EC  BFC874     mov.b 0x0874,0x0000
 007EE  B20FE0     and.w #0xfe,0x0000
 007F0  3A0007     bra nz, 0x000800
 007F2  E24879     cp0.b 0x0879
 007F4  320005     bra z, 0x000800
 007F6  800210     mov.w 0x0042,0x0000
 007F8  60007F     and.w 0x0000,#31,0x0000
 007FA  880210     mov.w 0x0000,0x0042
 007FC  070186     rcall 0x000b0a
 007FE  880218     mov.w 0x0010,0x0042
 00800  FC0000     disi #0
59:                }
 00802  F90034     pop.w 0x0034
 00804  78044F     mov.w [--0x001e],0x0010
 00806  BE034F     mov.d [--0x001e],0x000c
 00808  BE024F     mov.d [--0x001e],0x0008
 0080A  BE014F     mov.d [--0x001e],0x0004
 0080C  BE004F     mov.d [--0x001e],0x0000
 0080E  F90036     pop.w 0x0036
 00810  064000     retfie

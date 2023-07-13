.list
;
; 構造化マクロ 'struc'  Ver 2.40    30/Aug/1989
;
;  Copyright (C)1989 Kacchan
;                    JUP1076D(JUPITER)
;                    NBC00034(NIFty-Serve)
;                    QNG33261(PC-VAN)
;
.xlist

_??ver_struc equ 240

_?n     = 0
_?pi    = 0
_?pn    = 0
_?pb    = 0
_?pt    = 0
_?pc    = 0
_??and  = 2
_??or   = 1
_?regf  = 0
_$$byte equ byte ptr
_$$word equ word ptr

_if     macro s1,p1,p2,p3,p4
        _?opf  = 0
        _?lgf  = 0
        _?then = 0
        __ckp <p1>,<p2>,<p3>,<p4>
        ife _?then
          _?n = _?n+3
          __ps i,%_?pi,%_?n-3
          __ps n,%_?pn,%_?n-2
          __ps t,%_?pt,%_?n-1
          _?mf  = 0
          __c0 <s1>
        else
          _?n = _?n+2
          __ps n,%_?pn,%_?n-2
          __ps t,%_?pt,%_?n-1
          _?mf  = 6
          __c6 <s1>
        endif
        endm

_else   macro
        _?n = _?n+1
        __jp jmp,i,%_?pi-1
        __pps n,%_?pn-1,%_?n-1
        endm

_elseif macro s1,p1,p2,p3
        _?n = _?n+2
        __jp jmp,i,%_?pi-1
        __pps n,%_?pn-1,%_?n-2
        __ps t,%_?pt,%_?n-1
        _?mf  = 0
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        __c0 <s1>
        endm

_endif  macro
        __pp n,%_?pn-1
        __pp i,%_?pi-1
        endm

_skipif macro bno
        ifb <bno>
          __jp jmp,i,%_?pi-1
        else
          __jp jmp,i,%_?pi-bno
        endif
        endm

_exitif macro s1,p1,p2,p3
        _?n = _?n+1
        __ps n,%_?pn,%_?n-1
        _?mf  = 5
        _?exn = 1
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        __c5 <s1>
        endm

_switch macro o1,o2
        ife _?regf
          __dum %_?pb
        endif
        _?n = _?n+2
        __ps  b,%_?pb,%_?n-2
        __ps  n,%_?pn,%_?n-1
        __bkl s,%_?pb-1
        ifb <o2>
          __psc %_?pc,<o1>
        else
          ifdef _$$&o1
            __psc %_?pc,<_$$&o1 o2>
          else
            __psc %_?pc,<o1 o2>
          endif
        endif
        _?regf = 0
        endm

_case   macro s1,op,cd
        _?n = _?n+2
        __pps n,%_?pn-1,%_?n-2
        __ca1 <s1>,op,cd
        __lbl %_?n-1
        endm

_endsw  macro
        __bkl e,%_?pb-1
        __pp  n,%_?pn-1
        __pp  b,%_?pb-1
        __popm %_?pb
        endm

_default macro
        _?n = _?n+1
        __pps n,%_?pn-1,%_?n-1
        endm

_do     macro
        ife _?regf
          __dum %_?pb
        endif
        _?n = _?n+1
        __ps  b,%_?pb,%_?n-1
        __bkl s,%_?pb-1
        _?regf = 0
        endm

_while  macro s1,p1,p2
        ife _?regf
          __dum %_?pb
        endif
        _?n = _?n+2
        __ps  b,%_?pb,%_?n-2
        __ps  t,%_?pt,%_?n-1
        __bkl s,%_?pb-1
        _?mf  = 3
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2
        __c3 <s1>
        _?regf = 0
        endm

_enddo  macro
        __bkl e,%_?pb-1
        __jp  jmp,b,%_?pb-1,s
        __pp  b,%_?pb-1
        __popm %_?pb
        endm

_until  macro s1,p1,p2,p3
        _?n = _?n+1
        __ps  t,%_?pt,%_?n-1
        __bkl e,%_?pb-1
        _?mf  = 4
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        __c4 <s1>
        endm

_loop   macro p1
        ife _?regf
          __dum %_?pb
        endif
        _?n = _?n+1
        __ps b,%_?pb,%_?n-1
        ifnb <p1>
          mov cx,p1
        endif
        __bkl s,%_?pb-1
        _?regf = 0
        endm

_endloop macro cd
        __bkl e,%_?pb-1
        __jp  loop&cd,b,%_?pb-1,s
        __pp  b,%_?pb-1
        __popm %_?pb
        endm

_blockreg macro reg
        _?regf = 1
        __defm %_?pb,<reg>
        endm

_block  macro p1
        ife _?regf
          __dum %_?pb
        endif
        _?n = _?n+1
        __ps  b,%_?pb,%_?n-1
        __bkl s,%_?pb-1
        _?regf = 0
        endm

_endblock macro
        __bkl e,%_?pb-1
        __pp  b,%_?pb-1
        __popm %_?pb
        endm

_exit   macro s1,p1,p2,p3
        _?exn = 1
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        _?popf = 0
        _?bk = _?pb-1
        _?tmp = _?pb-_?exn
        __chkb %_?bk
        if _?popf
          _on _break %_?exn,<s1>,<>,p1,p2,p3
        else
          _?n = _?n+1
          __ps n,%_?pn,%_?n-1
          _?mf  = 1
          __c1 <s1>
        endif
        endm

_repeat macro s1,p1,p2,p3
        _?n = _?n+1
        __ps n,%_?pn,%_?n-1
        _?mf  = 2
        _?exn = 1
        _?end = 0
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        __c2 <s1>
        endm

_break  macro bno
        ifb <bno>
          _?exn = 1
        else
          _?exn = bno
        endif
        _?bk = _?pb-1
        _?tmp = _?pb-_?exn
        __popbk %_?bk
        __jp jmp,b,%_?pb-_?exn
        endm

_popreg macro
        _?bk = _?pb-1
        _?tmp = 0
        __popbk %_?bk
        $$pop0
        endm

_continue macro bno,ed
        _?exn = 1
        _?end = 0
        __ckp bno,ed
        if _?end
          __jp jmp,b,%_?pb-_?exn,e
        else
          __jp jmp,b,%_?pb-_?exn,s
        endif
        endm

_c      macro s1,p1,p2,p3
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        __c %_?mf,<s1>
        endm

_on     macro  oc,lb,cd,st,p1,p2,p3
        ifidn <oc>,<jump>
          __onjmp  <lb>,<cd>,<st>,p1,p2,p3
        else
          __oncall oc,<lb>,<cd>,<st>,p1,p2,p3
        endif
        endm

__oncall macro  oc,lb,cd,st,p1,p2,p3
        $$mac macro
         __irp &st
         &oc   &lb
        endm
        _?n = _?n+2
        __ps n,%_?pn,%_?n-2
        __ps t,%_?pt,%_?n-1
        _?mf  = 6
        _?opf = 0
        _?lgf = 0
        __ckp p1,p2,p3
        __c6 <cd>
        endm

__onjmp macro  lb,cd,st,p1,p2,p3
        __ifb st
        if _?bnk
          $$mac macro oc
            oc &lb
          endm
          _?n = _?n+1
          __ps n,%_?pn,%_?n-1
          _?mf  = 7
          _?opf = 0
          _?lgf = 0
          __ckp p1,p2,p3
          __c7 <cd>
        else
          __oncall jmp,<lb>,<cd>,<st>,p1,p2,p3
        endif
        endm

__rev   macro s1,s2,s3,s4,s5,s6,s7,s8,s9,sa,sb,sc,sd
        ifb <s1>
          __rept pop,sd,sc,sb,sa,s9,s8,s7,s6,s5,s4,s3,s2,s1
          exitm
        endif
        __rev s2,s3,s4,s5,s6,s7,s8,s9,sa,sb,sc,sd,s1
        endm

__rept  macro oc,s1,s2,s3,s4,s5,s6,s7,s8,s9,sa,sb,sc,sd
        ifb <s1>
          exitm
        endif
        ifidn <s1>,<all>
          &oc&a
        else
          ifidn <s1>,<flag>
            &oc&f
          else
            oc s1
          endif
        endif
        __rept oc,s2,s3,s4,s5,s6,s7,s8,s9,sa,sb,sc,sd
        endm

__dum   macro bk
        _?popf&bk = 0
        __dumy $$pop&bk
        endm
__dumy  macro nm
        nm macro
        endm
        endm

__defm  macro bk,st
        _?popf&bk = 1
        __rept push,st
        __defmm $$pop&bk,<st>
        endm
__defmm macro nm,st
        nm macro
          __rev &st
        endm
        endm

__chkb  macro bk
        if bk eq _?tmp
          exitm
        endif
        if _?popf&bk
          _?popf = 1
          exitm
        endif
        _?bk = bk-1
        __chkb %_?bk
        endm

__popbk macro bk
        if bk eq _?tmp
          exitm
        endif
        __popm %_?bk
        _?bk = bk-1
        __popbk %_?bk
        endm

__popm  macro bk
        $$pop&bk
        endm

__irp   macro s1,s2,s3,s4,s5,s6,s7,s8,s9,sa
        ifb <s1>
          exitm
        endif
        s1
        __irp <s2>,<s3>,<s4>,<s5>,<s6>,<s7>,<s8>,<s9>,<sa>
        endm

__ifb   macro  s1,s2,s3,s4,s5,s6,s7,s8,s9,sa,sb,sc,sd,se,sf
        _?bnk = 0
        ifb <s1>
          _?bnk = 1
        endif
        endm

__c     macro ex,s1
        __c&ex <s1>
        endm

__c0    macro s1
        ife _?lgf
          _?dlg = 2
          __xp s1
          __pp t,%_?pt-1
          exitm
        endif
        __cc <s1>
        endm

__c1    macro s1
        ife _?lgf
          _?dlg = 1
          __xp s1
          __pp n,%_?pn-1
          exitm
        endif
        __cc <s1>
        endm

__c2    macro s1
        ife _?lgf
          _?dlg = 1
          __xp s1
          __pp n,%_?pn-1
          exitm
        endif
        __cc <s1>
        endm

__c3    macro s1
        ife _?lgf
          _?dlg = 2
          __xp s1
          __pp t,%_?pt-1
          exitm
        endif
        __cc <s1>
        endm

__c4    macro s1
        ife _?lgf
          _?dlg = 2
          __xp s1
          __pp t,%_?pt-1
          __pp b,%_?pb-1
          __popm %_?pb
          exitm
        endif
        __cc <s1>
        endm

__c5    macro s1
        ife _?lgf
          _?dlg = 1
          __xp s1
          __pp n,%_?pn-1
          exitm
        endif
        __cc <s1>
        endm

__c6    macro s1
        ife _?lgf
          _?dlg = 2
          __xp s1
          __pp t,%_?pt-1
          $$mac
          __pp n,%_?pn-1
          exitm
        endif
        __cc <s1>
        endm

__c7    macro s1
        ife _?lgf
          _?dlg = 1
          __xp s1
          __pp n,%_?pn-1
          exitm
        endif
        __cc <s1>
        endm

__cc    macro s1
        _?dlg = _?lgf
        if _?lgf eq 1
          _?n = _?n+1
          __ps n,%_?pn,%_?n-1
          __xp s1
          __pp n,%_?pn-1
          exitm
        else
          _?n = _?n+1
          __ps t,%_?pt,%_?n-1
          __xp s1
          __pp t,%_?pt-1
          exitm
        endif
        endm

__ca1   macro s1,p1,p2
        _?opf = 0
        ifb <p1>
          __ca2 %_?opf,%_?pc-1,e,s1
          exitm
        else
          ifidn <p1>,<test>
            _?opf = 1
            ifb <p2>
              __ca2 %_?opf,%_?pc-1,e,s1
            else
              __ca2 %_?opf,%_?pc-1,p2,s1
            endif
          else
            __ca2 %_?opf,%_?pc-1,p1,s1
          endif
        endif
        endm

__ca2   macro op,sp,cd,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10
        $$oc&op <_$skc&sp>,<s1>,cd
        ifb <s2>
          __gjp $$&cd,%_?n-2
          exitm
        endif
        __gjp j&cd,%_?n-1
        __ca2 op,sp,cd,<s2>,<s3>,<s4>,<s5>,<s6>,<s7>,<s8>,<s9>,<s10>
        endm

__psc   macro sp,op
        _$skc&sp equ &op
        _?pc = _?pc+1
        endm

__pps   macro tp,sp,ln
        __lbl %_?sk&tp&sp
        _?sk&tp&sp = ln
        endm

__ps    macro tp,stp,nb
        _?sk&tp&stp = nb
        _?p&tp = _?p&tp+1
        endm

__pp    macro tp,sp
        __lbl %_?sk&tp&sp
        _?p&tp = _?p&tp-1
        endm

__jp    macro opj,tp,stp,se
        __gjp opj,%_?sk&tp&stp,se
        endm

__gjp   macro opj,ln,se
        opj  _@&ln&se
        endm

__lbl   macro ln,tp
_@&ln&tp:
        endm

__bkl   macro tp,stp
        __lbl %_?skb&stp,tp
        endm

__ckp   macro p1,p2,p3,p4,p5
        ifb <p1>
          exitm
        endif
        ifdef _??&p1
          _?lgf = _??&p1
        else
          ifidn <p1>,<test>
            _?opf = 1
          else
            ifidn <p1>,<end>
              _?end = 1
            else
              ifidn <p1>,<then>
                _?then = 1
                $$mac macro
                  __irp &p2
                endm
                __ckp p3,p4,p5
                exitm
              else
                _?exn = p1
              endif
            endif
          endif
        endif
        __ckp <p2>,<p3>,<p4>,<p5>
        endm

__xp    macro s1,s2,s3,s4,s5,s6,s7,s8,s9,sa,sb,sc
        ifb <s2>
          __x %_?opf,%_?dlg,%_?mf,s1
          exitm
        endif
        ifdif <s2>,<or>
          ifdif <s2>,<and>
            s1
            __xp <s2>,<s3>,<s4>,<s5>,<s6>,<s7>,<s8>,<s9>,<sa>,<sb>,<sc>
            exitm
          endif
        endif
        __x %_?opf,%_??&s2,%_?mf,s1
        __xp <s3>,<s4>,<s5>,<s6>,<s7>,<s8>,<s9>,<sa>,<sb>,<sc>
        endm

__x     macro op,lg,ex,s1,s2,s3,s4
        ifnb <s4>
            ifdef _$$&s1
                $$oc&op <_$$&s1 s2>,<s4>,s3
                __x&ex&lg s3
                exitm
            endif
            ifdef _$$&s3
                $$oc&op <s1>,<_$$&s3 s4>,s2
                __x&ex&lg s2
                exitm
            endif
            $$oc&op  <s1>,<s3 s4>,s2
            __x&ex&lg s2
            exitm
        endif
        ifnb <s3>
          $$oc&op  <s1>,<s3>,s2
          __x&ex&lg s2
          exitm
        endif
        ifnb <s2>
          or s1,s1
          __x&ex&lg s2
          exitm
        endif
        __x&ex&lg s1
        endm

__x01   macro cd
        __jp  j&cd,t,%_?pt-1
        endm
__x02   macro cd
        __jp  $$&cd,n,%_?pn-1
        endm

__x11   macro cd
        if _?lgf le 1
          __jp  j&cd,b,%_?pb-_?exn
        else
          __jp  j&cd,t,%_?pt-1
        endif
        endm
__x12   macro cd
        __jp  $$&cd,n,%_?pn-1
        endm

__x21   macro cd
        if _?lgf le 1
          if _?end
            __jp  j&cd,b,%_?pb-_?exn,e
          else
            __jp  j&cd,b,%_?pb-_?exn,s
          endif
        else
          __jp  j&cd,t,%_?pt-1
        endif
        endm
__x22   macro cd
        __jp  $$&cd,n,%_?pn-1
        endm

__x31   macro cd
        __jp  j&cd,t,%_?pt-1
        endm
__x32   macro cd
        if _?lgf eq 1
          __jp  $$&cd,n,%_?pn-1
        else
          __jp  $$&cd,b,%_?pb-1
        endif
        endm

__x41   macro cd
        __jp  j&cd,t,%_?pt-1
        endm
__x42   macro cd
        if _?lgf eq 1
          __jp  $$&cd,n,%_?pn-1
        else
          __jp  $$&cd,b,%_?pb-1,s
        endif
        endm

__x51   macro cd
        if _?lgf le 1
          __jp  j&cd,i,%_?pi-_?exn
        else
          __jp  j&cd,t,%_?pt-1
        endif
        endm
__x52   macro cd
        __jp  $$&cd,n,%_?pn-1
        endm

__x61   macro cd
        __jp  j&cd,t,%_?pt-1
        endm
__x62   macro cd
        __jp  $$&cd,n,%_?pn-1
        endm

__x71   macro cd
        if _?lgf le 1
          $$mac  j&cd
        else
          __jp j&cd,t,%_?pt-1
        endif
        endm
__x72   macro cd
        __jp  $$&cd,n,%_?pn-1
        endm


$$oc0   macro o1,o2,o3
        ifb <o3>
          cmp o1,o2
          exitm
        endif
        ifdif <o3>,<on>
          ifdif <o3>,<off>
            cmp o1,o2
            exitm
          endif
        endif
        test o1,o2
        endm
$$oc1   macro o1,o2,o3
        test o1,o2
        endm

$$a     macro lb
        jna  lb
        endm
$$nbe   macro lb
        jbe  lb
        endm
$$ae    macro lb
        jnae lb
        endm
$$nb    macro lb
        jb   lb
        endm
$$be    macro lb
        jnbe lb
        endm
$$na    macro lb
        ja   lb
        endm
$$b     macro lb
        jnb  lb
        endm
$$nae   macro lb
        jae  lb
        endm
$$g     macro lb
        jng  lb
        endm
$$nle   macro lb
        jle  lb
        endm
$$ge    macro lb
        jnge lb
        endm
$$nl    macro lb
        jl  lb
        endm
$$le    macro lb
        jnle lb
        endm
$$ng    macro lb
        jg lb
        endm
$$l     macro lb
        jnl lb
        endm
$$nge   macro lb
        jge lb
        endm
$$c     macro lb
        jnc lb
        endm
$$nc    macro lb
        jc lb
        endm
$$e     macro lb
        jne lb
        endm
$$ne    macro lb
        je lb
        endm
$$z     macro lb
        jnz  lb
        endm
$$nz    macro lb
        jz lb
        endm
$$no    macro lb
        jo  lb
        endm
$$o     macro lb
        jno lb
        endm
$$ns    macro lb
        js lb
        endm
$$s     macro lb
        jns lb
        endm
$$np    macro lb
        jp lb
        endm
$$p     macro lb
        jnp lb
        endm
$$po    macro lb
        jpe lb
        endm
$$pe    macro lb
        jpo lb
        endm
$$cxnz  macro lb
        jcxz lb
        endm
$$cxz   macro lb
        local llb
        jcxz  llb
        jmp   lb
llb:
        endm
jcxnz   macro lb
        $$cxz lb
        endm
jon     macro lb
        jnz lb
        endm
joff    macro lb
        jz  lb
        endm
$$on    macro lb
        jz  lb
        endm
$$off   macro lb
        jnz  lb
        endm

.sall
.list

.xlist
;
; 構造化マクロ 'struc'  Ver 2.40    30/Aug/1989
;   TASM(masm/ideal)依存マクロ
;
;  Copyright (C)1989 Kacchan
;                    JUP1076D(JUPITER)
;                    NBC00034(NIFty-Serve)
;                    QNG33261(PC-VAN)
;
_?ideal = 0

_ideal  macro
        _?ideal = 1
        ideal
        endm

_masm   macro
        _?ideal = 0
        masm
        endm

__oncall macro  oc,lb,cd,st,p1,p2,p3
        if _?ideal
          macro $$mac
           __irp &st
           &oc   &lb
          endm
        else
          $$mac macro
           __irp &st
           &oc   &lb
          endm
        endif
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
          if _?ideal
            macro $$mac oc
             oc &lb
            endm
          else
            $$mac macro oc
             oc &lb
            endm
          endif
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

__dumy  macro nm
        if _?ideal
          macro nm
          endm
        else
          nm macro
          endm
        endif
        endm

__defmm macro nm,st
        if _?ideal
          macro nm
          __rev &st
          endm
        else
          nm macro
          __rev &st
          endm
        endif
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
                if _?ideal
                  macro $$mac
                  __irp &p2
                  endm
                else
                  $$mac macro
                  __irp &p2
                  endm
                endif
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
.list

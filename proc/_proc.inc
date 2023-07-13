_$ProcVersion  equ 2.83
;-----------------------------------------------------------------
;  Procedure / Prototype / Call Macro
;      for OPTASM 1.5 / MASM 5.1
_$MSG equ <PROC.INC (Rev $1) Mar.24, 1990  written by Oka Toshiyuki>
;										enhanced by Kacchan
; KEYWORDS:
;	_proc, _func, _local, _use, _nosave, _call, _goto, _label
;   _return, _endp, _quit, .quit, _skip, .skip
;
; NOTE:
;   This file is in the public domain.
;   Anyone can freely use, duplicate, rewrite and distribute this
;  file without any permission from original author nor rewriters.
;-----------------------------------------------------------------
.xlist

;-----------------------------------------------------------------
;    Version Number Check
;-----------------------------------------------------------------
_CheckProcVersion  macro  v
	if v gt _$ProcVersion
		.err
		_$VerErr %_$ProcVersion
	endif
endm

_$VerErr  macro  ver
	%out  -- Version Error : My version is ver --
endm

;-----------------------------------------------------------------
;			No NOSAVE
;-----------------------------------------------------------------
_$NOSAVE equ <>
_$nsv = 0

;-----------------------------------------------------------------
;      Strict Parameter Check
;-----------------------------------------------------------------
_$STRICT_CHECK = 0

;-----------------------------------------------------------------
;     Declare Extrn When Refered
;-----------------------------------------------------------------
_$EXTRN_WHEN_REFERRED = 0

;-----------------------------------------------------------------
;     Declare SOFT Extrn
;-----------------------------------------------------------------
_$SOFT_EXTRN = 0

;-----------------------------------------------------------------
;         Define Register
;-----------------------------------------------------------------
_$reg$cs = 0
_$reg$ds = 0
_$reg$es = 0
_$reg$ss = 0
_$reg$ax = 1
_$reg$bx = 1
_$reg$cx = 1
_$reg$dx = 1
_$reg$si = 1
_$reg$di = 1
_$reg$bp = 1
_$reg$sp = 1
_$reg$al = 2
_$reg$ah = 3
_$reg$bl = 2
_$reg$bh = 3
_$reg$cl = 2
_$reg$ch = 3
_$reg$dl = 2
_$reg$dh = 3

_$msk$ax = 001h
_$msk$cx = 002h
_$msk$dx = 004h
_$msk$bx = 008h
_$msk$al = _$msk$ax
_$msk$ah = _$msk$ax
_$msk$cl = _$msk$cx
_$msk$ch = _$msk$cx
_$msk$dl = _$msk$dx
_$msk$dh = _$msk$dx
_$msk$bl = _$msk$bx
_$msk$bh = _$msk$bx
_$msk$si = 010h
_$msk$di = 020h
_$msk$bp = 040h
_$msk$sp = 080h
_$msk$cs = 100h
_$msk$ds = 200h
_$msk$es = 400h
_$msk$ss = 800h

_$low$ax  equ <al>
_$low$bx  equ <bl>
_$low$cx  equ <cl>
_$low$dx  equ <dl>
_$high$ax equ <ah>
_$high$bx equ <bh>
_$high$cx equ <ch>
_$high$dx equ <dh>

_$low$0   equ <al>
_$low$1   equ <cl>
_$low$2   equ <dl>
_$low$3   equ <bl>
_$high$0  equ <ah>
_$high$1  equ <ch>
_$high$2  equ <dh>
_$high$3  equ <bh>

_$wreg$ax equ <ax>
_$wreg$bx equ <bx>
_$wreg$cx equ <cx>
_$wreg$dx equ <dx>
_$wreg$si equ <si>
_$wreg$di equ <di>
_$wreg$bp equ <bp>
_$wreg$sp equ <sp>
_$wreg$cs equ <cs>
_$wreg$ds equ <ds>
_$wreg$es equ <es>
_$wreg$ss equ <ss>
_$wreg$al equ <ax>
_$wreg$ah equ <ax>
_$wreg$bl equ <bx>
_$wreg$bh equ <bx>
_$wreg$cl equ <cx>
_$wreg$ch equ <cx>
_$wreg$dl equ <dx>
_$wreg$dh equ <dx>

_$wreg$0 equ <ax>
_$wreg$1 equ <cx>
_$wreg$2 equ <dx>
_$wreg$3 equ <bx>
_$wreg$4 equ <si>
_$wreg$5 equ <di>
_$wreg$6 equ <bp>

;-----------------------------------------------------------------
;    Procedure index number
;-----------------------------------------------------------------
ifndef _$pnum
	_$pnum = 0									; procedure index number
endif

;-----------------------------------------------------------------
; _proc : Define/Declare Procedure
;		  m : procedure name
;		  p : formal parameter list 
;		  t : proc type (near or far)
;		  e : external
; (public,extrn,global,local or forward)
;		  u : used register list
; ex.
; _proc foo <al,dx> far extrn <ax>
;-----------------------------------------------------------------
_proc  macro  m, p, t, e, u
	_$proc  m, <p>, <>, <e>, <u>, t
endm


;-----------------------------------------------------------------
; _func : Define/Declare Function
;		  m : function name 
;		  p : formal parameter list 
;		  r : retrun register list
;		  t : proc type (near or far)
;		  e : external
; (public,extrn,global,local or forward)
;		  u : used register list
; ex.
; _func foo <si,di> <ax> near global
;-----------------------------------------------------------------
_func  macro  m, p, r, t, e, u
	_$proc  m, <p>, <r>, <e>, <u>, t
endm


_$proc  macro  m, p, r, e, u, t, l
	ifndef _$n$&m								; _$n$?? is the number of procedure
		_$pnum = _$pnum + 1
		_$Equ  _$n$&m, %_$pnum
	endif
    ifdifi <l>,<pascal>
		ifdef _$POSTFIX_UNDERSCORE
			_$clbl  equ  <&m&_>					; _$clbl : Current proc label
		else
			_$clbl  equ  <_&m>
		endif
	else
		_$clbl  equ  <m>
	endif
	_$proc1  %_$n$&m, m, %_$clbl, <p>, <r>, <e>, <u>, t, l
endm

_$proc1 macro  i, m, pn, p, r, e, u, t, l
	_$cpn  equ  <i>								; _$cpn : Current proc number
	_$ext$&i = 1								; extrn declared
	_$lbl$&i  equ  <pn>							; _$lbl$?? : Procedure label
	_$res = 1
	ifdef _$p&i
	ifndef NOPRMCHK
		_$res = 0
	endif
	endif
	ifnb <e>									; External defined
		ifdifi <e>, <local>
			ifidni <e>, <global>
				public  pn
			else
				_$res = _$res + 2
				ifidni <e>, <extrn>
					if _$EXTRN_WHEN_REFERRED
						_$ext$&i = 0
					else
						_$DeclExtrn pn,t
					endif
				else
				ifidni <e>, <public>
					public  pn
				else
				ifdifi <e>, <forward>
					.err
					%out  -- m e : Illegal external type --
					exitm
				endif
				endif
				endif
			endif
		endif
	else										; External not defined
		ifdef ALLGLOBAL
			public  pn							; globalize all symbols
		endif
	endif
	if _$res									; Prototype declaretion
		_$proto  i, m, <p>, <r>, <e>, <u>, t, l
		if _$res GE 2
			exitm
		endif
	else
		IF1
			_$ChkPrm  m, <p>, %_$p&i
			_$ChkRet  m, <r>, %_$r&i
			_$ChkTyp  m, <t>, %_$t&i
			_$ChkLng  m, <l>, %_$l&i
		endif
	endif
	_$retregs  equ  <r>
	_$SetRegMask  %_$NOSAVE
	_$nsv = _$msk
	_$uses   equ <>
	_$locals equ <>
	_$AnalProto  i, <p>, <r>, <u>
	_$svd = _$sff								; stack frame flag
	if _$svd
	    ifidni <l>,<pascal>						; Pascal Convention
			_$Reverse %_$v&i, _$lst
			_$DefStkPrm  i, %_$lst, t			; Define parameter variables
		else
			_$DefStkPrm  i, %_$v&i, t			; Define parameter variables
		endif
		_$svd = _$dsp
	endif
	_$name  equ  <m>
	ifidni <t>,<interrupt>
		pn  proc  far
	else
		pn  proc  t								; proc statement
	endif
	if _$svd
		push  bp
		mov   bp, sp							; make stack frame
	endif
	_$tmpf = 0									; stack temp reg not exist.
	_$dsp  = 2									; for stack temp.
	_$localcont = 1								; for first _local
endm

_$DeclExtrn macro pn,t
	ifb <t>
		_$DeclExt2 pn,<proc>
	else
		ifidni <t>,<interrupt>
			_$DeclExt2 pn,<far>
		else
			_$DeclExt2 pn,t
		endif
	endif
endm

_$DeclExt2 macro pn,t
	if _$SOFT_EXTRN
		soft extrn  pn : t
	else
		extrn  pn : t
	endif
endm

_$proto  macro  i, m, p, r, e, u, t, l
	_$p&i  equ  <p>
	_$r&i  equ  <r>
	_$t&i  equ  <t>
	_$u&i  equ  <u>
	_$l&i  equ  <l>								; language
	_$n&i  =  -1								; # of parameter
	ifdifi <l>,<pascal>
		_$DefMac  m
	endif
endm


_$DefMac  macro  m								; define call macro
	m  macro  a, r, n
		_call  m <a>, <r>, <n>
	endm
endm

_$ChkPrm  macro  m, x, y
	_$CompPair <x>, <y>
	ife _$res
		.err
		%out  -- m : parameter register(s) mismatch --
	endif
endm

_$ChkRet  macro  m, x, y
	_$CompPair <x>, <y>
	ife _$res
		.err
		%out  -- m : return register(s) mismatch --
	endif
endm

_$CompPair macro x, y
	_$res = 1
	_$cntx = 0
	_$cnty = 0
	irp $vx, <x>
		_$cntx = _$cntx + 1
		_$cnty = 0
		irp $vy, <y>
			_$cnty = _$cnty + 1
			if _$cntx eq _$cnty
				ifdifi <$vx>,<$vy>
					_$res = 0
				endif
				exitm
			endif
		endm
	endm
	if _$cntx ne _$cnty
		_$res = 0
	endif
endm

_$ChkTyp  macro  m, x, y
	ifdifi <x>, <y>
		.err
		%out  -- m : procedure type mismatch --
	endif
endm

_$ChkLng  macro  m, x, y
	ifdifi <x>, <y>
		.err
		%out  -- m : language type mismatch --
	endif
endm
;
; define parameter variables
; i:pnum, lst:var list, t:proc type
;
_$DefStkPrm  macro  i, lst, pt
	_$cnt = 0
	_$dsp = 0
	ifidni <pt>, <far>
		_$base = 6
	else
		_$base = 4
	endif
	irp  $x, <lst>
		_$cnt = _$cnt + 1
		if _$cnt ge _$vp&i
			exitm
		endif
		_$DefPrm  %(_$base+_$dsp), %_$p&i&$v&&$x, %_$p&i&$t&&$x
	endm
endm

_$DefPrm  macro  d, v, t
	ifnb <t>
		ifidni <t>, <dword>
			_$dsp = _$dsp + 4
		else
			_$dsp = _$dsp + 2
		endif
		v  equ  <(t ptr [bp+d])>
	else
		_$dsp = _$dsp + 2
		v  equ  <([bp+d])>
	endif
	???&v = 1
	_$Concat  _$locals, %_$locals, v
endm

; -- prototype analysis --
_$AnalProto  macro  i, p, r, u
	_$cnt = 0									; param count
	_$prm = 0									; param which reg mask
	_$sm = 0									; segreg pos mask
	_$vm = 0									; var pos mask
	_$sn = 0									; # of segreg
	_$sff = 0									; stack frame frag
	_$slst equ <>								; seg reg list
	_$vlst equ <>								; var list
	_$vp&i = 1000								; va start pos
	irp $x, <p>
		_$cnt = _$cnt + 1
		ifdifi <$x>, <...>
			_$AP2  i, $x
		else
			_$vp&i = _$cnt
			rept 16
				_$AP2  i, <$v$>,<var>
				if _$cnt ge 16
					exitm
				endif
				_$cnt = _$cnt + 1
			endm
			exitm
		endif
	endm
	_$n&i = _$cnt								; # of parameter
	_$Equ  _$s&i, %_$slst
	_$Equ  _$v&i, %_$vlst
	_$prm&i =  _$prm							; param which reg mask
	_$sm&i  =  _$sm								; segreg pos mask
	_$sn&i  =  _$sn								; # of segreg
	_$vm&i  =  _$vm								; var pos mask
	_$SetRegMask  <r>
	_$rm&i = _$msk
	if _$res
		%out -- duplicate retrun registers --
	endif
	_$SetRegMask  <u>
	_$um&i = _$msk
	if _$res
		%out -- duplicate using registers --
	endif
endm

_$AP2  macro  i, x, y							; x:varname, y:vartype
	_$DefNthPrm  i, %_$cnt, x, y				; define Nth param var
	ifdef _$reg$&x								; -- Register --
		if _$prm and _$msk$&x
			.err
			%out -- duplicate parameter registers --
		endif
		_$prm = _$prm or _$msk$&x
		_$AP2a  i, %_$wreg$&x, %_$cnt
		ife _$reg$&x							; segreg ?
			_$AP3								; make _$slst
		endif
	else										; -- Variable --
		_$p&i&$&x = _$cnt
		_$AP4									; make _$vlst
	endif
endm

_$AP2a  macro  i, x, n
	_$p&i&$&x = n
endm

_$AP3  macro									; make _$slst
	_$Concat  _$slst, %_$slst, %_$cnt
	_$sm = _$sm or (1 shl (_$cnt-1))
	_$sn = _$sn + 1
endm

_$AP4  macro									; make _$vlst
	_$Concat  _$vlst, %_$vlst, %_$cnt
	_$vm  = _$vm or (1 shl (_$cnt-1))
	_$sff = 1
endm

_$DefNthPrm  macro  i, n, x, y
	_$p&i&$v&n  equ  <x>
	_$p&i&$t&n  equ  <y>
endm


;-----------------------------------------------------------------
; _local : Declare local variables
;		   v : variable list
;		   b : stack bottom 
;		   p : stack overflow handler
;-----------------------------------------------------------------
_local  macro  v, b, p
	local _$ok

	ife _$localcont
		.err
		%out  -- multiple _locals without ',c' --
	endif
	_$localcont = 0
	_$ChkUse  %_$uses
	ife _$svd
		_$svd = -1
		push  bp
		mov   bp, sp
	endif
	_$tmpf = 1									; stack temp reg exist.
	irp  $x, <v>
		_$DefVar  $x
	endm
	ifnb <b>
		ifidni <b>, <c>
			_$localcont = 1
		else
			push  ax
			mov   ax, sp
			sub   ax, _$dsp
			cmp   ax, b
			pop   ax
			ja    _$ok
			ifnb <p>
				p
			endif
			_return
		 _$ok:
		endif
	endif
	ife _$localcont
		sub   sp, _$dsp
	endif
endm

_$ChkUse  macro  r
	ifnb <r>
		.err
		%out  -- _local must precede _use --
	endif
endm

_$DefVar  macro  v, s, n
	ifb <s>
		_$dsp = _$dsp + 2
	else
		ifb <n>
			_$nv = 1
		else
			_$nv = n
		endif
		ifidni <s>, <word>
				_$dsp = _$dsp + _$nv*2
		else
			ifidni <s>, <byte>
				_$dsp = _$dsp + ((_$nv+1) and 0FFFEh)
		else
			ifidni <s>, <dword>
				_$dsp = _$dsp + _$nv*4
		else
				_$dsp = _$dsp + s*2
				_$DV2  v, <>, %_$dsp
				exitm
		endif
		endif
		endif
	endif
	_$DV2  v, s, %_$dsp
endm

_$DV2  macro  v, s, d
	ifnb <s>
		v  equ  <(s ptr [bp-d])>
	else
		v  equ  <([bp-d])>
	endif
	???&v = 1
	_$Concat  _$locals, %_$locals, v
endm


;-----------------------------------------------------------------
; _usef: Declare Using Flag macro
; _use : Declare Using Register macro
;	 r : register list
;	 s : nosave or blank
;-----------------------------------------------------------------
_usef  macro
	pushf
	_$Concat _$uses, %_$uses, <f>
endm

_use  macro  r, s
	_$PushReg  <r>
	_$Concat _$uses, %_$uses, <r>
	ifidni <s>, <nosave>
		_$SetRegMask  <r>
		_$nsv = _$nsv or _$msk
	endif
	_$ChkDupl %_$name, %_$retregs, <r>
endm

_$ChkDupl macro  m, rr, ur
	_$res = 0
	irp $x,<rr>
		_$res = _$res or _$msk$&&$x
	endm
	if _$res
		irp $y,<ur>
			if _$res and _$msk$&&$y
				.err
				%out  -- m : return register(s) must not be saved --
				exitm
			endif
		endm
	endif
endm

;-----------------------------------------------------------------
; _nosave :Declare destroyed registers
;	 r : register list
;-----------------------------------------------------------------
_nosave  macro  r
	_$SetRegMask  <r>
	_$nsv = _$nsv or _$msk
endm


;-----------------------------------------------------------------
; _goto : goto 
; _label : define local label
; _retlabel : _return and _label
;  1) goto label
;  2) goto label, cond
;  When you use the second form, you have to
;  include 'struc.inc'(ver 2.3 or later), too.
;-----------------------------------------------------------------
_goto  macro  lbl, cd,p1,p2
	_$goto  %_$cpn, lbl, <cd>,<p1>,<p2>
endm

_$goto  macro  i, l, cd,p1,p2
	ifb <cd>
		jmp		l&$&i
	else
		_on jump l&$&i,<cd>,<>,<p1>,<p2>
	endif
endm

_label  macro  lbl
	_$label  lbl, %_$cpn
endm

_retlabel  macro  lbl
	_$return  %_$cpn
	_$label  lbl, %_$cpn
endm

_$label  macro  l, i
l&$&i:
endm

;-----------------------------------------------------------------
; _quit[n] : goto quit label
; .quit[n] : define quit label
; _skip[n] : goto skip label
; .skip[n] : define skip label
;  1) _quit[n], _skip[n]
;  2) _quit[n] cond, _skip[n] cond
;  n = 1,2,3,4,5
;  When you use the second form, you have to
;  include 'struc.inc'(ver 2.3 or later), too.
;-----------------------------------------------------------------
IRP $x, <,1,2,3,4,5>
	_quit&$x  macro  cd,p1,p2
		_$goto  %_$cpn, _$quit&$x, <cd>,<p1>,<p2>
	endm
	.quit&$x  macro
		_$label  _$quit&$x, %_$cpn
	endm
	_skip&$x  macro  cd,p1,p2
		_$goto  %_$cpn, _$skip&$x, <cd>,<p1>,<p2>
	endm
	.skip&$x  macro
		_$label  _$skip&$x, %_$cpn
	endm
ENDM

;-----------------------------------------------------------------
; _return : Return from procedure
;  1) _return
;  2) _return cond
;  When you use the second form, you have to
;  include 'struc.inc'(ver 2.3 or later), too.
;-----------------------------------------------------------------
_return  macro  cd,p1,p2
	_$return  %_$cpn, <cd>,<p1>,<p2>
endm

_$return  macro  i, cd,p1,p2
	ifb <cd>
		jmp  _$end&i
	else
		_on jump _$end&i,<cd>,<>,<p1>,<p2>
	endif
endm

;-----------------------------------------------------------------
; _endp : End Procedure macro
;-----------------------------------------------------------------
_endp  macro  n
	_$endp  %_$cpn, %_$name, n
	_$nsv = 0
endm

_$endp  macro  i, m, n
_$end&i:
	_$PopReg  %_$uses
	if _$svd									; if exist stack frame ?
			mov   sp, bp
			pop   bp
	endif
	ifb <n>
		_$RetInstr %_$t&i, %_$l&i
	else
		ifdifi <n>, <0>
			ret 	n
		endif
	endif
	_$Unlocal  %_$locals						; undefine local vars
	_$clbl  endp								; ENDP
endm

_$RetInstr  macro  t, l
	ifidni <t>,<interrupt>
		iret
	else
		ifidni <l>,<pascal>
			if _$svd gt 0
				ret _$svd
			else
				ret
			endif
		else
			ret
		endif
	endif
endm

_$Unlocal  macro  l
	irp $x, <l>
		$x  equ  <#######>
		???&&$x = 0
	endm
endm


;-----------------------------------------------------------------
; _call : Call Procedure macro  
;	  m : proc name 
;	  a : argument list 
;	  r : return register list
;	  s : save register list
;-----------------------------------------------------------------
_call  macro  m, a, r, s
	ifndef  _$n$&m
		%out  -- call m: no prototype --
		call  m
	else
		_$call0  %_$n$&m, m, <a>, <r>, <s>
	endif
endm

_$call0  macro  i, m, a, r, s
	_$call  i, m, %_$lbl$&i, <a>, <r>, <s>
endm

_$call  macro  i, m, pn, a, r, s
	ife _$ext$&i
		_$DeclExtrn  pn, %_$t&i
		_$ext$&i = 1
	endif
	if _$n&i eq -1
		_$AnalProto  i, %_$p&i, %_$r&i, %_$u&i
	endif
	if _$STRICT_CHECK
		_$ChkRet  m, <r>, %_$r&i
	else
		ifnb <r>
			_$ChkRet  m, <r>, %_$r&i
		endif
	endif
	_$ExplodeArg  <a>
	if (_$argn ne _$n&i) and (_$vp&i eq 1000)
		.err
		%out  -- m : number of argument not match --
	else
		_$ClassifyArg  i, %_$argn, %_$u&i, <s>
		_$SregList i, %_$s&i
		_$PushReg  %_$psreg
		_$PushReg  %_$sav
		_$StackVar i, %_$v&i, %_$l&i
		_$PushReg  %_$asreg
		_$MovePrm  i, %_$mov
		_$XchgReg  i, %_$xchg
		_$PopReg   %_$psreg
		call  pn
		_$RestReg  %_$sfd, %_$l&i
		_$PopReg   %_$sav
		_$PopReg   %_$psreg
	endif
endm

_$ExplodeArg  macro  a
	_$rp = 0
	_$wrph = 0									; word reg flag (high)
	_$wrpl = 0									; word reg flag (low)
	_$arm = 0									; which reg mask
	_$msk = 1
	_$argn = 0
	irp $x, <a>
		_$argn = _$argn + 1
		_$ExplArg  %_$argn, <$x>, $x
		_$msk = _$msk shl 1
	endm
	_$nwrp = not (_$wrph and _$wrpl)
endm

_$ExplArg  macro  n, x, z1,z2,z3,z4,z5
	_$argt&n  equ  <>
	_$arg&n   equ  <x>
	_$CheckReg <z1>, h
	ifnb <z2>
		ife _$rp and _$msk
			_$DecmpArg  n, <x>, x
			exitm
		endif
		_$CheckReg <z2>, l
		_$rp = _$rp and (not _$msk)
	else
		_$wrpl = _$wrpl or _$msk
	endif
endm

_$CheckReg  macro  x, f
	_$cnt = 0
	irpc $y, <x>
		ifidni <$y>,<->
			exitm
		endif
		ifidni <$y>,<+>
			exitm
		endif
		_$cnt = _$cnt + 1
		if _$cnt gt 2
			exitm
		endif
	endm
	if _$cnt eq 2
		ifdef _$wreg$&x
			_$rp = _$rp or _$msk
			_$arm = _$arm or _$msk$&x
			if _$reg$&x le 1
				_$wrp&f = _$wrp&f or _$msk
			endif
		endif
	endif
endm

_$DecmpArg  macro  n, x, z1,z2,z3,z4,z5			; decomposite arg name and type
	irp  _$y,<byte,word,dword,ea,addr>
		ifidni <_$y>,<z1>
			_$argt&&n equ  <z1>
			_$arg&&n  equ  <z2>
			exitm
		endif
	endm
endm

_$SregList  macro  i, l
	_$asreg  equ  <>
	_$psreg  equ  <>
	irp $x, <l>
		_$SL1  %_$arg&&$x, %_$p&i&$v&&$x
	endm
endm

_$SL1  macro  x, y
	ifdifi <x>, <y>
		_$Concat  _$asreg, %_$asreg, <x>
		_$Concat  _$psreg, %_$psreg, <y>
	endif
endm


_$ClassifyArg  macro  i, n, u, s
	_$mov    equ  <>
	_$xchg   equ  <>
	_$sav    equ  <s>
	_$SetRegMask  <s>
	_$savreg  =  _$msk
	ife _$nsv									; no nosave
		_$Concat  _$sav, %_$sav, <u>
	else
		_$savreg = _$savreg or _$nsv
		irp $x, <u>
			_$AppendSav  $x
		endm
	endif
	_$savreg = _$savreg or _$um&i or _$rm&i
	_$mark = _$sm&i or _$vm&i					; mark seg reg and variable position
	_$mask = 1
	_$cnt  = 0
	rept &n
		_$cnt = _$cnt + 1
		ife _$mark and _$mask					; yet examined ?
			_$mark  = _$mark or _$mask
			_$cycle = _$mask
			_$lst  equ  <>
			_$CA2  i, %_$cnt
		endif
		_$mask = _$mask shl 1
	endm
endm

_$CA2  macro  i, n
	_$CA2a  i, %_$arg&n, %_$p&i&$v&n, n
endm

_$CA2a  macro  i, x, y, n						; x:Nth arg, y:Nth prm, n:N
	if _$rp and (1 shl (n-1))					; x is reg ?
		_$CA3  i, <x>, %_$wreg$&x, <y>, n
	else
		_$CA3  i, <x>, <>, <y>, n
	endif
endm

_$CA3  macro  i, x, xw, y, n					; x:Nth arg, y:Nth prm, n:N
	ifdifi <x>, <y>
		_$AppendSav  <y>
		_$Concat  _$lst, %_$lst, n
		ifnb <xw>								; x is reg ?
			if _$prm&i and _$msk$&xw
				_$CA4  i, %_$p&i&$&xw
			else
				_$Concat  _$mov, %_$lst, %_$mov
			endif
		else
			_$Concat  _$mov, %_$lst, %_$mov
		endif
	else
		ifdef _$SAVE_ALL
			_$AppendSav  <y>
		endif
	endif
endm

_$CA4  macro  i, n
	_$msk = 1 shl (n -1)
	if _$cycle and _$msk
		_$Separate  %_$lst, n
	else
		if _$mark and _$msk
			_$Concat  _$mov, %_$lst, %_$mov
		else
			_$mark = _$mark or _$msk
			_$cycle = _$cycle or _$msk
			_$CA2  i, n
		endif
	endif
endm

_$Separate  macro  l, n
	_$res = 0
	_$lst  equ  <>
	irp $x, <l>
		ife _$res
			ifidni <$x>, <n>
				_$Concat  _$mov, %_$lst, %_$mov
				_$res = 1
				_$Concat  _$xchg, %_$xchg, <$x>
			else
				_$Concat  _$lst, %_$lst, <$x>
			endif
		else
			_$Concat  _$xchg, %_$xchg, <$x>
		endif
	endm
endm

_$AppendSav  macro  x
	ife _$savreg and _$msk$&x
		_$savreg = _$savreg or _$msk$&x
		_$Concat  _$sav, %_$sav, %_$wreg$&x
	endif
endm

_$XchgReg  macro  i, l
	_$msk = 0
	irp $x,<l>
		_$XR1  %_$arg&&$x, %_$p&i&$v&&$x
	endm
endm

_$XR1  macro  x, y
	ife _$msk
		if _$msk$&x ne _$msk$&y
			_$msk = _$msk$&y
			push _$wreg$&y
		endif
		mov  y, x
	else
		if _$msk$&x eq _$msk
			pop  _$wreg$&y
			if _$reg$&y ge 2					; byte reg ?
				if _$reg$&x ne _$reg$&y
					irpc $z,<y>
						xchg  &&$z&&l, &&$z&&h
						exitm
					endm
				endif
			endif
			_$msk = 0
		else
			mov  y, x
		endif
	endif
endm

_$MovePrm  macro  i, l
	irp $x, <l>
		_$MP  $x, %_$argt&&$x, %_$arg&&$x, %_$p&i&$v&&$x
	endm
endm

_$MP  macro  n, t, x, y
	if _$rp and (1 shl (n-1))
		if _$reg$&x or _$reg$&y
			mov   y, x
		else									; both seg reg
			push  x
			pop   y
		endif
	else
		_$MP2  y, <x>, t
	endif
endm

_$MP2  macro  y, x, t
	ifidni <t>,<ea>
			lea  y, x
	else
	ifidni <t>,<addr>
			lea  y, x
	else
	ifidni <t>,<byte>
		if _$reg$&y le 1						; word reg ?
			mov  _$low$&y, byte ptr (x)
			mov  _$high$&y, 0
		else									; byte reg
			mov  y, byte ptr (x)
		endif
	else
	ifidni <t>,<word>
			mov  y, word ptr (x)
	else
		ifidni <x>,<0>
			xor  y, y
		else
			mov  y, x
		endif
	endif
	endif
	endif
	endif
endm
;
; Push variables into stack
; i:proc id, vl:var list, lng:langage
;
_$StackVar  macro  i, vl, lng
	_$tmpf1 = 0									; temp reg assigned ?
	_$sfd = 0									; stack frame depth
	ifnb <vl>
		ifdifi <lng>,<pascal>
			_$Reverse  <vl>, _$lst				; Reverse list
			_$SV1  i, %_$lst
		else
			_$SV1  i, <vl>
		endif
	endif
endm

_$SV1  macro  i, l
	if _$nwrp and _$vm&i						; need to make temp reg ?
		_$SV1a  _$cnt							; find free reg (AX, CX, DX)
		if _$cnt ge 3							; not found
			if _$tmpf							; stack temp reg exist ?
				_$tmpreg   equ  <word ptr [bp-2]>
				_$tmpregl  equ  <byte ptr [bp-2]>
				_$tmpregh  equ  <byte ptr [bp-1]>
				_$tmpf1 = 7
			else
				_$tmpreg  equ  <>
				.err
				%out -- declare _local --
				exitm
			endif
		else
			_$SV2  %_$cnt						; set _$tmpreg
		endif
		if (_$tmpf1 and 7)						; temp reg is not ax ?
			mov  _$tmpreg, ax
		endif
	endif
	_$axdty = 0									; dirty flag
	irp $x, <l>
		if _$argn ge $x
			if _$rp and (1 shl ($x-1))
				_$SV3  $x, %_$argt&&$x, %_$arg&&$x, %_$p&i&$t&&$x
			else
				_$SV4  $x, %_$argt&&$x, %_$arg&&$x, %_$p&i&$t&&$x
			endif
		endif
	endm
	if (_$tmpf1 and 7)							; temp reg was not ax ?
		mov  ax, _$tmpreg
	endif
endm

_$SV1a macro v
	v = 0
	irp $x,<1,2,4>
		ife _$arm and $x
			exitm
		endif
		v = v + 1
	endm
endm

_$SV2  macro  n									; set tmp reg
	_$tmpreg   equ  <_$wreg$&n>
	_$tmpregl  equ  <_$low$&n>
	_$tmpregh  equ  <_$high$&n>
	_$tmpf1 = n + 8
	ife _$savreg and (1 shl n)
		push  _$tmpreg
		_$tmpf1 = _$tmpf1 + 8
	endif
endm

_$SV3  macro  n, at, r, t						; argument is a register
	ifidni <t>, <dword>
		.err
		%out  -- parameter is double word type --
		exitm
	endif
	if _$reg$&r le 1							; word reg
		ifidni <t>, <byte>
			.err
			%out  -- parameter is byte type --
			exitm
		endif
		_$SV3a  r
	else										; byte reg
		ifidni <t>, <word>
			.err
			%out  -- parameter is word type --
			exitm
		endif
		_$SV3b  r
	endif
endm

_$SV3a macro  r									; word reg
	ifidni <r>, <ax>
		if _$axdty
			push  _$tmpreg
		else
			push  ax
		endif
	else
		push  r
	endif
	_$sfd = _$sfd + 2
endm

_$SV3b macro  r									; byte reg
	ifidni <r>, <ah>
		if _$axdty
			mov  al, _$tmpregh
		else
			mov  al, ah
		endif
	else
	  ifidni <r>, <al>
		if _$axdty
			mov  al, _$tmpregl
		endif
	  else
		mov  al, r
	  endif
	endif
	mov  ah, 0
	push  ax
	_$axdty = 1
	_$sfd = _$sfd + 2
endm

_$SV4  macro  n, at, a, t						; argument is not a register
	ifidni <t>, <var>
		_$SV4v n, at, a
	else
		ifidni <t>, <dword>
			_$SV5  n, at, a
		else
			ifidni <t>, <byte>
				_$MP2  al, <a>, at
				mov  ah, 0
			else
				_$MP2  ax, <a>, at
			endif
			push  ax
			_$axdty = 1
			_$sfd = _$sfd + 2
		endif
	endif
endm

_$SV4v  macro  n, at, h, l						; parameter type is variable
	ifnb <l>
		_$SV5  n, <at>, <h>, <l>
	else
		ifidni <at>, <addr>
			_$SV5  n, <at>, <h>
		else
			ifidni <at>, <dword>
				_$SV5  n, <at>, <h>
			else
				_$MP2  ax, <h>, <at>
				push  ax
				_$axdty = 1
				_$sfd = _$sfd + 2
			endif
		endif
	endif
endm

_$SV5  macro  n, at, h, l						; parameter type is double word
	ifnb <l>
		_$SV6  n, <h>, _$wrph
		_$SV6  n, <l>, _$wrpl
	else
		ifidni <at>,<addr>
			ifndef ???&h
				mov   ax, SEG h
				push  ax
				mov   ax, OFFSET h
				push  ax
			else
				ife ???&h
					mov   ax, SEG h
					push  ax
					mov   ax, OFFSET h
					push  ax
				else
					push  ss
					lea   ax, h
					push  ax
				endif
			endif
			_$axdty = 1
		else
			ifidni <at>,<dword>
				push  word ptr h +2
				push  word ptr h
			else
				ifidni <at>,<const>
					mov  ax, h
					push ax
					xor  ax
					push ax
				else
					ifb <at>
						ifidni <h>, <0>
							xor  ax, ax
							push ax
							push ax
						else
							_$IsNumber h
							if _$res
								mov  ax, h
								push ax
								xor  ax, ax
								push ax
							else
								push  word ptr h +2
								push  word ptr h
							endif
						endif
					else
						.err
						%out  -- parameter is double word type --
					endif
				endif
			endif
		endif
		_$sfd = _$sfd + 4
	endif
endm

_$SV6  macro  n, x, m
	if m and (1 shl (n-1))
		_$SV3a  <x>
	else
		_$argt0  equ  <>
		_$arg0   equ  <x>
		_$DecmpArg  0,<x>,x
		_$MP2  ax, %_$arg0, %_$argt0
		push  ax
		_$axdty = 1
		_$sfd = _$sfd + 2
	endif
endm

_$RestReg  macro  n, l
	if n
		ifdifi <l>,<pascal>
			add   sp, n
		endif
		if _$tmpf1 ge 16
			pop  _$tmpreg
		endif
	endif
endm


;-----------------------------------------------------------------
;   Is number ?
;-----------------------------------------------------------------
_$IsNumber macro n
	_$res = 0
	irpc $x,<n>
		ifdif <$x>,<'>
			if ('&$x&' GE '0') and ('&$x&' LE '9')
				_$res = 1
			endif
		endif
		exitm
	endm
endm

;-----------------------------------------------------------------
;   define function address
;-----------------------------------------------------------------
_defaddr  macro  sym, pn
	_$defaddr0  dd, sym, pn
endm

_defoff  macro  sym, pn
	_$defaddr0  dw, sym, pn
endm

_$defaddr0  macro  typ, sym, pn
	ifdef  _$n$&pn
		_$defaddr1  %_$n$&pn, <typ>, <sym>
	else
		sym  typ  pn
	endif
endm

_$defaddr1  macro  i, typ, sym
	ife _$ext$&i
		_$DeclExtrn  pn, %_$t&i
		_$ext$&i = 1
	endif
	sym  typ  _$lbl$&i
endm


;-----------------------------------------------------------------
;		Move Proc Offset
;-----------------------------------------------------------------
_movoff  macro  sym, pn
	ifdef  _$n$&pn
		_$movoff1  %_$n$&pn, <sym>
	else
		mov  sym, OFFSET pn
	endif
endm

_$movoff1  macro  i, sym
	ife _$ext$&i
		_$DeclExtrn  pn, %_$t&i
		_$ext$&i = 1
	endif
	mov  sym, _$lbl$&i
endm


;-----------------------------------------------------------------
;		Push / Pop Registers
;-----------------------------------------------------------------
_$PushReg  macro  r
	irp $x, <r>
	  ifidni <$x>,<f>
	  	pushf
	  else
		push  $x
	  endif
	endm
endm

_$PopReg  macro  l
	_$Reverse  <l>, _$lst							; reverse list
	_$$PopRegs  %_$lst
endm

_$$PopRegs  macro  r
	irp $x, <r>
	  ifidni <$x>,<f>
		popf
	  else
		pop   $x
	  endif
	endm
endm


;-----------------------------------------------------------------
;		List Handling
;-----------------------------------------------------------------
_$Reverse macro  l, r
	r equ <>
	irp $x,<l>
		_$Concat  r, $x, %&r
	endm
endm

_$Concat  macro  v, x, y
	ifb <x>
			v  equ  <y>
	else
		ifb <y>
			v  equ  <x>
		else
			v  equ  <x,y>
		endif
	endif
endm


;-----------------------------------------------------------------
;   		Text Equate
;-----------------------------------------------------------------
_$Equ  macro  m, v
		m  equ  <v>
endm


;-----------------------------------------------------------------
;		   Set Register Bit 
;-----------------------------------------------------------------
_$SetRegMask  macro  r
	_$msk = 0
	_$res = 0
	irp $x, <r>
		if _$msk and _$msk$&&$x
			_$res = 1
		endif
		_$msk = _$msk or _$msk$&&$x
	endm
endm

;-----------------------------------------------------------------
;		 Debug print
;-----------------------------------------------------------------
_$dprint macro fmt,?1,?2,?3,?4,?5
	_$dbgprn  macro $1,$2,$3,$4,$5
		%out  fmt
	endm
	_$dbgprn  <?1>,<?2>,<?3>,<?4>,<?5>
endm

IF1
_$dprint %_$MSG, %_$ProcVersion
ENDIF

.list

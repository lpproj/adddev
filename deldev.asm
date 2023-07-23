	page	,132
include	s&p.inc
include	adddel.inc

GotoErr	macro cond, errno
	_if <cond> then <<Err errno>>
	endm

FOR_OFF	equ	1
CR	equ	0dh
LF	equ	0ah
Vers	equ	'deldev.exe v',ADDDEL_VER,BETA,ADDDEL_EXTRA ADDDEL_DATE,CR,LF,0

code	segment
	assume	cs:code,ds:nothing,es:nothing,ss:Stack
	org	0h
ID	label	byte
	org	10h
IntVect	label	dword
	org	80h
cmdline	db	80 dup(?)
	org	100h
IDstrings db Vers
	org	110h
IntVectr	dd	192	dup(?) ; 割込ベクタ保存バッファ
ScurStdinPtr	dd	?	; 本 プログラム 実行前の STDIN	ポインタ
ScurClockPtr	dd	?	; 本 プログラム 実行前の 最終 LOADED デバドラ ポインタ
SLastDevPtr		dd	?	; 本 プログラム 実行前の 最終 LOADED デバドラ ポインタ
SNulDevPtr		dd	?	; 本 プログラム 実行前の NUL ポインタ

SaveFcbNo		dw	0	; 書き換えた内部FCBの数
SaveFcb			SaveFcbStruc MAXINTFCB dup	(<>)
DeInstF			db	MAXDEV dup(0)	;
DeInstFCount	db	MAXDEV dup(0)	;
DeInstIdx		dw	0

MsDosIntnl	dd	?	; MS-DOS内部変数保持ポインタ
ResidentSeg	dw	0
Errcode		dw	0
ExpectID	db	ADDDEV_ID,0
CommandBlk	CmdBlk	<,,14h>	; DeInstall リクエスト用

OpnMsg		db	Vers
	strcnst DetachMsg,<'Detach Device(s)',CR,LF>
	strcnst	errmsg0, <CR,LF>
	strcnst errmsg1, <07,"No device driver(s) loaded by ",ADDDEV_ID,CR,LF>
	strcnst errmsg2, <07,"Memory release fault",CR,LF>

ermsgptr label word
	irpc x, <0123456789abcdef>
	ifdef errmsg&x
		dw	errmsg&x + 2
	endif
	endm

_proc Ctrl_C_Exit
	_use <ds,ax,dx>
	movsg ds,cs
	mov	dx,offset C23
	mov	ax,2523h
	msdos
_endp
C23: iret

_proc StrOut <<msgp dword>> ; ASCIZ 文字を標準出力に表示
	_use <ds,es,ax,bx,cx,dx,di>
	lds	dx,msgp
	les di,msgp
	mov	cx,di
	not	cx
	mov	al,0
	repnz scasb
	_if <z>
		dec	di
		sub	di,dx
		mov	cx,di
		mov	bx,dx			; htutiyaさんのありがとう
		_loop				;
			mov	ah,02h		;
			mov	dl,[bx]		;
			inc	bx		;
			msdos			;　１文字出力する
		_endloop			;
	_endif
_endp

_proc Err <ErrorNo>	; エラー番号に対応するメッセージを表示する
	_use <ds,ax,si>
	mov	si, ErrorNo
	add	si, si
	mov	si, ermsgptr[si]
	StrOut <<cs, si>>
	mov	ax,4c01h
	msdos
_endp 0

_proc MemoryFree
	_use <es, ax>
	mov	ax,ResidentSeg
	mov	es,ax
	mov	byte ptr es:ID, 00h
	mov	ah,49h
	msdos
	GotoErr <c>,2
_endp

_proc ResidentSegSet
	_local <IDsize word>
	_use <ds,es,ax,bx,cx,si,di>
	movsg es,cs
	mov di,offset ExpectID
	mov	al,0
	mov cx,di
	not cx
	repnz scasb
;	GotoErr <<cx z>>,3
	sub	di,offset ExpectID
	dec	di
	mov IDsize,di
	;;;;;;

	mov	ah,52h
	msdos
	mov	word ptr MsDosIntnl, bx
	mov	word ptr MsDosIntnl+2, es
	mov	es,es:[bx-2]	; get 1st MCB segment

	_do
		mov	al,es:[0]
		_if <<al e 'Z'> or <al e 'M'>>
			_if <<word es:[1] ne 0>> 	; free ?
				push es
				mov	ax,es
				inc	ax
				mov	es,ax
				mov	di,offset ID
				mov	si,offset ExpectID
				mov 	cx,IDsize
				repz 	cmpsb
				mov	ax,es
				pop	es
				_if <z> then <<mov ResidentSeg,ax>>
			_endif
		_else
			_return
		_endif
		_exit <<byte es:[0] e 'Z'>>
		mov	bx,es
		mov	ax,es:[3]
		add	ax,bx
		inc	ax
		mov	es,ax
	_enddo
	GotoErr <<ResidentSeg e 0>>,1
_endp

_proc RestoreVector <<svtblp dword>>
	_use <ds,es,si,di,cx>
	xor di,di
	mov es,di
	lds	si,svtblp
	mov cx,400h / 2
	cld
	cli
	repz movsw
	mov	word ptr es:[23h*4],offset C23 	; ここはプロセス終了時に
	mov	word ptr es:[23h*4+2],cs	    ; 元に戻される
	sti
_endp

_proc RestoreEnv
	_use <ds,es,ax,bx,cx,dx,si,di>
	mov	es,ResidentSeg
	lds	bx,MsDosIntnl

	push es
	  les di,es:ScurStdinPtr
	  mov word ptr [bx].StdinPtr,di
	  mov word ptr [bx].StdinPtr+2,es
	pop	es

	push es
	  les di,es:ScurClockPtr
	  mov word ptr [bx].ClockPtr,di
	  mov word ptr [bx].ClockPtr+2,es
	pop	es

	mov	cx, es:SaveFcbNo

	_if <cxnz>
		_loop
			mov	si, cx
			dec	si

			mov	ax, si
			mov	bx, Size SaveFcbStruc
			mul	bl
			mov	si,ax

			mov	bx, es:SaveFcb[si].Ofs
			push es
			  les di, es:SaveFcb[si].OldPtr
			  mov [bx],di
			  mov [bx+2],es
			  mov [bx+4],di
			pop	es
		_endloop
	_endif
	mov	di, offset IntVect
	RestoreVector <<es, di>>
_endp

_proc DeInstall
	_local <<Dev_Sent dword>, <Dev_Ient dword>>
	_use <es,ds,ax,bx,cx,dx,si,di>

	lds	bx, MsDosIntnl
	lds	si, [bx+34]			; ds:si = 現在の NUL へのポインタ

	; 現在組み込まれているデバイスのリンクをたどって,
	; 組み込む前の最終デバイスに達するまで,DeInstall コマンドを
	; 発行する。

	_do
		mov	es, ResidentSeg
		dec	es:DeInstIdx
		mov	di, es:DeInstIdx
		mov cl, es:DeInstFCount[di]
		_while <<cl nz>>
			mov	ax, ds
			les	bx, es:SLastDevPtr

			mov	dx,es
			_exit <<dx e ax> and <bx e si>>	; 組み込む直前のデバイスドライバに
											; 達したら抜け出る
			mov es, ResidentSeg
			mov	al, es:DeInstF[di]
			shr	al, cl
			_if <c>
				mov	ax,[si].S_Entry
				mov	word ptr Dev_Sent, ax
				mov	word ptr Dev_Sent+2, ds
				mov	ax,[si].I_Entry
				mov	word ptr Dev_Ient, ax
				mov	word ptr Dev_Ient+2, ds

				movsg es, cs
				mov	CommandBlk.req_com, 14h
				mov	bx, offset CommandBlk
				call Dev_Sent
				call Dev_Ient
			_endif
			lds	si, [si]	; ds:si = その次のデバイスへのポインタ
			mov	es, ResidentSeg
			dec cl
		_enddo
		mov	es, ResidentSeg
	_until <<es:DeInstIdx e 0>>
_endp

_proc Relink
	_use <es,ds,si,di>
	mov	es,ResidentSeg
	lds	si,es:SNulDevPtr
	les	di,es:SLastDevPtr
	mov	[si],di
	mov	[si+2],es
_endp

_proc ClosingMsg
	_use <es,ax,dx,si,di>
	StrOut <<cs,DetachMsg>>
;	mov	es,ResidentSeg
;	mov	es,es:[2ch]
;	mov	di,0
;	_do
;		_exit <<word es:[di] e 0001h>>
;		inc di
;	_enddo
;	inc	di
;	inc	di
;	StrOut <<es, di>>

;	mov	ds,ResidentSeg
;	mov	si,80h
;	lodsb
;	mov	di,si
;	xor	ah,ah
;	add	si,ax
;	mov	byte ptr [si],0
;	StrOut <<ds, di>>
_endp

_proc Main
	mov	ax,cs
	mov	ds,ax

	mov	ax,offset IDstrings
	Ctrl_C_Exit
	StrOut <<cs, ax>>
	ResidentSegSet
	DeInstall
	Relink
	RestoreEnv
	MemoryFree
	ClosingMsg
if FOR_OFF
	mov	ax,0c00h		;KeyBuffer clear
	msdos				;VJEb をはずしたときゴミが出るので
endif
	mov	ax,4c00h
	msdos
_endp 0

Stack	segment	Stack
	dw	128 dup	(?)
Stack	ends

	end	_Main

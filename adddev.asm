	page	,132

include s&p.inc
include adddel.inc

msdos macro
	int	21h
	endm

Vers equ ADDDEV_ID,BETA,ADDDEL_DATE
code segment
	assume	cs:code,ds:nothing,es:nothing,ss:Stack
;  以下の変数は 本プログラムでロードしたデバイスドライバを解放するために
;  必要とする.
	org	0h
dv_infos label	byte
Myname	label	byte
	org	10h
IntVect	label	dword		; 割込ベクタ保存バッファ
	org	80h
CmdLine db	80 dup(?)
	org	100h
Mynamer	db Vers
		org	110h
IntVectr 	dd	192 dup(?) ; 割込ベクタ保存バッファ
ScurStdinPtr	dd	?	; 本 プログラム実行前のSTDIN ポインタ
ScurClockPtr	dd	?	; 本 プログラム実行前の最終 Loaded device pointer
SLastDevPtr	dd	?	; 本 プログラム実行前の最終 Loaded device pointer
SNulDevPtr	dd	?	; 本 プログラム実行前のNUL pointer

SaveFcbNo	dw	0	; 書き換えた内部 FCB の数
SaveFcb 	SaveFcbStruc MAXINTFCB dup	(<>)
DeInstF 	db	MAXDEV dup (0)	;
DeInstFCount	db	MAXDEV dup (0)	;
DeInstIdx	dw	0

dv_infoe	label	byte
dv_info_size dw (((offset dv_infoe - offset dv_infos + 1) shr 4) + 1) shl 4

WorkingBuffer label byte; (多目的バッファ先頭番地)
DevInfo 		db	256	dup(?) ; そのファイルの内容を読み込むバッファ
DevSpecAdr		dw	?	; そのバッファのdevice=後の文字のアドレス
FileNameLastAdr dw	?	; device=後のファイル名を区切る空白のアドレス
LastDevEndSeg	dw	?	; 最後のデバドラ常駐最後番地
MsDosIntnl		dd	?	; MS-DOS内部変数保持ポインタ
NoOfLoadDev		db	0	; 一つもロードしなかったら常駐しない
EnvStored		db	0	; 割込ベクトルを保存したら1
Silent			db	0	; メッセージ抑制フラグ
BreakStatus		db	?	; Break ON/OFF を保存
DOSflag			db	0	; 6 でDOS6, 5 でDOS5, 4 でDOS4, 3 でDOS3.X
SearchUMB		db	0	; command.comの後ろを探したかどうか
systemFCBsize		db	0	; DOS4+ 3bh : DOS3.X 35h
CommandBlk		CmdBlk	<,,0>	; 初期化コマンドパケット
				even
MaxIntnlFcb		dw	?
TopSeg			dw	?
McbSeg			dw	?
EnvSeg			dw	0
SpecPath		dw	2 dup (WorkingBuffer, ?)

	strcnst IDstrings, <Vers,0ah,0dh>
	strcnst Crlf,	 <0dh,0ah>
	strcnst LoadMsg, <'Loading... '>
	strcnst errmsg0, <>
	strcnst errmsg1, <07,"Spec file couldn't open">
	strcnst errmsg2, <07,"missing 'device='">
	strcnst errmsg3, <07,"require MS-DOS v3.xx or later">
	strcnst errmsg4, <07,"missing device driver file name">
	strcnst errmsg5, <07,"error while loading driver file(may be no file)">
	strcnst errmsg6, <07,"must link this program with /HIGH switch.">
	strcnst errmsg7, <07,"device driver initialize error">
	strcnst errmsg8, <07,"cannot install Block device">
	strcnst	errmsg9, <07,"too many replaced internal dos devices">
	strcnst errmsga, <07,"too many drivers in one file">
	strcnst errmsgb, <07,"too many drivers at a time">
	strcnst errmsgc, <07,"cannot find environment block">
;;	strcnst warnmsg, <07,"installation was rejected",0dh,0ah>

ermsgptr label word
	irpc x, <0123456789abcdef>
	ifdef errmsg&x
		dw	errmsg&x + 2
	endif
	endm

AssumePathStr	db	'SYS=',0
DEVICEstring	db	'DEVICE'
SilentStr	db	'SILENT'

UsgMsg dw UsgMsg+2
 db	0dh,0ah
if ENGLISH
 db "Usage of ",ADDDEV_ID,BETA,"(rusu, TELEPUTE -off- and S_HAY)",0dh,0ah
 db "  1.  Make a text file like config.sys which describes device drivers to be",0dh,0ah
 db "      configured.",0dh,0ah
 db "      The SPACEs and TABs at the head of any line are ignored.",0dh,0ah
 db "      If the first character (other than SPACEs or TABs) of a line is a '#',",0dh,0ah
 db "      the line is regarded as a comment.",0dh,0ah
 db "      Lines only with SPACEs and TABs, and null lines are ignored.",0dh,0ah
 db "        [ex.] sample.dev",0dh,0ah
 db "          # This is sample",0dh,0ah
 db "          device=a:\sys\mouse.sys",0dh,0ah
 db "          device=c:\mttk.drv /N /C74",0dh,0ah
 db "          # end of sample",0dh,0ah
 db "  2.  After the file has been made, execute adddev like this.",0dh,0ah
 db "        [ex.] adddev c:sample.dev",0dh,0ah
 db "  3.  If the  designated  file  (sample.dev in the example)  doesn't  exist",0dh,0ah
 db "      in the current directory, the adddev will  search  the  file  in  the",0dh,0ah
 db "      directory assigned to the environment  variable  'SYS'  (if assigned).",0dh,0ah
 db "  4.  When the file hasn't be found, if the filename doesn't have extension,",0dh,0ah
 db "      the adddev will search the file 'filename.dev' as same process  as  3.",0dh,0ah
 db "  5.  You can also register a device driver directly from the command  line,",0dh,0ah
 db "      by putting '",CMD_CHR,"' or '",CMD_CHS,"' before filename like followings.",0dh,0ah
 db "        [ex.] adddev "
 db CMD_CHR
 db "a:\mouse.sys -B",0dh,0ah
else
 db	'使用法: ',ADDDEV_ID,BETA,'(rusu, TELEPUTE -off- and S_HAY)',0dh,0ah
 db	' 1.config.sys に記述するのと同様に組み込みたいデバイスドライバ',0dh,0ah
 db	'   ファイルを任意の名前のファイルに記述する。 記述に関して行頭',0dh,0ah
 db	'   の空白、タブコードは無視する。それ以外の文字で最初に # が有',0dh,0ah
 db	'   るときはコメント行とみなす。 空白またはタブのみの行、または',0dh,0ah
 db	'   null 行は無視をする。',0dh,0ah
 db	'   [例] sample.dev というファイルを',0dh,0ah
 db	'     # コメントも書けます.',0dh,0ah
 db	'     device=a:\sys\mouse.sys',0dh,0ah
 db	'     device=c:\mttk.drv /N /C74',0dh,0ah
 db	'   などの内容にする。',0dh,0ah
 db	' 2.その後,このプログラムをそのファイル名を指定して実行する。',0dh,0ah
 db	'   [例] A>adddev b:sample.dev',0dh,0ah
 db	' 3.指定されたファイル名がカレントディレクトリに存在しない時は、',0dh,0ah
 db	'   環境変数 SYS が設定されていれば、SYS の内容のディレクトリを',0dh,0ah
 db	'   探す。',0dh,0ah
 db	' 4.それでも見つからないときは、 もし指定されたファイル名に拡張',0dh,0ah
 db	'   子がなかったら .dev をファイル名に追加して、3. を繰り返す。',0dh,0ah
 db	' 5.または、コマンドラインで ['
 db	CMD_CHR,'|',CMD_CHS
 db	'] ではじまるファイル名をデバイ',0dh,0ah
 db	'   スドライバそのものとして登録する。',0dh,0ah
 db	'   [例] A>adddev '
 db	CMD_CHR
 db	'a:\mouse.sys -B',0dh,0ah
endif
 db	0
_proc Ctrl_C_Exit
	_use <ds,ax,dx>
	movsg ds,cs
	mov	dx,offset C23
	mov	ax,2523h
	msdos
	mov	ax,3300h
	msdos
	mov	BreakStatus,dl
	mov	ax,3301h
	mov	dl,0
	msdos
_endp
C23: iret

_proc StrOut <<msgp dword>>			; ASCIZ 文字を標準出力に表示
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

_proc InitMem
	push	ds						; S_HAY
	mov	ah, 62h						; S_HAY
	msdos							; S_HAY
	mov	ds, bx				;Current PSP Seg; S_HAY
	push	ds				;Current PSP Seg; S_HAY
	mov	es, word ptr ds:[2ch]		;Env       Seg	; S_HAY
	mov	ah, 49h						; S_HAY
	msdos							; S_HAY
	push	ax				;Env  MCB  Seg	; S_HAY
	mov	es, ax				;Env  MCB  Seg	; S_HAY
	add	ax, word ptr es:[0003h]				; S_HAY
	inc	ax				;Code MCB  Seg2	; S_HAY
	inc	ax				;Code      Seg2	; S_HAY
	pop	es				;Env  MCB  Seg	; S_HAY
	pop	bx				;Code      Seg1	; S_HAY
	sub	ax, bx						; S_HAY
	mov	ax, es				;Env  MCB  Seg	; S_HAY
	mov	es, bx				;Code      Seg1	; S_HAY
	jnz	e_not_cont					; S_HAY
	dec	bx				;Code MCB  Seg	; S_HAY
	mov	ds, bx				;Code MCB  Seg	; S_HAY
	push	ds						; S_HAY
	mov	bx, word ptr ds:[0003h]				; S_HAY
	inc	bx				;Code Size Para+1;S_HAY
	mov	es, ax				;Env  MCB  Seg	; S_HAY
	inc	ax				;Env       Seg	; S_HAY
	push	ax				;Env       Seg	; S_HAY
	add	word ptr es:[0003h], bx				; S_HAY
	mov	word ptr es:[0001h], ax				; S_HAY
	mov	bl, byte ptr ds:[0000h]				; S_HAY
	mov	byte ptr es:[0000h], bl				; S_HAY
	pop	bx						; S_HAY
	push	bx						; S_HAY
	mov	ah, 50h						; S_HAY
	msdos							; S_HAY
	pop	es				;Env       Seg	; S_HAY
	pop	ax				;Code MCB  Seg	; S_HAY
	inc	ax				;Code      Seg	; S_HAY
	mov	ds, ax				;Code      Seg	; S_HAY
	push	si						; S_HAY
	push	di						; S_HAY
	push	cx						; S_HAY
	sub	si, si						; S_HAY
	sub	di, di						; S_HAY
	mov	cx, 100h					; S_HAY
	cld							; S_HAY
	rep	movsb						; S_HAY
	mov	word ptr es:[0030h], es				; S_HAY
	mov	word ptr es:[0036h], es				; S_HAY
	pop	cx						; S_HAY
	pop	di						; S_HAY
	pop	si						; S_HAY
e_not_cont:							; S_HAY
	pop	ds						; S_HAY

	mov	dx,cs
	mov	ah,55h	; psp copy (dx:segment)
	msdos

	mov	ax,es:[16h]
	xchg ax,cs:[16h]
	mov	McbSeg,ax

	mov	bx, es
	mov	ah,50h	; set current psp seg (bx)
	msdos

	push	cx					;
	mov	cx,5					; ファイルハンドルの
fhclose:						; クローズ
	mov	bx,cx					;
	dec	bx					;
	mov	ah,3eh					;
	msdos						;
	loop	fhclose					;
	pop	cx					;

	mov	bx,cs	; set psp seg
	mov	ah,50h
	msdos

	movsg	ds, cs
	push	si
	push	di
	push	cx
	mov	si, offset Mynamer
	mov	di, offset Myname
	mov	cx, 10h
	rep	movsb
	pop	cx
	pop	di
	pop	si

	mov	ax,McbSeg
	push es
	mov	bx,cs
	mov	es,ax
	sub	bx,ax
	dec	bx
	mov	ah,4ah
	msdos
	mov	ah,4ah
	msdos
	pop	es

	mov	TopSeg,es
	mov	ax,dv_info_size
	add	ax,offset dv_infos
	mov	cl,4
	shr	ax,cl
	add	ax,TopSeg
	mov	LastDevEndSeg,ax
	movsg ds,cs
	movsg es,cs
_endp

_proc StoreVector
	_use <ds,es,cx,si,di>
	mov	es, TopSeg
	xor	si, si
	mov	ds, si
	mov	di, offset IntVect
	mov	cx, 200h
	cli
	repz movsw
	sti
_endp

_proc RestoreVector
	_use <ds,es,si,di,cx>
	xor di,di
	mov es,di
	mov ds,TopSeg
	mov si,offset IntVect
	mov cx,200h
	cli
	repz movsw
	mov	word ptr es:[23h*4],offset C23	; ここはプロセス終了時に
	mov	word ptr es:[23h*4+2],cs	    ; 元に戻される
	sti
_endp

_proc DeInstall
	_local <<Dev_Sent dword>, <Dev_Ient dword>>
	_use <es,ds,ax,bx,cx,dx,si,di>

	lds	bx, MsDosIntnl
	lds	si, [bx+34]			; ds:si = 現在の NUL へのポインタ

	; 現在組み込まれているデバイスのリンクをたどって,
	; 組み込む前の最終デバイスに達するまで,DeInstall コマンドを
	; 発行する。

	_while <<DeInstIdx ne 0>>
		dec	DeInstIdx
		mov	di, DeInstIdx
		mov	cl, DeInstFCount[di]
		_while <<cl nz>>
			mov	ax, ds
			les	bx, SLastDevPtr

			mov	dx,es
			_return <<dx e ax> and <bx e si>>; 組み込む直前のデバイスドライバに
											; 達したら抜け出る
			mov	al, DeInstF[di]
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
			dec cl
		_enddo
	_enddo
_endp

_proc RestoreEnv
	_return <<EnvStored e 0>>
	DeInstall
	lds	bx, MsDosIntnl
	les	di, ScurStdinPtr
	mov	word ptr [bx].StdinPtr,di
	mov	word ptr [bx].StdinPtr+2,es
	les	di, ScurClockPtr
	mov	word ptr [bx].ClockPtr,di
	mov	word ptr [bx].ClockPtr+2,es

	lds	bx, SNulDevPtr
	les	di, SLastDevPtr
	mov	[bx], di
	mov	[bx+2], es

	mov	cx, SaveFcbNo

	_if <cxnz>
		_loop
			mov	si, cx
			dec	si

			mov	ax, si
			mov	bx, Size SaveFcbStruc
			mul	bl
			mov	si,ax

			mov	bx, SaveFcb[si].Ofs
			les	di, SaveFcb[si].OldPtr
			mov	[bx],di
			mov	[bx+2],es
			mov	[bx+4],di
		_endloop
	_endif
	RestoreVector
_endp

_proc NoResident
	mov	es,TopSeg	; memory の解放
	mov	ah,49h
	msdos
	movsg es,cs
	mov	ah,49h
	msdos
	mov	dl,BreakStatus
	mov	ax,3301h
	msdos
	mov	ax,4cffh	; 異常終了ということで 0ffh	を返す
	msdos
_endp 0

GotoErr macro cond, errno
	_if <cond>
		Err errno
	_endif
	endm

_proc Err <ErrorNo>	; エラー番号に対応するメッセージを表示する
	_use <ds,si>
	RestoreEnv		; 割込ベクタを元に戻す
	mov	si, ErrorNo
	add	si, si
	mov	si, ermsgptr[si]
	StrOut <<cs, si>>

	_if <<byte ErrorNo be 5>> then <<StrOut <<cs, UsgMsg>>>>
	NoResident
_endp 0

_proc Diag			; /HIGH SWITCH つきで LINK したか調べる
	mov	ax,cs
	mov	bx,ds
	add	bx,10h
	GotoErr <<ax e bx>>, 6
_endp

_proc CloseSpecFile <Handler>
	_use <ax,bx>
	mov	bx,Handler
	mov	ah,3eh		; close file
	msdos
_endp

_proc DeInstDevMark
	_use <ax, bx, cx>
	mov	bx, DeInstIdx
	GotoErr <<bx ae MAXDEV>>,11
	GotoErr <<DeInstFCount[bx] ae MAXDEV>>,10
	inc	DeInstFCount[bx]
	mov	ax,DevSpecAdr
	mov	cx,cs
	not	ax
	not	cx
	_if <<word CommandBlk.BpbPtr e ax>>, and
	_c  <<word CommandBlk.BpbPtr+2 e cx>>
		mov	cl,DeInstFCount[bx]
		xor	al,al
		stc
		rcl	al,cl
		or	DeInstF[bx],al
	_endif
_endp

_proc ResidentProc
	mov	bx,LastDevEndSeg		; 常駐するメモリを確保
	sub	bx,TopSeg			;
	mov	es,TopSeg			;
	mov	ah,4ah				;
	msdos

	mov	ax, TopSeg			; 常駐部のPID変更
	dec	ax				;
	mov	es, ax				;
	inc	ax				;
	mov	word ptr es:[0001h],ax		;

	movsg	ds, cs
	mov	es,TopSeg
	mov	si,offset dv_infos
	add	si,410h
	mov	di,si
	mov	cx,dv_info_size
	sub	cx,410h
	repz movsb

	mov	dl,BreakStatus
	mov	ax,3301h
	msdos

	mov	ax,4c00h
	msdos
_endp 0

_proc SaveEnv
	_use <es, ds, ax, bx>
	mov	ax,3000h	; version check
	msdos
	GotoErr <<al l 3> or <al g 6>>, 3			; DOS4+
	mov	byte ptr DOSflag, al				; DOS4+
	_if <<al e 3>>						; DOS4+
		mov	byte ptr systemFCBsize, SYSFCBSIZE3	; DOS4+
	_else							; DOS4+
		mov	byte ptr systemFCBsize, SYSFCBSIZE4	; DOS4+
	_endif							; DOS4+

	mov	ah,52h
	msdos				; return es:bx
	mov	word ptr MsDosIntnl,	 bx
	mov	word ptr MsDosIntnl+2,	 es

	lds	si,	es:[bx].StdinPtr
	mov	word ptr ScurStdinPtr,	 si
	mov	word ptr ScurStdinPtr+2, ds

	lds	si,	es:[bx].ClockPtr
	mov	word ptr ScurClockPtr,	 si
	mov	word ptr ScurClockPtr+2, ds

	add	bx,34
	mov	word ptr SNulDevPtr,   bx
	mov	word ptr SNulDevPtr+2, es

	lds	si,SNulDevPtr	; ロード前の最終デバドラ開始番地を es:di へ
	les	di,[si]
	mov	word ptr SLastDevPtr,	di
	mov	word ptr SLastDevPtr+2, es

	StoreVector
	mov	EnvStored,1
_endp

_func LoadDevDr <> <ax>
	_local <<OvrLoadBlk dword>>		; AX=4B03H のシステムコール用
	_use <ds,es,bx,dx,si>
	_if <<Silent e 0>>
		StrOut <<cs, IDstrings>>
		StrOut <<cs, LoadMsg>>		; Loading ...  メセージ表示
		StrOut <<cs, DevSpecAdr>>
		StrOut <<cs, Crlf>>
	_endif
	mov	ax, LastDevEndSeg
	mov	word ptr OvrLoadBlk,   ax
	mov	word ptr OvrLoadBlk+2, ax

	movsg ds,cs

	mov	si,FileNameLastAdr
	mov	byte ptr [si],0

	mov	dx,80h
	mov	ah,1ah			; dta set 久保さんありがとう
	msdos

	movsg es,ss
	lea	bx,OvrLoadBlk	; es:bx
	mov	dx,DevSpecAdr	; ds:dx
	mov	ax,4b03h		; Load Program
	msdos
	GotoErr <c>, 5
	mov	ax,word ptr OvrLoadBlk
_endp

_func Readln <Handler, <bufp dword>, cx> <cx>
; INPUT
;  Handler ファイルハンドラ
;  bufp    読み込みバッファポインタ
;  cx	   読み込みバッファサイズ
; OUTPUT
;  cx	   EOFなら 1
	_local	<<Bufful word>, <ReadCh byte>, <k1st word>, <Eof word>>
	_use <ds, ax,bx,dx,si>
	_if <cxnz>
		xor	ax,ax
		mov Bufful,ax
		mov k1st,ax
		mov	Eof,ax
		lds	si, bufp
		_do
			push ds
			push cx
				movsg ds,ss
				lea dx,ReadCh
				mov cx,1
				mov	bx,Handler
				mov ah,3fh
				msdos
			pop	cx
			pop ds

			_if <<c> or <ax z>>
				mov	al,1ah
			_else
				mov al,ReadCh
			_endif

			_if <<k1st e 0>>
				_if <<al ae  81h> and <al be 0a0h>>, or
				_c  <<al ae 0e0h> and <al be 0fch>>
					mov k1st,1
				_else
					_if <<al ae 'a'> and <al be 'z'>> then <<sub al,'a'-'A'>>
				_endif
			_else
				mov k1st,0
			_endif

			_if <<Bufful e 0>>
				dec	cx
				_if <z or <al e 1ah>>
					mov Bufful,1
					mov	byte ptr [si],0ah
					_if <<al e 1ah>> then <<mov Eof,1>>
				_else
					mov [si],al
					inc	si
				_endif
			_endif
		_until <<al e 1ah> or <al e 0ah>>
	_else
		mov Eof,1
	_endif
	mov	cx,Eof
_endp


_func SetDevInfo <Handler> <cx>
	_use <ds,es,ax,bx,si,di>
	movsg ds,cs
	movsg es,cs
	mov	si,offset DevInfo	; 仕様ファイル xxxx.dev の内容を
	mov	di,si				; 読み込むバッファをクリアして
	xor	al,al
	mov	cx,size DevInfo
	push cx
	repz stosb
		pop	cx
	Readln <Handler, <cs,si>, cx> <cx>	; 1行読む (DS:SI, BX, CX)
_endp


_func StripBlank <si> <si,ax>
	_do
		lodsb
	_until <<al ne TAB> and <al ne ' '>>
_endp

_func DevInfoScan <> <ax>
	_use <ds, es, cx, si, di>
	movsg ds,cs
	mov	si,offset DevInfo
	StripBlank <si> <si,ax>
	_if <<al a ' '> and <al ne '#'>>
		movsg es, cs
		dec	si
		mov	di,offset DEVICEstring
		mov	cx,size DEVICEstring
		repz cmpsb
		GotoErr <nz>, 2
		StripBlank <si> <si,ax>
;		GotoErr <<al ne '='>>, 2
;		StripBlank <si> <si,ax>
		_if <<al e '='>> then <<StripBlank <si> <si,ax>>>	;

		GotoErr <<al b ' '>>, 2
		dec	si
		mov	DevSpecAdr,si
		_do
			lodsb
			GotoErr <<si ae <offset DevInfo + Size DevInfo>>>, 4
		_until <<al e ' '> or <al e 0dh> or <al e 0ah>>
		dec	si
		mov	FileNameLastAdr,si
		mov	ax,0
	_elseif <<al e '#'>>
		StripBlank <si> <si,ax>
		movsg es, cs
		dec	si
		_do
			_do
				lodsb
			_until <<al e 'S'> or <al e 0dh> or <al e 0ah>>
			dec	si
			mov	di,offset SilentStr
			mov	cx,size SilentStr
			repz cmpsb
			_if <z> then <<mov Silent,1>>
		_until <<al e 0dh> or <al e 0ah>>
		mov ax,1
	_else
		mov	ax,1
	_endif
_endp

_func FindEnvStr <ds, si> <es, di>
;	in	: ds:si search string
;	out	: es:di Environment	string
	_use <bx>

	mov	ax,352eh
	msdos
	mov	di,es

	mov	ah,52h
	msdos
	mov	es,es:[bx-2]	; get 1st MCB segment
searchEnvBlock:
	_do
		mov	al,es:[0]
		GotoErr <<al ne 'Z'> and <al ne 'M'>>,12
		_if <<word es:[1] e di>>
			mov	ax,es
			inc	ax
			_if <<ax ne di> and <word es:[3] ge 000ah>>
				mov	EnvSeg,ax
				_break
			_endif
		_endif
		_exit <<al e 'Z'>>
		mov	bx,es
		mov	ax,es:[3]
		add	ax,bx
		inc	ax
		mov	es,ax
	_enddo

	_if <<EnvSeg e 0> and <SearchUMB e 0>>
		mov	SearchUMB,1
		mov	ax, 0c000h
	    searchUMBloop:
		mov	es, ax
		mov	al,byte ptr es:[0]
		_if <<al e 'M'>>
			push	es
			mov	bx,es
			mov	ax,es:[3]
			add	ax,bx
			jo	NoMCB
			inc	ax
			jo	NoMCB
			mov	es,ax
			mov	al,byte ptr es:[0]
			pop	es
			_if <<al e 'M'> or <al e 'Z'>>
				jmp	searchEnvBlock
			_endif
		    NoMCB:
		_elseif <<al e 'Z'>>
			mov	bx,es
			mov	ax,es:[3]
			add	ax,bx
			jno	searchEnvBlock
		_endif
			mov	ax,es
		_if <<ax ne 0ff00h>>
			add	ax,100h
			jmp	searchUMBloop
		_endif
	_endif

	_if <<EnvSeg e 0>>
		_if <<DOSflag ge 4>>
			mov	es,di
			mov	es,es:[2ch]
			mov	EnvSeg,es
		_endif
	_endif

	GotoErr <<EnvSeg e 0>>,12

	mov	es,EnvSeg

	xor	di,di
	mov	bx,si
	_do
		mov	si,bx
		_do
			lodsb
			_return <<al z>>
			_exit <<al ne es:[di]>>
			inc	di
		_enddo
		_do
			inc	di
		_until <<byte es:[di] e 0>>
		inc	di
	_until <<byte es:[di] e 0>>
_endp

_func OpenSpecFile <> <bx>
	_local <<k1st word>>
	_use <ax, cx, dx, si, di>
	mov	ax,4400h
	mov	bx,STDIN
	msdos
	_if <nc>
		_if <<dx z 80h>>,test
			mov	bx,STDIN
			_return
		_endif
	_endif

	mov	si, offset AssumePathStr ; DS:SI
	FindEnvStr <ds,si>			 ; RETURN ES:DI
	xchgs es,ds
	mov	si,SpecPath[0]
	xchg si,di
	_do
		StripBlank <si>
		stosb
	_until <<al z>>
	dec	di
	mov	al,es:[di-1]

	_if <<al ne '\'> and <al ne '/'> and <al ne ':'>>
		mov	al,'\'
		stosb
	_endif

	mov	SpecPath+2,di		; last of environment string

	movsg ds, cs
	mov	si,offset CmdLine
	lodsb
	mov	bl,al
	mov	bh,0
	mov	CmdLine[bx]+2,bh	; use bh as 0

	StripBlank <si>

	_if <<al e CMD_CHR> or <al e CMD_CHS>>		; -off- さんのアイデア
		_if <<al e CMD_CHS>> then <<mov Silent,1>>
		mov	k1st,0
		mov	di,offset DevInfo
		mov	DevSpecAdr,di
		push di
		_do
			lodsb
			_if <<k1st e 0>>
				_if <<al ae  81h> and <al be 0a0h>>, or
				_c  <<al ae 0e0h> and <al be 0fch>>
					mov k1st,1
				_else
					_if <<al ae 'a'> and <al be 'z'>> then <<sub al,'a'-'A'>>
				_endif
			_else
				mov k1st,0
			_endif
			stosb
		_until <<al e 0dh>>
		mov	byte ptr [di],0ah
		mov	byte ptr [di+1],0
		pop	si
		_do
			lodsb
			GotoErr <<si ae <offset DevInfo + Size DevInfo>>>, 4
		_until <<al e ' '> or <al e TAB> or <al e 0dh>>
		dec	si
		mov	FileNameLastAdr,si
		mov	bx,-1
		_return
	_endif

	_do
		stosb
		lodsb
	_until <<al z> or <al e 0dh>>

	mov	word ptr [di  ], 0 +'D' shl	8
	mov	word ptr [di+2],'E'+'V' shl	8
	mov	byte ptr [di+4],'$'

	; 1st : try	as is command line string
	; 2nd : environment	string + command line string
	; 3rd : command line string	+ '.dev'
	; 4th : environment	string + command line + '.dev'

	mov	dx,80h
	mov	ah,1ah		; dta set  久保さんありがとう
	msdos

	_loop 4
		mov	si,cx
		dec	si
		_if <<si e 1>> then <<mov byte ptr [di],'.'>>
		add	si,si
		and	si,2
		mov	dx,SpecPath[si]
		mov	ax,3d00h
		msdos
		_exit <nc>
	_endloop
	GotoErr <c>, 1
	mov	bx,ax
_endp

_proc CheckStdDev <DevHeadPtr dword>
	_use <ax, bx, cx, dx, si, di>
	les	bx,	MsDosIntnl
	les	bx,	es:[bx+4]
	mov	ax,	es:[bx+4]
	mov	MaxIntnlFcb, ax
	mov	ah,	systemFCBsize
	dec	al
	mul	ah
	add	bx,	ax
	add	bx,	6

	_do
		mov	di, bx
		_if <<word es:[bx] ne 0>>
			add	di,	DevName
			lds	si,	DevHeadPtr
			add	si,	DeviceName
			mov	cx,	Size DevName
			cld
			repz cmpsb
			_if <z>
				mov	ax, SaveFcbNo
				GotoErr <<ax ae MAXINTFCB>>, 9
				inc	SaveFcbNo

				mov	cx,	Size SaveFcbStruc
				mul	cl
				mov	si,	ax

				lea	ax,	[bx].DevPtr
				mov	SaveFcb[si].Ofs, ax
				lds	ax,	es:[bx].DevPtr
				mov	word ptr SaveFcb[si].OldPtr, ax
				mov	word ptr SaveFcb[si].OldPtr+2, ds

				lds	ax,	DevHeadPtr
				mov	word ptr es:[bx].DevPtr, ax
				mov	word ptr es:[bx].DevPtr+2, ds
				mov	es:[bx].DevOfs, ax
				_break	; exit _do .. _enddo block
			_endif
		_endif

		dec	MaxIntnlFcb
		_exit <z>			; compare complete
		sub	bl,	systemFCBsize
		sbb	bh,	0
	_enddo
_endp

_proc InitDevDr <LoadSeg>
	_local <<Strategy dword>, <Interrupt dword>>, c
	_local <<CharDev word>,<next dword>>
	_use <ds,ax,bx,dx,si>

	mov	ds,LoadSeg	; ロード直後
	mov	si,0

	mov CharDev,0
	_do
		test [si].Attrib,8000h	; キャラクタデバイスか
		GotoErr <z>, 8

		mov	ax, [si].S_Entry
		mov	word ptr Strategy, ax
		mov	word ptr Strategy+2, ds

		mov	ax, [si].I_Entry
		mov	word ptr Interrupt,	ax
		mov	word ptr Interrupt+2, ds

		mov	ax, DevSpecAdr
		mov	word ptr CommandBlk.BpbPtr,ax
		mov	word ptr CommandBlk.BpbPtr+2,cs

		movsg es,cs
		mov	bx,offset CommandBlk
		call Strategy	; far call (es:bx)
		call Interrupt	; far call

		test CommandBlk.req_sta,8000h	; check error status
		GotoErr <nz>, 7

		mov ax, word ptr [si].NextLink
		mov	word ptr next, ax
		mov	ax, word ptr [si].NextLink+2
		mov	word ptr next+2, ax

		_if <<word [si].Attrib nz 8000h>>,test
						; init で BLOCK device に変身したかどうかを調べる
			inc	NoOfLoadDev
			mov	CharDev,1

			mov	ax, ds			; 再リンクを行なう
			mov bx, si

			lds	si,SNulDevPtr
			les di,[si]
			mov [si],bx
			mov [si+2],ax

			mov ds, ax
			mov si, bx

			mov [si],di
			mov [si+2],es

			CheckStdDev <<ds,si>>
			_if <<[si].Attrib nz 0001h>>, test 	; stdin ?
				les	bx,	MsDosIntnl
				mov	word ptr es:[bx].StdinPtr, si
				mov	word ptr es:[bx].StdinPtr+2, ds
			_else
				_if <<[si].Attrib nz 0008h>>, test	; clock ?
					les	bx,	MsDosIntnl
					mov	word ptr es:[bx].ClockPtr, si
					mov	word ptr es:[bx].ClockPtr+2, ds
				_endif
			_endif

			DeInstDevMark
;;		_else
;;			StrOut <<cs, warnmsg>>
		_endif

		_exit <<word next e -1>>
		_if <<word next+2 e -1>> then <<mov word ptr next+2,0>>
								; assume segment's offset 0 if -1
		mov	ax,ds
		add	word ptr next+2,ax	; Next include driver's begin segment set
		lds	si, next
	_enddo

	inc	DeInstIdx

	_if <<CharDev e 1>>
		movsg ds,cs
		les	ax,CommandBlk.EndPtr
		mov	dx,es	; segment
		add	ax,15	; offset + 0fh
		_if <c> then <<add dx,1000h>> ; segment wrap?  1000h=10000h shr 4
		mov	cl,4
		shr	ax,cl
		add	ax,dx
		mov	LastDevEndSeg,ax
	_endif
_endp

_proc LoadDriverMain
	_use <ax>
	LoadDevDr	<> <ax> 		; デバドラファイルを読み込む ax:ロードセグメント
	InitDevDr	<ax> 			; デバドラに初期化コマンドを送る
	mov	ax,	0c00h
	msdos
_endp

_proc LoadDr
	_local <Handler, <SpecEof word>>
	_use <ds,si,ax,bx>
	OpenSpecFile <> <bx>	; xxxxxx.dev ファイルを開く
	mov	Handler, bx
	_if <<bx e -1>>
		LoadDriverMain
	_else
		mov	SpecEof,0
		_while <<SpecEof ne 1>>
			_do
				SetDevInfo <Handler> <cx> ; その内容をバッファに読み込む
				mov	SpecEof, cx
				DevInfoScan <> <ax>			; ファイル名,オプションを設定
			_until <<ax z> or <SpecEof e 1>>
			_if <<ax z>> then <<LoadDriverMain>>
		_enddo
		CloseSpecFile <Handler>
	_endif
_endp

_proc Main
	cld
	Ctrl_C_Exit			; Ctrl-C EXIT ADDRESS を変更し,Break OFF にする
	Diag					; /high switch でリンクしているか調べる
	InitMem 				; メモリ獲得など
	SaveEnv 				; 本 プログラム 実行前の DOS 環境をメモリに保存

; 以下2行は -off- さんの助言により追加したものです。
	mov	ax,	0c00h	; Kbd フラッシュ
	msdos
; ちなみに、NEC純正 adddrv.exe では,実行中はキーボード割り込みを
; マスクしてキー入力を阻止しております。

	LoadDr					; xxxxxx.dev で指定された デバドラ を読み込む
	_if <<NoOfLoadDev ne 0>>; ロードされた デバドラ がなければそのまま
		ResidentProc		; このプログラムとデバドラを常駐させる
	_else
		NoResident
	_endif
_endp 0

code ends

Stack segment Stack
	dw 256 dup (?)
Stack ends

	end	_Main

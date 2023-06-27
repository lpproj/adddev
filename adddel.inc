
STDIN	equ	0
STDOUT	equ	1

MAXINTFCB equ 	10
MAXDEV	equ	10

FR_ENV	equ	1
CMD_CHR	equ	'+'
CMD_CHS	equ	'*'
BETA	equ	' '	;; β版なら 'b' 正式版は ' '
ADDDEL_VER	equ	'2.55'
ADDDEL_DATE	equ	'(95-02-08)'
ADDDEV_ID	equ	'adddev.exe v',ADDDEL_VER
SYSFCBSIZE3	equ	35h
SYSFCBSIZE4	equ	3bh

ENGLISH		equ	0	; メッセージを英語にするなら 1

msdos macro
	int	21h
	endm

movsg macro	seg1,seg2
	push seg2
	pop	seg1
	endm

xchgs macro seg1, seg2
	push seg1
	push seg2
	pop	seg1
	pop	seg2
	endm

strcnst	macro var, str
	ifnb <var>
	  var dw var+2
	endif
	irp s, <str>
      db &s
	endm
	db	0
	endm

TAB	equ	9

DosStruc	struc	; Dos Internal variable	structure
DpbPtr		dd	?	; 0	1 2	3
IntFcbPtr	dd	?	; 4	5 6	7
ClockPtr	dd	?	; 8	9 a	b
StdinPtr	dd	?	; c	d e	f
DosStruc	ends

IntFcbStruc	struc
??1		dw	?
??2		db	5 dup (?)
DevPtr	dd	?
DevOfs	dw	?
??3		db	19 dup (?)
DevName	db	8 dup (?)
DevExt	db	3 dup (?)
??4		db 	10 dup (?)		;; DOS5	db	16 dup (?)
IntFcbStruc	ends

SaveFcbStruc struc
Ofs			dw	?
OldPtr		dd	?
SaveFcbStruc ends

DevHdr		struc
NextLink	dd	?
Attrib		dw	?
S_Entry		dw	?
I_Entry		dw	?
DeviceName	db	8 dup (?)
DevHdr		ends

CmdBlk	struc
req_len	db	size CmdBlk	; デバイスドライバへのリクエストブロック長
req_uni	db	?
req_com	db	?	; デバイスドライバへの命令
req_sta	dw	?
req_rev	db	8 dup (?)
units	db	?
EndPtr	dd	?	; 初期化命令実行後にデバドラの終了番地が入る
BpbPtr	dd	?	; 初期化命令の時に起動された要因の文字列が入る
blkdevn	db	?
CmdBlk	ends

#
# MAKEFILE for Ndmake
#
.SUFFIXES: .ish .lzh .exe .obj .asm

SWCHAR	= -
VER	= 253
ADDOLD	= 252
DELOLD	= 252
PROG	= adddev.exe deldev.exe
SRC	= adddev.asm deldev.asm
INCLUDE	= adddel.inc s&p.inc
MAN		= adddev.man  readme.doc
DOC		= history.doc readme.doc
ADDEXLZH	= add$(VER)ex.lzh
ADDSRLZH	= add$(VER)sr.lzh
ADDEXISH	= add$(VER)ex.ish
ADDSRISH	= add$(VER)sr.ish

all : $(PROG)

upload : $(ADDEXISH) $(ADDSRISH)

reform : $(SRC) $(INCLUDE)
	for %f in ($(SRC) $(INCLUDE)) \
		do overwrit %f sed -f regular

$(ADDEXISH) : $(ADDEXLZH)
	-rm $@
	ish $(ADDEXLZH) $(SWCHAR)s

$(ADDSRISH) : $(ADDSRLZH)
	-rm $@
	ish $(ADDSRLZH) $(SWCHAR)s

$(ADDEXLZH) : $(PROG) $(MAN)
	lharc u $* $(PROG) $(MAN)

$(ADDSRLZH) : $(SRC) $(INCLUDE) $(DOC)
	lharc u $* $(SRC) $(INCLUDE) $(DOC)

adddev.exe : adddev.obj
	olink /high $*;
#	touch $@ -t0$(VER)00
#この touch は日経バイトのディスクサービス版

deldev.exe : deldev.obj
	olink $*;
#	touch $@ -t0$(VER)00
#この touch は日経バイトのディスクサービス版

adddev.obj : adddev.asm adddel.inc
	optasm $*;

deldev.obj : deldev.asm adddel.inc
	optasm $*;

clean :
	-rm -f *.obj *.map *.sym *.com *.exe *.lzh *.ish *.bak *.bdf

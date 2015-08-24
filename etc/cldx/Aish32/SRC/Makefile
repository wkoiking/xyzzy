# aish/koish 1993,94,95 by H.Ogasawara (COR.)
# Makefile for C++ Builder 1997 by Hiroyuki Shimada.

#### *** Borland C++ Builder
CC	= bcc32
LK	= $(CC)
#CFLAGS	= -O2 -w! -DBCB=1 -DUNIX=1 -DUUENCODE=1 -DMIMEDEC=1 -DNOERR=1
CFLAGS	= -O2 -w -DBCB=1 -DUNIX=1 -DUUENCODE=1 -DMIMEDEC=1 -DNOERR=1 -D__BUILDING_THE_DLL=1
CPPFLAGS = $(CFLAGS)
#CPPFLAGS = $(CFLAGS) -DAISHMV_DEBUG
#LFLAGS = 
LFLAGS = -tWD
SYSOBJ	= ai_unix.obj
#WILDARGS = C:\PROGRA~1\BORLAND\CBUILDER3\LIB\WILDARGS.OBJ
####
####

# AISHMV32.DLL をコンパイルするときのフラグ
MVFLAG	= -DMVOLENC=1

AIOBJ	= aish32.obj ai_crc.obj ai_blk2.obj ai_fio.obj ai_fcrc.obj ai_7.obj \
	ai_8.obj ai_s.obj ai_n.obj ai_hed.obj ai_ish.obj ai_blk.obj ai_nam.obj \
	aish.obj ai_uue.obj mimedec.obj
OBJ	= $(STARTUP) $(AIOBJ) $(AIOBJAS) $(SYSOBJ)
MOBJ	= $(STARTUP) aishmv32.obj aishmv.obj ai_crc32.obj ai_crcmv.obj \
	ai_fiomv.obj ai_nam.obj $(SYSOBJ)
WINAPI_OBJ = wincur.obj

KOBJ	= koish.obj $(AIOBJ)
KLIB	= clib.a gnulib.a
PROG	= AISH32.DLL
LIB		= AISH32.LIB
MPROG	= AISHMV32.DLL
MLIB	= AISHMV32.LIB
KPROG	= koishenc.win
HEADER	= AISH32.H
MHEADER = AISHMV32.H
#DEFFILE	= AISH32.DEF
#MDEFFILE	= AISHMV32.DEF
####

all:	$(PROG) $(MPROG) $(LIB) $(MLIB)

$(PROG): $(OBJ) $(WINAPI_OBJ) $(DEFFILE)
	$(LK) $(LFLAGS) @&&|
		$(OBJ) $(WINAPI_OBJ) $(WILDARGS)
|
	ren $(PROG) $(PROG)

$(MPROG): $(MOBJ) $(WINAPI_OBJ) $(MDEFFILE)
	$(LK) $(LFLAGS) @&&|
		$(MOBJ) $(WINAPI_OBJ) $(WILDARGS)
|
	ren $(MPROG) $(MPROG)

$(LIB): $(PROG)
	implib $(LIB) $(PROG)

$(MLIB): $(MPROG)
	implib $(MLIB) $(MPROG)

$(KPROG): $(KOBJ)
	$(LK) $(KOBJ) -e$(KPROG) $(KLIB)

autodecode: autodecode.pl
	-@( if [ `which perl|wc -w` != 1 ];then \
	cp autodecode.pl $@ ;else \
	sed -e "1s=/.*=`which perl`=" autodecode.pl > $@;fi )
	-chmod 755 $@

aish32.obj:	aish32.cpp aish32.h aish.h ai_fio.h wincur.h
aishmv32.obj:	aishmv32.cpp aishmv32.h aishmv.h ai_fio.h
aish.obj:		aish.cpp ai.h microfio.h config.h
ai_ish.obj:	ai_ish.cpp ai.h microfio.h config.h
ai_fio.obj:	ai_fio.cpp microfio.h config.h
ai_fcrc.obj:	ai_fcrc.cpp microfio.h config.h
ai_hed.obj:	ai_hed.cpp ai.h microfio.h config.h
ai_blk.obj:	ai_blk.cpp ai.h ai_hed.h microfio.h config.h
ai_blk2.obj:	ai_blk2.cpp ai.h microfio.h config.h
ai_s.obj:		ai_s.cpp config.h
ai_crc.obj:	ai_crc.cpp config.h
ai_nam.obj:	ai_nam.cpp config.h
ai_uue.obj:	ai_uue.cpp ai.h config.h microfio.h
koish.obj:	koish.cpp ai.h microfio.h config.h
aishmv.obj:	aishmv.cpp config.h microfio.h
	$(CC) $(CPPFLAGS) $(MVFLAG) -c aishmv.cpp
ai_crc32.obj:	ai_crc32.cpp config.h
	$(CC) $(CPPFLAGS) $(MVFLAG) -c ai_crc32.cpp
ai_crcmv.obj:	ai_crc.cpp config.h
	$(CC) $(CPPFLAGS) $(MVFLAG) -o$@ -c ai_crc.cpp
ai_fiomv.obj:	ai_fio.cpp config.h microfio.h
	$(CC) $(CPPFLAGS) $(MVFLAG) -o$@ -c ai_fio.cpp
wincur.obj: wincur.cpp wincur.h

####
clean: clean_not_dll
	del $(PROG)
	del $(MPROG)
	del $(LIB)
	del $(MLIB)
	del test.exe
	del testmv.exe

clean_not_dll:
	del *.obj
	del core
	del *~
	del autodecode
	DEL *.BAK
	del *.tds

install: $(PROG) $(MPROG) $(LIB) $(MLIB) $(HEADER) $(MHEADER)
	copy $(PROG) ..\$(PROG)
	copy $(MPROG) ..\$(MPROG)
	copy $(LIB) ..
	copy $(MLIB) ..
	copy $(HEADER) ..\$(HEADER)
	copy $(MHEADER) ..\$(MHEADER)

####
.c.obj:
	$(CC) $(CFLAGS) -c $<
.cpp.obj:
	$(CC) $(CPPFLAGS) -c $<
####

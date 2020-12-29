# Makefile for Independent JPEG Group's software

# This makefile is suitable for Turbo C/C++ on MS-DOS.
# It works with Turbo C++ for DOS, revision 1.0 or later,
# and has been tested with Turbo C++ for MS-DOS.
# Thanks to Tom Wright and Ge' Weijers (original DOS) and
# Ken Porter (OS/2) for this file.
# Note: Uses large memory model

# Read installation instructions before saying "make" !!

# Are we under DOS or OS/2?
!if !$d(DOS) && !$d(OS2)
!if $d(__OS2__)
OS2=1
!else
DOS=1
!endif
!endif

# The name of your C compiler:
CC= tcc

# You may need to adjust these cc options:
!if $d(DOS)
CFLAGS= -O2 -ml -a- -b- -w-par -w-stu -w-ccc -w-rch
!else
CFLAGS= -O1 -w-par -w-stu -w-ccc -w-rch
!endif
# -O2 enables full code optimization (for pre-3.0 Borland C++, use -O -G -Z).
# -O2 is buggy in Borland OS/2 C++ revision 2.0, so use -O1 there for now.
# If you have Borland OS/2 C++ revision 1.0, use -O or no optimization at all.
# -mm selects medium memory model (near data, far code pointers; DOS only!)
# -w-par suppresses warnings about unused function parameters
# -w-stu suppresses warnings about incomplete structures
# -w-ccc suppresses warnings about compile-time-constant conditions
# -w-rch suppresses warnings about unreachable code
# Generally, we recommend defining any configuration symbols in jconfig.h,
# NOT via -D switches here.

# Link-time cc options:
!if $d(DOS)
LDFLAGS= -ml
# memory model option here must match CFLAGS!
!else
LDFLAGS=
# -lai full-screen app
# -lc case-significant link
!endif

# Put here the object file name for the correct system-dependent memory
# manager file.
# For DOS, we recommend jmemdos.c and jmemdosa.asm.
# For OS/2, we recommend jmemnobs.c (flat memory!)
# SYSDEPMEMLIB must list the same files with "+" signs for the librarian.
!if $d(DOS)
SYSDEPMEM= jmemdos.obj jmemdosa.obj
SYSDEPMEMLIB= +jmemdos.obj +jmemdosa.obj
!else
SYSDEPMEM= jmemnobs.obj
SYSDEPMEMLIB= +jmemnobs.obj
!endif

# End of configurable options.


# source files: JPEG library proper
LIBSOURCES= jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c \
        jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c \
        jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c \
        jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c \
        jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c \
        jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c \
        jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jquant1.c \
        jquant2.c jutils.c jmemmgr.c
# memmgr back ends: compile only one of these into a working library
SYSDEPSOURCES= jmemansi.c jmemname.c jmemnobs.c jmemdos.c jmemmac.c
# source files: cjpeg/djpeg/jpegtran applications, also rdjpgcom/wrjpgcom
APPSOURCES= cjpeg.c djpeg.c jpegtran.c rdjpgcom.c wrjpgcom.c cdjpeg.c \
        rdcolmap.c rdswitch.c transupp.c rdppm.c wrppm.c rdgif.c wrgif.c \
        rdtarga.c wrtarga.c rdbmp.c wrbmp.c rdrle.c wrrle.c
SOURCES= $(LIBSOURCES) $(SYSDEPSOURCES) $(APPSOURCES)
# files included by source files
INCLUDES= jdct.h jerror.h jinclude.h jmemsys.h jmorecfg.h jpegint.h \
        jpeglib.h jversion.h cdjpeg.h cderror.h transupp.h
# documentation, test, and support files
DOCS= README install.txt usage.txt cjpeg.1 djpeg.1 jpegtran.1 rdjpgcom.1 \
        wrjpgcom.1 wizard.txt example.c libjpeg.txt structure.txt \
        coderules.txt filelist.txt change.log
MKFILES= configure Makefile.in makefile.ansi makefile.unix makefile.b32 \
        makefile.bcc makefile.mc6 makefile.dj makefile.wat makefile.vc \
        makefile.vs makejdsw.vc6 makeadsw.vc6 makejdep.vc6 makejdsp.vc6 \
        makejmak.vc6 makecdep.vc6 makecdsp.vc6 makecmak.vc6 makeddep.vc6 \
        makeddsp.vc6 makedmak.vc6 maketdep.vc6 maketdsp.vc6 maketmak.vc6 \
        makerdep.vc6 makerdsp.vc6 makermak.vc6 makewdep.vc6 makewdsp.vc6 \
        makewmak.vc6 makejsln.v16 makeasln.v16 makejvcx.v16 makejfil.v16 \
        makecvcx.v16 makecfil.v16 makedvcx.v16 makedfil.v16 maketvcx.v16 \
        maketfil.v16 makervcx.v16 makerfil.v16 makewvcx.v16 makewfil.v16 \
        makeproj.mac makcjpeg.st makdjpeg.st makljpeg.st maktjpeg.st \
        makefile.manx makefile.sas makefile.mms makefile.vms makvms.opt
CONFIGFILES= jconfig.cfg jconfig.bcc jconfig.mc6 jconfig.dj jconfig.wat \
        jconfig.vc jconfig.mac jconfig.st jconfig.manx jconfig.sas \
        jconfig.vms
CONFIGUREFILES= config.guess config.sub install-sh ltmain.sh depcomp \
        missing ar-lib
OTHERFILES= jconfig.txt ckconfig.c jmemdosa.asm libjpeg.map libjpeg.pc.in
TESTFILES= testorig.jpg testimg.ppm testimg.gif testimg.bmp testimg.jpg \
        testprog.jpg testimgp.jpg
DISTFILES= $(DOCS) $(MKFILES) $(CONFIGFILES) $(SOURCES) $(INCLUDES) \
        $(CONFIGUREFILES) $(OTHERFILES) $(TESTFILES)
# library object files common to compression and decompression
COMOBJECTS= jaricom.obj jcomapi.obj jutils.obj jerror.obj jmemmgr.obj $(SYSDEPMEM)
# compression library object files
CLIBOBJECTS= jcapimin.obj jcapistd.obj jcarith.obj jctrans.obj jcparam.obj \
        jdatadst.obj jcinit.obj jcmaster.obj jcmarker.obj jcmainct.obj \
        jcprepct.obj jccoefct.obj jccolor.obj jcsample.obj jchuff.obj \
        jcdctmgr.obj jfdctfst.obj jfdctflt.obj jfdctint.obj
# decompression library object files
DLIBOBJECTS= jdapimin.obj jdapistd.obj jdarith.obj jdtrans.obj jdatasrc.obj \
        jdmaster.obj jdinput.obj jdmarker.obj jdhuff.obj jdmainct.obj \
        jdcoefct.obj jdpostct.obj jddctmgr.obj jidctfst.obj jidctflt.obj \
        jidctint.obj jdsample.obj jdcolor.obj jquant1.obj jquant2.obj \
        jdmerge.obj
# These objectfiles are included in libjpeg.lib
LIBOBJECTS= $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS)
# object files for sample applications (excluding library files)
COBJECTS= cjpeg.obj rdppm.obj rdgif.obj rdtarga.obj rdrle.obj rdbmp.obj \
        rdswitch.obj cdjpeg.obj
DOBJECTS= djpeg.obj wrppm.obj wrgif.obj wrtarga.obj wrrle.obj wrbmp.obj \
        rdcolmap.obj cdjpeg.obj
TROBJECTS= jpegtran.obj rdswitch.obj cdjpeg.obj transupp.obj


all: libjpeg.lib cjpeg.exe djpeg.exe jpegtran.exe rdjpgcom.exe wrjpgcom.exe

libjpeg.lib: $(LIBOBJECTS)
	- del libjpeg.lib
	tlib libjpeg.lib /E /C @&&|
+jcapimin.obj +jcapistd.obj +jcarith.obj +jctrans.obj +jcparam.obj &
+jdatadst.obj +jcinit.obj +jcmaster.obj +jcmarker.obj +jcmainct.obj &
+jcprepct.obj +jccoefct.obj +jccolor.obj +jcsample.obj +jchuff.obj &
+jcdctmgr.obj +jfdctfst.obj +jfdctflt.obj +jfdctint.obj +jdapimin.obj &
+jdapistd.obj +jdarith.obj +jdtrans.obj +jdatasrc.obj +jdmaster.obj &
+jdinput.obj +jdmarker.obj +jdhuff.obj +jdmainct.obj +jdcoefct.obj &
+jdpostct.obj +jddctmgr.obj +jidctfst.obj +jidctflt.obj +jidctint.obj &
+jdsample.obj +jdcolor.obj +jquant1.obj +jquant2.obj +jdmerge.obj &
+jaricom.obj +jcomapi.obj +jutils.obj +jerror.obj +jmemmgr.obj &
$(SYSDEPMEMLIB)
|

cjpeg.exe: $(COBJECTS) libjpeg.lib
	$(CC) $(LDFLAGS) -ecjpeg.exe $(COBJECTS) libjpeg.lib

djpeg.exe: $(DOBJECTS) libjpeg.lib
	$(CC) $(LDFLAGS) -edjpeg.exe $(DOBJECTS) libjpeg.lib

jpegtran.exe: $(TROBJECTS) libjpeg.lib
	$(CC) $(LDFLAGS) -ejpegtran.exe $(TROBJECTS) libjpeg.lib

rdjpgcom.exe: rdjpgcom.c
!if $d(DOS)
	$(CC) -ms -O rdjpgcom.c
!else
	$(CC) $(CFLAGS) rdjpgcom.c
!endif

# On DOS, wrjpgcom needs large model so it can malloc a 64K chunk
wrjpgcom.exe: wrjpgcom.c
!if $d(DOS)
	$(CC) -ml -O wrjpgcom.c
!else
	$(CC) $(CFLAGS) wrjpgcom.c
!endif

# This "{}" syntax allows Borland Make to "batch" source files.
# In this way, each run of the compiler can build many modules.
.c.obj:
	$(CC) $(CFLAGS) -c{ $<}

jconfig.h: jconfig.txt
	echo You must prepare a system-dependent jconfig.h file.
	echo Please read the installation directions in install.txt.
	exit 1

clean:
	- del *.obj
	- del libjpeg.lib
	- del cjpeg.exe
	- del djpeg.exe
	- del jpegtran.exe
	- del rdjpgcom.exe
	- del wrjpgcom.exe
	- del testout*.*

test: cjpeg.exe djpeg.exe jpegtran.exe
	- del testout*.*
        djpeg -dct int -ppm -outfile testout.ppm testorig.jpg
        djpeg -dct int -gif -outfile testout.gif testorig.jpg
        djpeg -dct int -bmp -colors 256 -outfile testout.bmp testorig.jpg
        cjpeg -dct int -outfile testout.jpg testimg.ppm
	djpeg -dct int -ppm -outfile testoutp.ppm testprog.jpg
	cjpeg -dct int -progressive -opt -outfile testoutp.jpg testimg.ppm
	jpegtran -outfile testoutt.jpg testprog.jpg
!if $d(DOS)
	fc /b testimg.ppm testout.ppm
	fc /b testimg.gif testout.gif
	fc /b testimg.bmp testout.bmp
	fc /b testimg.jpg testout.jpg
	fc /b testimg.ppm testoutp.ppm
	fc /b testimgp.jpg testoutp.jpg
	fc /b testorig.jpg testoutt.jpg
!else
	echo n > n.tmp
	comp testimg.ppm testout.ppm < n.tmp
        comp testimg.gif testout.gif < n.tmp
	comp testimg.bmp testout.bmp < n.tmp
	comp testimg.jpg testout.jpg < n.tmp
	comp testimg.ppm testoutp.ppm < n.tmp
	comp testimgp.jpg testoutp.jpg < n.tmp
	comp testorig.jpg testoutt.jpg < n.tmp
	del n.tmp
!endif


jaricom.obj: jaricom.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcapimin.obj: jcapimin.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcapistd.obj: jcapistd.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcarith.obj: jcarith.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jccoefct.obj: jccoefct.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jccolor.obj: jccolor.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcdctmgr.obj: jcdctmgr.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jchuff.obj: jchuff.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcinit.obj: jcinit.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcmainct.obj: jcmainct.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcmarker.obj: jcmarker.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcmaster.obj: jcmaster.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcomapi.obj: jcomapi.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcparam.obj: jcparam.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcprepct.obj: jcprepct.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jcsample.obj: jcsample.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jctrans.obj: jctrans.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdapimin.obj: jdapimin.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdapistd.obj: jdapistd.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdarith.obj: jdarith.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdatadst.obj: jdatadst.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h
jdatasrc.obj: jdatasrc.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h
jdcoefct.obj: jdcoefct.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdcolor.obj: jdcolor.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jddctmgr.obj: jddctmgr.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jdhuff.obj: jdhuff.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdinput.obj: jdinput.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdmainct.obj: jdmainct.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdmarker.obj: jdmarker.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdmaster.obj: jdmaster.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdmerge.obj: jdmerge.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdpostct.obj: jdpostct.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdsample.obj: jdsample.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jdtrans.obj: jdtrans.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jerror.obj: jerror.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jversion.h jerror.h
jfdctflt.obj: jfdctflt.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jfdctfst.obj: jfdctfst.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jfdctint.obj: jfdctint.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jidctflt.obj: jidctflt.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jidctfst.obj: jidctfst.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jidctint.obj: jidctint.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jdct.h
jquant1.obj: jquant1.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jquant2.obj: jquant2.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jutils.obj: jutils.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h
jmemmgr.obj: jmemmgr.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jmemsys.h
jmemansi.obj: jmemansi.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jmemsys.h
jmemname.obj: jmemname.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jmemsys.h
jmemnobs.obj: jmemnobs.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jmemsys.h
jmemdos.obj: jmemdos.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jmemsys.h
jmemmac.obj: jmemmac.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h jmemsys.h
cjpeg.obj: cjpeg.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h jversion.h
djpeg.obj: djpeg.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h jversion.h
jpegtran.obj: jpegtran.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h transupp.h jversion.h
rdjpgcom.obj: rdjpgcom.c jinclude.h jconfig.h
wrjpgcom.obj: wrjpgcom.c jinclude.h jconfig.h
cdjpeg.obj: cdjpeg.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
rdcolmap.obj: rdcolmap.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
rdswitch.obj: rdswitch.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
transupp.obj: transupp.c jinclude.h jconfig.h jpeglib.h jmorecfg.h jpegint.h jerror.h transupp.h
rdppm.obj: rdppm.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
wrppm.obj: wrppm.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
rdgif.obj: rdgif.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
wrgif.obj: wrgif.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
rdtarga.obj: rdtarga.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
wrtarga.obj: wrtarga.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
rdbmp.obj: rdbmp.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
wrbmp.obj: wrbmp.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
rdrle.obj: rdrle.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
wrrle.obj: wrrle.c cdjpeg.h jinclude.h jconfig.h jpeglib.h jmorecfg.h jerror.h cderror.h
jmemdosa.obj: jmemdosa.asm
	tasm /mx jmemdosa.asm

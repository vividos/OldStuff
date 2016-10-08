# Microsoft Developer Studio Generated NMAKE File, Based on d64view.dsp
!IF "$(CFG)" == ""
CFG=d64view - Win32 Debug
!MESSAGE No configuration specified. Defaulting to d64view - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "d64view - Win32 Release" && "$(CFG)" != "d64view - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "d64view.mak" CFG="d64view - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "d64view - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "d64view - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "d64view - Win32 Release"

OUTDIR=.\release
INTDIR=.\release\intermediate
# Begin Custom Macros
OutDir=.\release
# End Custom Macros

ALL : "$(OUTDIR)\d64view.exe"


CLEAN :
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\bamview.obj"
	-@erase "$(INTDIR)\canvas.obj"
	-@erase "$(INTDIR)\chargen.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\childframe.obj"
	-@erase "$(INTDIR)\d64image.obj"
	-@erase "$(INTDIR)\d64view.pch"
	-@erase "$(INTDIR)\d64view_res.res"
	-@erase "$(INTDIR)\ddeserv.obj"
	-@erase "$(INTDIR)\dirview.obj"
	-@erase "$(INTDIR)\doctempl.obj"
	-@erase "$(INTDIR)\document.obj"
	-@erase "$(INTDIR)\imagebase.obj"
	-@erase "$(INTDIR)\mainframe.obj"
	-@erase "$(INTDIR)\pch.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\d64view.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\d64view_res.res" /i "$(wxwin)/include" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\d64view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib regex.lib fl.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\d64view.pdb" /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /out:"$(OUTDIR)\d64view.exe" /libpath:"$(wxwin)/lib" 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\bamview.obj" \
	"$(INTDIR)\canvas.obj" \
	"$(INTDIR)\chargen.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\childframe.obj" \
	"$(INTDIR)\d64image.obj" \
	"$(INTDIR)\ddeserv.obj" \
	"$(INTDIR)\dirview.obj" \
	"$(INTDIR)\doctempl.obj" \
	"$(INTDIR)\document.obj" \
	"$(INTDIR)\imagebase.obj" \
	"$(INTDIR)\mainframe.obj" \
	"$(INTDIR)\pch.obj" \
	"$(INTDIR)\d64view_res.res"

"$(OUTDIR)\d64view.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

OUTDIR=.\debug
INTDIR=.\debug\intermediate
# Begin Custom Macros
OutDir=.\debug
# End Custom Macros

ALL : "$(OUTDIR)\d64view.exe"


CLEAN :
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\bamview.obj"
	-@erase "$(INTDIR)\canvas.obj"
	-@erase "$(INTDIR)\chargen.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\childframe.obj"
	-@erase "$(INTDIR)\d64image.obj"
	-@erase "$(INTDIR)\d64view.pch"
	-@erase "$(INTDIR)\d64view_res.res"
	-@erase "$(INTDIR)\ddeserv.obj"
	-@erase "$(INTDIR)\dirview.obj"
	-@erase "$(INTDIR)\doctempl.obj"
	-@erase "$(INTDIR)\document.obj"
	-@erase "$(INTDIR)\imagebase.obj"
	-@erase "$(INTDIR)\mainframe.obj"
	-@erase "$(INTDIR)\pch.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\d64view.exe"
	-@erase "$(OUTDIR)\d64view.ilk"
	-@erase "$(OUTDIR)\d64view.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\d64view_res.res" /i "$(wxwin)/include" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\d64view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib regexd.lib fld.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\d64view.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /out:"$(OUTDIR)\d64view.exe" /pdbtype:sept /libpath:"$(wxwin)/lib" 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\bamview.obj" \
	"$(INTDIR)\canvas.obj" \
	"$(INTDIR)\chargen.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\childframe.obj" \
	"$(INTDIR)\d64image.obj" \
	"$(INTDIR)\ddeserv.obj" \
	"$(INTDIR)\dirview.obj" \
	"$(INTDIR)\doctempl.obj" \
	"$(INTDIR)\document.obj" \
	"$(INTDIR)\imagebase.obj" \
	"$(INTDIR)\mainframe.obj" \
	"$(INTDIR)\pch.obj" \
	"$(INTDIR)\d64view_res.res"

"$(OUTDIR)\d64view.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("d64view.dep")
!INCLUDE "d64view.dep"
!ELSE 
!MESSAGE Warning: cannot find "d64view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "d64view - Win32 Release" || "$(CFG)" == "d64view - Win32 Debug"
SOURCE=..\source\app.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\app.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\app.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\bamview.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bamview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\bamview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\canvas.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\canvas.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\canvas.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\chargen.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chargen.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\chargen.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\charset.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\charset.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\charset.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\childframe.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\childframe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\childframe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\d64image.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\d64image.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\d64image.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\ddeserv.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ddeserv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ddeserv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\dirview.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dirview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dirview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\doctempl.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\doctempl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\doctempl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\document.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\document.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\document.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\imagebase.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\imagebase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\imagebase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\mainframe.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mainframe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yu"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\mainframe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\d64view.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\pch.cpp

!IF  "$(CFG)" == "d64view - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yc"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pch.obj"	"$(INTDIR)\d64view.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /Fp"$(INTDIR)\d64view.pch" /Yc"common.hpp" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pch.obj"	"$(INTDIR)\d64view.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\d64view_res.rc

"$(INTDIR)\d64view_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


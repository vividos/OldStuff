# Microsoft Developer Studio Project File - Name="d64view" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=d64view - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "d64view.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "d64view - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /i "$(wxwin)/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib regex.lib fl.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(wxwin)/lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "d64view - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /i "$(wxwin)/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib regexd.lib fld.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(wxwin)/lib"

!ENDIF 

# Begin Target

# Name "d64view - Win32 Release"
# Name "d64view - Win32 Debug"
# Begin Group "source files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\app.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\bamview.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\canvas.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\chargen.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\charset.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\childframe.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\d64image.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\ddeserv.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\dirview.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\doctempl.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\document.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\imagebase.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\mainframe.cpp
# ADD CPP /Yu"common.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\pch.cpp
# ADD CPP /Yc"common.hpp"
# End Source File
# End Group
# Begin Group "header files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\source\about.hpp
# End Source File
# Begin Source File

SOURCE=..\source\app.hpp
# End Source File
# Begin Source File

SOURCE=..\source\bamview.hpp
# End Source File
# Begin Source File

SOURCE=..\source\canvas.hpp
# End Source File
# Begin Source File

SOURCE=..\source\charset.hpp
# End Source File
# Begin Source File

SOURCE=..\source\childframe.hpp
# End Source File
# Begin Source File

SOURCE=..\source\common.hpp
# End Source File
# Begin Source File

SOURCE=..\source\d64image.hpp
# End Source File
# Begin Source File

SOURCE=..\source\dirview.hpp
# End Source File
# Begin Source File

SOURCE=..\source\doctempl.hpp
# End Source File
# Begin Source File

SOURCE=..\source\document.hpp
# End Source File
# Begin Source File

SOURCE=..\source\imagebase.hpp
# End Source File
# Begin Source File

SOURCE=..\source\mainframe.hpp
# End Source File
# Begin Source File

SOURCE=..\source\version.hpp
# End Source File
# End Group
# Begin Group "resource files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\d64.ico
# End Source File
# Begin Source File

SOURCE=.\d64view.ico
# End Source File
# Begin Source File

SOURCE=.\d64view_res.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# End Target
# End Project

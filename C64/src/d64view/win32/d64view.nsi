#
# d64view nullsoft install script
#
# $Id: d64view.nsi,v 1.9 2003/05/19 21:17:32 vividos Exp $
#

!define VERSION "0.99"

# compiler utility commands
#
!packhdr temp.dat "upx -9 temp.dat"

# general installer config
#
OutFile "d64view-${VERSION}-win32.exe"
Name "d64view '${VERSION}'"
CRCCheck on

# install directory config
#
InstallDir "$PROGRAMFILES\d64view"

# license page config
#
#LicenseText "You should read the following license before installing."
#LicenseData "License.installer.txt"

# component page config
#
ComponentText "This will install d64view '${VERSION}' on your system."
InstType Typical

# directory page config
#
DirShow show
DirText "Please select a location to install d64view '${VERSION}' (or use the default)."

# install page config
#
AutoCloseWindow false
ShowInstDetails show

# uninstall config
#
UninstallText "This will uninstall d64view '${VERSION}' from your system."
ShowUninstDetails show

# compiler flags
#
SetOverwrite ifnewer
SetCompress auto
SetDatablockOptimize on
SetDateSave on

#
# installation execution commands
#

#
# section: all needed d64view files
#
Section -
SetOutPath $INSTDIR
File release\d64view.exe
File ..\Readme
File ..\Copying
WriteUninstaller $INSTDIR\uninst-d64view.exe

WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\d64view" "DisplayName" "d64view '${VERSION}' (remove only)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\d64view" "UninstallString" '"$INSTDIR\uninst-d64view.exe"'

WriteRegStr HKEY_CLASSES_ROOT ".d64" "" "D64view.Document"
WriteRegStr HKEY_CLASSES_ROOT "D64view.Document" "" "d64 disk image"
WriteRegStr HKEY_CLASSES_ROOT "D64view.Document\shell\open\command" "" "$INSTDIR\d64view.exe %1"
WriteRegStr HKEY_CLASSES_ROOT "D64view.Document\shell\open\ddeexec" "" '[open("%1")]'
WriteRegStr HKEY_CLASSES_ROOT "D64view.Document\shell\open\ddeexec\application" "" "d64view"
WriteRegStr HKEY_CLASSES_ROOT "D64view.Document\DefaultIcon" "" "$INSTDIR\d64view.exe,12"

SectionEnd

#
# section: uninstall
#
Section Uninstall
Delete $INSTDIR\d64view.exe
Delete $INSTDIR\d64view.ini
Delete $INSTDIR\Readme
Delete $INSTDIR\Copying
Delete $INSTDIR\uninst-d64view.exe
RMDir $INSTDIR

DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures"
DeleteRegKey HKEY_CLASSES_ROOT ".d64"
DeleteRegKey HKEY_CLASSES_ROOT "D64view.Document"

SectionEnd

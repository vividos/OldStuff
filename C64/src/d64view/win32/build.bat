nmake -f d64view.mak CFG="d64view - Win32 Release"
upx --best --compress-icons=0 release\d64view.exe
makensis d64view.nsi

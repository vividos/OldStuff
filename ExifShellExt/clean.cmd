@echo off
regsvr32 /u /s bin\Debug\ExifShellExt.dll
regsvr32 /u /s bin\Release\ExifShellExt.dll
del /f /q bin/*.*
del /f /q intermediate/*.*
rmdir /q bin
rmdir /q intermediate
del src\dlldata.c
del src\ExifShellExt_h.h
del src\ExifShellExt_i.c
del src\ExifShellExt_p.c
del *.ncb
del *.suo

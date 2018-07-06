REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for the standalone intro

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch\

ca65 ..\shared\intro.s -o intro-standalone.o -D EXIT_ACTION_SOFT_RESET -t c64
ca65 scrolltext.s -t c64
ld65 -C ..\shared\intro.cfg intro-standalone.o scrolltext.o -o intro-standalone.prg
pucrunch.exe -f -c64 -i1 -g0x37 intro-standalone.prg intro-standalone-pucrunch.prg
c1541 < c1541script.txt

pause

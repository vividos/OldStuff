REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE

ca65 ..\shared\intro.s -o intro-standalone.o -D EXIT_ACTION_SOFT_RESET -t c64
ca65 scrolltext.s -t c64
ld65 -C intro.cfg intro-standalone.o scrolltext.o -o intro.prg
c1541 < c1541script.txt

pause

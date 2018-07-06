REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for the intro for Upfront's Freezer demo

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch\

ca65 ..\shared\intro.s -o intro-upfront-freezer.o -D EXIT_ACTION_START_PAYLOAD -t c64
ca65 scrolltext.s -t c64
ca65 ..\shared\copy_prg_and_start.s -o copy_prg_and_start-upfront-freezer.o -D PRG_START_ADDR=$0818 -D NUM_BLOCKS=89
ca65 prg.s -t c64
ld65 -C ..\shared\intro.cfg intro-upfront-freezer.o scrolltext.o copy_prg_and_start-upfront-freezer.o prg.o -o intro-upfront-freezer.prg
pucrunch.exe -f -c64 -i1 -g0x37 intro-upfront-freezer.prg intro-upfront-freezer-pucrunch.prg
c1541 < c1541script.txt

pause

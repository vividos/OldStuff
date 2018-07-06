REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for the intro for Hal's Barbaric demo

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch\

ca65 ..\shared\intro.s -o intro-hal-barbaric.o -D EXIT_ACTION_START_PAYLOAD -t c64
ca65 scrolltext.s -t c64
ca65 ..\shared\copy_prg_and_start.s -o copy_prg_and_start-hal-barbaric.o -D PRG_START_ADDR=$2000 -D NUM_BLOCKS=68
ca65 prg.s -t c64
ld65 -C ..\shared\intro.cfg intro-hal-barbaric.o scrolltext.o copy_prg_and_start-hal-barbaric.o prg.o -o intro-hal-barbaric.prg
pucrunch.exe -f -c64 -i1 -g0x37 intro-hal-barbaric.prg intro-hal-barbaric-pucrunch.prg
c1541 < c1541script.txt

pause

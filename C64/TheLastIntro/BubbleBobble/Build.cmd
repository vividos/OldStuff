REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for the intro for Bubble Bobble

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch\

pucrunch.exe -ffast -c64 -x0x4460 -i1 -g0x37 ..\games\bubble-bobble.prg bubble-bobble-pucrunch.prg
ca65 ..\shared\intro.s -o intro-bubble-bobble.o -D EXIT_ACTION_START_PAYLOAD -t c64
ca65 scrolltext.s -t c64
ca65 ..\shared\copy_prg_and_start.s -o copy_prg_and_start-bubble-bobble.o -D PRG_START_ADDR=$080d -D NUM_BLOCKS=150
ca65 prg.s -t c64
ld65 -C ..\shared\intro.cfg intro-bubble-bobble.o scrolltext.o copy_prg_and_start-bubble-bobble.o prg.o -o intro-bubble-bobble.prg
c1541 < c1541script.txt

pause

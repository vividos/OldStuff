REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for Sha-Jongg with intro

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch\

ca65 ..\shared\intro.s -o intro-shajongg.o -D EXIT_ACTION_START_PAYLOAD -t c64
ca65 scrolltext.s -t c64
ca65 ..\shared\copy_prg_and_start.s -o copy_prg_and_start-shajongg.o -D PRG_START_ADDR=$0812 -D NUM_BLOCKS=58
ca65 prg.s -t c64
ld65 -C intro-shajongg.cfg intro-shajongg.o scrolltext.o copy_prg_and_start-shajongg.o prg.o -o intro-shajongg.prg
pucrunch.exe -f -c64 -i1 -g0x37 intro-shajongg.prg intro-shajongg-pucrunch.prg
c1541 < c1541script.txt

pause

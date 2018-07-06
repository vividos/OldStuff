REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for the intro for Giana Sisters

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch

pucrunch.exe -ffast -c64 -x0x0832 -i1 -g0x37 ..\games\giana-sisters-game.prg giana-sisters-game-pucrunch.prg
ca65 ..\shared\intro.s -o intro-giana-sisters.o -D EXIT_ACTION_START_PAYLOAD -t c64
ca65 scrolltext.s -t c64
ca65 ..\shared\copy_prg_and_start.s -o copy_prg_and_start-giana-sisters.o -D PRG_START_ADDR=$080e -D NUM_BLOCKS=129
ca65 prg.s -t c64
ld65 -C ..\shared\intro.cfg intro-giana-sisters.o scrolltext.o copy_prg_and_start-giana-sisters.o prg.o -o intro-giana-sisters.prg
pucrunch.exe -f -c64 -i1 -g0x37 intro-giana-sisters.prg intro-giana-sisters-pucrunch.prg
c1541 < c1541script.txt

pause

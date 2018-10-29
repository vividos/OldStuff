REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for the DiskMenu program

set PATH=%PATH%;%CD%\..\tools\cc65\bin;%CD%\..\tools\winVICE;%CD%\..\tools\pucrunch\

ca65 diskmenu.s -o diskmenu.o -t c64
ld65 -C diskmenu.cfg diskmenu.o -o diskmenu.prg
pucrunch.exe -f -c64 -i1 -g0x37 diskmenu.prg diskmenu-pucrunch.prg
c1541 < c1541script.txt

pause

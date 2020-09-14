REM
REM compiles libjpeg for Turbo C++ 1.01/3.00
REM
copy makefile.tcc makefile
copy jconfig.tcc jconfig.h

set PATH=%PATH%;c:\tc\bin

make clean
make libjpeg.lib

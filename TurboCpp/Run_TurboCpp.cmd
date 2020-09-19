REM
REM This runs dosbox, mounts this folder and sets the PATH to the Turbo C++ bin folder
REM
set PATH=%PATH%;C:\...\DOSBOX

mkdir proj\bin
mkdir proj\obj

dosbox.exe ^
   -c "mount c %CD%" ^
   -c "mount d d:\ -t cdrom " ^
   -c "C:" ^
   -c "set PATH=C:\TC\BIN"

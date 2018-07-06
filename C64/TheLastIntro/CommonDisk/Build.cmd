REM -------------------------------------------------------------	
REM
REM vividos' last intro
REM (c) 2018 vividos
REM
REM -------------------------------------------------------------	

REM Build.cmd - Build script for a common disc, containing the
REM standalone intro, all games and demos

pushd ..\Standalone && call Build.cmd && popd
pushd ..\ShaJongg && call Build.cmd && popd
pushd ..\HalBarbaric && call Build.cmd && popd
pushd ..\UpfrontFreezer && call Build.cmd && popd
pushd ..\GianaSisters && call Build.cmd && popd
pushd ..\BubbleBobble && call Build.cmd && popd

c1541 < c1541script.txt

pushd ..\Standalone && call Clean.cmd && popd
pushd ..\ShaJongg && call Clean.cmd && popd
pushd ..\HalBarbaric && call Clean.cmd && popd
pushd ..\UpfrontFreezer && call Clean.cmd && popd
pushd ..\GianaSisters && call Clean.cmd && popd
pushd ..\BubbleBobble && call Clean.cmd && popd

pause

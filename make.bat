@echo off
REM run via Developer Command Prompt for VS 2019

cd resources/
echo clean...
del index.res
echo bundle...
rc.exe /r index.rc
cd ..

cd build/
echo clean...
del main.obj main.exe

echo compile and link...
cl ../main.cpp /std:c++17 /DUNICODE /D_UNICODE /link user32.lib gdi32.lib winmm.lib ../resources/index.res
cd ..
pause

echo execute...
cd build/
main.exe
cd ..


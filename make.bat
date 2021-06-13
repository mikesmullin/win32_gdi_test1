REM run via Developer Command Prompt for VS 2019

cd build/
echo clean...
del /y main.obj main.exe

echo compile and link...
cl ../main.cpp /std:c++17 /DUNICODE /D_UNICODE /link user32.lib
pause

cd ..
echo execute...
"build/main.exe"
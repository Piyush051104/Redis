@echo off
set PATH=C:\msys64\mingw64\bin;%PATH%
cd build
mingw32-make
echo Build complete!
pause
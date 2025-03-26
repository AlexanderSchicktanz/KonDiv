@echo off
echo > Setup required Environment
echo -------------------------------------

:: Set the paths
SET RAYLIB_PATH=C:\raylib\raylib
SET COMPILER_PATH=C:\raylib\w64devkit\bin
SET CC=gcc

:: Set environment variables
SET PATH=%COMPILER_PATH%;%PATH%
SET CFLAGS=-s -O2 -DNDEBUG -fvisibility=hidden -Wl,--gc-sections -std=c99 -static -Wall -I%RAYLIB_PATH%\src -Iexternal -DPLATFORM_DESKTOP
SET LDFLAGS=-lraylib -lopengl32 -lgdi32 -lwinmm

:: Get the file name without the extension from the argument
SET FILE_NAME=main.c
SET NAME_PART=kd

echo.
echo > Clean latest build
echo ------------------------

:: Delete the existing .exe file if it exists
IF EXIST %NAME_PART%.exe del /F %NAME_PART%.exe

echo.
echo > Saving Current File
echo -------------------------

echo.
echo > Compile program
echo -----------------------

:: Show the compiler version
%CC% --version

:: Compile the code
%CC% -o %NAME_PART%.exe %FILE_NAME% %CFLAGS% %LDFLAGS%
:: tcc -o flame.exe flame.c -O2 -DNDEBUG -fvisibility=hidden -Wl,-IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -lraylib -lopengl32 -lgdi32 -lwinmm

echo.
echo > Reset Environment
echo --------------------------
:: Reset the PATH environment variable
SET PATH=%PATH:;%COMPILER_PATH%=%

echo.
echo > Execute program
echo -----------------------

:: Run the compiled program if it exists
IF EXIST %NAME_PART%.exe %NAME_PART%.exe

REM emcc -o index.html main.c -Wall -std=gnu99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I C:/raylib/raylib/src -I C:/raylib/raylib/src/external -L. -L C:/raylib/raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file resources --shell-file C:/raylib/raylib/src/customshell.html C:/raylib/raylib/src/web/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS="['_free', '_malloc', '_main','_setSize']" -s EXPORTED_RUNTIME_METHODS="['ccall']"

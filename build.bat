@echo off
mkdir build
pushd build
del *.pdb
rem Files
rem LibsPath
rem Libs
rem Includes
set FilesDebug= ..\win32_typingGame.cpp
set Includes= ..\Libs\include 
set LibsPath=/LIBPATH:"..\Libs\lib\"
set Libs=kernel32.lib user32.lib shell32.lib gdi32.lib opengl32.lib glfw3.lib irrKlang.lib
echo Win32API build

echo %time%
cl /DDEBUG /DTYPING_INTERNAL /FeTypingGame.exe /MD -FC -Zi %FilesDebug% /I  %Includes% /link %LibsPath% %Libs% /NODEFAULTLIB:MSVCRTD -subsystem:windows
rem cl /DDEBUG /DDEBUG_TIME_STEP /FeCardGame.exe /MD -FC -Zi %FilesDebug% /I  %Includes% /link %LibsPath% %Libs% /NODEFAULTLIB:MSVCRTD -subsystem:console
echo %time%

echo Creating Game DLL
echo %time%
cl /LD  ../typinggame.cpp /Fegame.dll -Zi /link -PDB:game_%random%.pdb
echo %time%

echo Compiling and linking Shaders
echo %time%
ShaderCompiler.exe
echo %time%

rem cl /DDEBUG /DEBUG /FeAssetTester.exe /MD -FC -Zi ../AssetTester.cpp 

rem cl /DDEBUG /DEBUG /FeShaderCompiler.exe /MD -FC -Zi ../ShaderCompiler.cpp /I  %Includes% /link %LibsPath% %Libs% /NODEFAULTLIB:MSVCRTD -subsystem:console

popd 
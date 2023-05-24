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
set Libs=kernel32.lib user32.lib shell32.lib gdi32.lib opengl32.lib glfw3.lib
rem Compile
cl /DDEBUG /FeTypingGame.exe /MD -FC -Zi %FilesDebug% /I  %Includes% /link %LibsPath% %Libs% /NODEFAULTLIB:MSVCRTD -subsystem:windows
rem cl /DDEBUG /DDEBUG_TIME_STEP /FeCardGame.exe /MD -FC -Zi %FilesDebug% /I  %Includes% /link %LibsPath% %Libs% /NODEFAULTLIB:MSVCRTD -subsystem:console


cl /LD  ../typinggame.cpp /Fegame.dll -Zi /link -PDB:game_%random%.pdb

popd 
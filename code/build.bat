@echo off

mkdir ..\build
pushd ..\build
cl /Zi ..\code\win32_scratch.cpp user32.lib
popd
@echo off

mkdir ..\..\build
pushd ..\..\build
cl /Zi ..\NotHandMadeHero_oxoxoxo\code\win32_scratch.cpp user32.lib Gdi32.lib
popd
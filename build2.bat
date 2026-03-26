@echo off
IF not exist build mkdir build
call cl /std:c++20 /Od /nologo /FC /Z7 main.cpp imgui/imgui*.cpp /Fe:build\main.exe /Fo:build\

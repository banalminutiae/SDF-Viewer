@echo off
call cl /std:c++20 /Od /nologo /FC /Z7 main.cpp imgui/imgui*.cpp

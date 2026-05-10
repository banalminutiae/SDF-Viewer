@echo off
IF not exist build mkdir build
pushd build
call cl /std:c++20 /Od /nologo /FC /Z7 ..\main.cpp ..\imgui\imgui*.cpp /Fe:main.exe
popd
copy pixel.hlsl build
copy vertex.hlsl build
copy common.hlsl build
build\main.exe

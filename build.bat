@echo off
REM Can call this script from anywhere as long as it is located in projected root dir

REM NOTE(Gonk): cd %~dp0 so you can call this from anywhere
pushd %~dp0

set outputDir=build

if not exist "%outputDir%" mkdir "%outputDir%"
pushd "%outputDir%"

set debug_compiler_opts=/Od /Zi /DBUILD_DEBUG /Fd /Fm /Fo

REM /wd4127 - conditional expression is constant
REM /wd5045 - spectre stuff

set compiler_opts=/Wall /wd4127 /wd5045 /nologo /std:c11 /FC /Featlas_cli.exe
REM set compiler_opts=%compiler_opts% %debug_compiler_opts%
set compiler_opts=%compiler_opts% /O2 /wd4711 /wd4710

set linker_opts=user32.lib /INCREMENTAL:NO /SUBSYSTEM:CONSOLE

cl ..\atlas_cli.c %compiler_opts% /link %linker_opts%

popd
popd
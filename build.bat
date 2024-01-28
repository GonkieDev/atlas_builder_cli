@echo off
REM Can call this script from anywhere as long as it is located in projected root dir

REM NOTE(Gonk): cd %~dp0 so you can call this from anywhere
pushd %~dp0

set outputDir=build

if not exist "%outputDir%" mkdir "%outputDir%"
pushd "%outputDir%"

set debug_compiler_opts=/Od /Zi /DBUILD_DEBUG /Fd /Fm /Fo
set compiler_opts=%debug_compiler_opts%

REM /wd4127 - conditional expression is constant

cl ..\sprite_atlas_cli.c /Wall /wd4127 /wd5045 %compiler_opts% /nologo /std:c11 /FC /link user32.lib /INCREMENTAL:NO /SUBSYSTEM:CONSOLE

popd
popd
@echo off
pushd %~dp0

call build.bat RELEASE
if exist .\build\atlas_cli.zip del .\build\atlas_cli.zip
pushd build
7z a -tzip .\atlas_cli.zip atlas_cli.exe
popd

popd
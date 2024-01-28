@echo off
pushd %~dp0

call build.bat RELEASE

set ship_files_dir=ship_files
if not exist "%ship_files_dir%" mkdir "%ship_files_dir%"

xcopy build\atlas_cli.exe %ship_files_dir% /h /i /c /k /e /r /y
7z a -tzip atlas_cli.zip %ship_files_dir%

popd
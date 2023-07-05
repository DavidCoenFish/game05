::pip install pyinstaller

::@ECHO OFF
PUSHD %~dp0

::C:\Python310\python.exe
pyinstaller --onefile drag_drop_file_rename_snake_case.py
xcopy dist\drag_drop_file_rename_snake_case.exe drag_drop_file_rename_snake_case.exe /Y

POPD

exit /b %ERRORLEVEL%

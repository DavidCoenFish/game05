::pip install pyinstaller

::@ECHO OFF
PUSHD %~dp0

::C:\Python310\python.exe
pyinstaller --onefile version.py
xcopy dist\version.exe version.exe /Y

POPD

exit /b %ERRORLEVEL%

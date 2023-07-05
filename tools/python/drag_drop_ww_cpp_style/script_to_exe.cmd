::pip install pyinstaller

::@ECHO OFF
PUSHD %~dp0

::C:\Python310\python.exe
pyinstaller --onefile drag_drop_ww_cpp_style.py
xcopy dist\drag_drop_ww_cpp_style.exe drag_drop_ww_cpp_style.exe /Y

POPD

exit /b %ERRORLEVEL%

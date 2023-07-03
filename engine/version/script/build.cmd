::build <1:reposity root> <2:relative dir to project> <3:output dir> <4:version major> <5:version minor> <6:version patch> <7:version store> <8:verbose>
::@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2 %~3 %~4 %~5 %~6 %~7 %~8

::1 C:\development\game04
::2 support\s01_d3dx12_engine
::3 C:\development\game04\support\s01_d3dx12_engine\Version00\Output
::4 0
::5 0
::6 0
::7 0
::8 True

ECHO version.exe "%~1" "%~2" "%~3" "%~4" "%~5" "%~6" "%~7" %~8

version.exe "%~1" "%~2" "%~3" "%~4" "%~5" "%~6" "%~7" %~8

POPD

exit /b %ERRORLEVEL%

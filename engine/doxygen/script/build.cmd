::build <1:reposity root> <2:relative dir to project> <3:output dir> <4:version major> <5:version minor> <6:version patch> <7:version store> <8:verbose>
::@ECHO OFF

echo %~nx0 %~1 %~2 %~3 %~4 %~5 %~6 %~7 %~8

PUSHD "%~1"

SET DOXYGEN_EXE=C:\Program Files\doxygen\bin\doxygen.exe

SET /p PROJECT_NUMBER=<version\output\build_version.txt
SET PROJECT_NUMBER=%PROJECT_NUMBER:"=%

"%DOXYGEN_EXE%" doxygen_config.ini

POPD

exit /b %ERRORLEVEL%

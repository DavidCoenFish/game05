@ECHO OFF
PUSHD %~dp0

cls

echo %time% %~nx0 %*

SET AUTOMOTRON_EXE="%~dp0..\..\build\s01_d3dx12_engine\Automatron03\Release\bin\net5.0\Automatron03.exe"
SET AUTOMOTRON_DLL="%~dp0..\..\build\s01_d3dx12_engine\Automatron03\Release\bin\net5.0\Automatron03.dll"
SET DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com"

SET MISSING_AUTOMATRON=
IF NOT EXIST %AUTOMOTRON_EXE% SET MISSING_AUTOMATRON=TRUE
IF NOT EXIST %AUTOMOTRON_DLL% SET MISSING_AUTOMATRON=TRUE

IF defined MISSING_AUTOMATRON (
   echo %DEVENV% "%~dp0s01_d3dx12_engine.sln" /build "Release|x64" /project Automatron03
   call %DEVENV% "%~dp0s01_d3dx12_engine.sln" /build "Release|x64" /project Automatron03
   )

set COMMENT=%*
set COMMENT=%COMMENT:"=%
echo %AUTOMOTRON_EXE% %~dp0..\..\ %~dp0Automatron03\Tasks "%COMMENT%"
call %AUTOMOTRON_EXE% %~dp0..\..\ %~dp0Automatron03\Tasks "%COMMENT%"
if %errorlevel% NEQ 0 (
   echo Automatron03.exe exited with %errorlevel%
   exit /b %errorlevel%
   )

POPD

exit /b 0



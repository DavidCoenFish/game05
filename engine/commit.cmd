@ECHO OFF
PUSHD %~dp0

cls

echo %time% %~nx0 %*

SET AUTOMOTRON_EXE="%~dp0..\build\engine\automatron\Release\bin\net5.0\automatron.exe"
SET AUTOMOTRON_DLL="%~dp0..\build\engine\automatron\Release\bin\net5.0\automatron.dll"
SET DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com"

::msbuild -t:restore

echo %AUTOMOTRON_EXE%
echo %AUTOMOTRON_DLL%
echo %DEVENV%

SET MISSING_AUTOMATRON=
IF NOT EXIST %AUTOMOTRON_EXE% SET MISSING_AUTOMATRON=TRUE
IF NOT EXIST %AUTOMOTRON_DLL% SET MISSING_AUTOMATRON=TRUE

IF defined MISSING_AUTOMATRON (
   echo %DEVENV% "%~dp0engine.sln" /build "Release|x64" /project automatron
   call %DEVENV% "%~dp0engine.sln" /build "Release|x64" /project automatron
   )

set COMMENT=%*
set COMMENT=%COMMENT:"=%
echo %AUTOMOTRON_EXE% %~dp0..\..\ %~dp0automatron\tasks_commit_only "%COMMENT%"
call %AUTOMOTRON_EXE% %~dp0..\..\ %~dp0automatron\tasks_commit_only "%COMMENT%"
if %errorlevel% NEQ 0 (
   echo automatron.exe exited with %errorlevel%
   exit /b %errorlevel%
   )

POPD

exit /b 0



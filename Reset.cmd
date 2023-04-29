@ECHO OFF
PUSHD %~dp0

CLS

ECHO %time% %~nx0 %~1

SET GIT="git.exe"

ECHO %GIT% reset --hard
%GIT% reset --hard
IF 0 NEQ %ERRORLEVEL% (
  ECHO ERRORLEVEL %ERRORLEVEL%
  exit /b -1
)

ECHO %GIT% status
%GIT% status
IF 0 NEQ %ERRORLEVEL% (
  ECHO ERRORLEVEL %ERRORLEVEL%
  exit /b -1
)


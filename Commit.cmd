@ECHO OFF
PUSHD %~dp0

CLS

ECHO %time% %~nx0 %~1

SET GIT="git.exe"

FOR /F "skip=1 tokens=1-6" %%G IN ('WMIC Path Win32_LocalTime Get Day^,Hour^,Minute^,Month^,Second^,Year /Format:table') DO (
   IF "%%~L"=="" goto S_DONE
      SET _yyyy=%%L
      SET _mm=00%%J
      SET _dd=00%%G
      SET _hour=00%%H
      SET _minute=00%%I
      SET _second=00%%K
)
: S_DONE

SET _mm=%_mm:~-2%
SET _dd=%_dd:~-2%
SET _hour=%_hour:~-2%
SET _minute=%_minute:~-2%
SET _second=%_second:~-2%

SET TIMESTAMP=%_yyyy%-%_mm%-%_dd%T%_hour%:%_minute%:%_second%

FOR /F "usebackq" %%i IN (`hostname`) DO SET HOST_NAME=%%i


ECHO %GIT% pull
%GIT% pull
IF 0 NEQ %ERRORLEVEL% (
  ECHO ERRORLEVEL %ERRORLEVEL%
  exit /b -1
)

ECHO %GIT% add .
%GIT% add .
IF 0 NEQ %ERRORLEVEL% (
  ECHO ERRORLEVEL %ERRORLEVEL%
  exit /b -1
)

SET COMMIT_ARGS="%TIMESTAMP% %HOST_NAME%"
ECHO %GIT% commit -m %COMMIT_ARGS%
%GIT% commit -m %COMMIT_ARGS%
IF 0 NEQ %ERRORLEVEL% (
  ECHO ERRORLEVEL %ERRORLEVEL%
  exit /b -1
  )

ECHO %GIT% push
%GIT% push
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

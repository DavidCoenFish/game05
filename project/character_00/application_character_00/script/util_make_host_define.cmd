::util_make_host_define.cmd <1:host define output path>

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1

IF EXIST %1 (
   POPD
   EXIT /b %errorlevel%
   )

FOR /F "usebackq" %%i IN (`hostname`) DO SET HOST_NAME=%%i

SET HOST_NAME=%HOST_NAME: =_%
SET HOST_NAME=%HOST_NAME:-=_%

SETLOCAL ENABLEDELAYEDEXPANSION

CALL :UCase HOST_NAME UPPER_HOST_NAME
ECHO.%UPPER_HOST_NAME%

ECHO Write new host define file
> "%~1.temp" echo.#define HOST_%UPPER_HOST_NAME% 1

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~1" "%~1.temp"


ENDLOCAL
POPD
exit /b %errorlevel%

:UCase
:: Converts to upper/lower case variable contents
:: Syntax: CALL :UCase _VAR1 _VAR2
:: Syntax: CALL :LCase _VAR1 _VAR2
:: _VAR1 = Variable NAME whose VALUE is to be converted to upper/lower case
:: _VAR2 = NAME of variable to hold the converted value
:: Note: Use variable NAMES in the CALL, not values (pass "by reference")

SET _UCase=A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
SET _Lib_UCase_Tmp=!%1!
FOR %%Z IN (%_UCase%) DO SET _Lib_UCase_Tmp=!_Lib_UCase_Tmp:%%Z=%%Z!
SET %2=%_Lib_UCase_Tmp%
GOTO:EOF

POPD
exit /b %errorlevel%

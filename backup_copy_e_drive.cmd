::BackupCopyEDrive.cmd

@ECHO OFF
PUSHD %~dp0

cls

echo %~nx0 %~1 %~2

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

SET TIMESTAMP=%_yyyy%-%_mm%-%_dd%T%_hour%%_minute%%_second%
SET DESTINATION=E:\backup\%TIMESTAMP%_game05

::/L list and dont copy
::/NJH no job header
robocopy . %DESTINATION% /S /XD .dist .git .vs .vscode build dist /NS /NC /NFL /NDL /NP 



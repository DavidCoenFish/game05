::util_make_resource <1:version file path>

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1

SET /p VERSION=<"%~1"
SET VERSION=%VERSION:"=%
SET VERSION_COMMA=%VERSION:.=,%

call util_search_replace.cmd "..\resources\application_lq_00.rc.template" "..\resources\application_lq_00.rc.temp" "___FILE_VERSION_COMMA___" "%VERSION_COMMA%"
call util_search_replace.cmd "..\resources\application_lq_00.rc.temp" "..\resources\application_lq_00.rc.temp" "___PRODUCT_VERSION_COMMA___" "%VERSION_COMMA%"
call util_search_replace.cmd "..\resources\application_lq_00.rc.temp" "..\resources\application_lq_00.rc.temp" "___FILE_VERSION___" "%VERSION%"
call util_search_replace.cmd "..\resources\application_lq_00.rc.temp" "..\resources\application_lq_00.rc.temp" "___PRODUCT_VERSION___" "%VERSION%"

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "..\resources\application_lq_00.rc" "..\resources\application_lq_00.rc.temp"

POPD
exit /b %ERRORLEVEL%
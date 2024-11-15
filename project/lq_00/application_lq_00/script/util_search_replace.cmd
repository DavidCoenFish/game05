::util_search_replace.cmd <1:input file path> <2:output file path> <3:find value> <4:replace value>

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2 %~3 %~4

::echo powershell -Command "(gc '%~1') -replace '%~3', '%~4' | Out-File -encoding ASCII '%~2'"
call powershell -Command "(gc '%~1') -replace '%~3', '%~4' | Out-File -encoding DEFAULT '%~2'"

POPD
exit /b %ERRORLEVEL%
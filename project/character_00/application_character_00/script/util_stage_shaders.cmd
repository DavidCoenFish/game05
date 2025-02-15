::stage_shaders.cmd <1:input dir> <2:output dir>

::C:\development\game05\build\character_00\static_character_00\x64\Debug\output
::C:\development\game05\build\character_00\application_character_00\x64\Debug\output

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2

::XCOPY "%~1Shader" "%~2Shader" /S /I /Y
ROBOCOPY "%~1Shader" "%~2Shader" /MIR /NJH /NJS /NP

POPD
exit /b %ERRORLEVEL%
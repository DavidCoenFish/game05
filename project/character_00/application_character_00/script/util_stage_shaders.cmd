::stage_shaders.cmd <1:input dir> <2:output dir>

::C:\development\game05\build\character_00\static_character_00\x64\Debug\output
::C:\development\game05\build\character_00\application_character_00\x64\Debug\output

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2

::XCOPY "%~1Shader" "%~2Shader" /S /I /Y
ROBOCOPY "%~1shader/static_character_00" "%~2shader/static_character_00" /MIR /NJH /NJS /NP

if %errorlevel% GTR 8 (
   echo ROBOCOPY "%~1shader/static_character_00" "%~2shader/static_character_00" /MIR /NJH /NJS /NP
   echo errorlevel: %errorlevel%
   exit /b 1
   )


POPD
exit /b 0
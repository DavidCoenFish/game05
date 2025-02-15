::prebuild.cmd <1:static character build output dir> <2:output dir>

::@ECHO OFF
PUSHD %~dp0

echo %time% %~nx0 %~1 %~2

SET DATA_DIR=..\..\..\..\engine\version\output\

SET BUILD_VERSION_FILE_PATH=%DATA_DIR%build_version.txt
SET BUILD_VERSION_FILE_PATH_COMMA=%DATA_DIR%build_version_comma.txt
SET BUILD_VERSION_FILE_PATH_0=%DATA_DIR%build_version_major.txt
SET BUILD_VERSION_FILE_PATH_1=%DATA_DIR%build_version_minor.txt
SET BUILD_VERSION_FILE_PATH_2=%DATA_DIR%build_version_patch.txt
SET BUILD_VERSION_FILE_PATH_3=%DATA_DIR%build_version_store.txt
SET HOST_DEFINE_FILE_PATH=..\source\host_define.h

::echo call util_make_host_define.cmd "%HOST_DEFINE_FILE_PATH%"
call util_make_host_define.cmd "%HOST_DEFINE_FILE_PATH%"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

::echo call util_make_resource.cmd "%BUILD_VERSION_FILE_PATH%"
call util_make_resource.cmd "%BUILD_VERSION_FILE_PATH%"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

::echo call util_stage_shaders.cmd "%~1" "%~2"
call util_stage_shaders.cmd "%~1" "%~2"
if %errorlevel% GEQ 8 (
   exit /b %ERRORLEVEL%
   )


echo %time% %~nx0 finished

POPD

exit /b %ERRORLEVEL%

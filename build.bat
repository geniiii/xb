@echo off

call cached_vc_vars.bat

setlocal EnableDelayedExpansion

set compiler=cl.exe
set engine_name=xtal
set application_name=xb

set build=debug
set debug=n
set hot_reload=n
set optimizations=n
if "%build%" equ "release"           set optimizations=y
if "%build%" equ "release_hr"        set optimizations=y
if "%build%" equ "release_debug"     set optimizations=y
if "%build%" equ "release_debuginfo" set optimizations=y

if "%build%" equ "debug"             set debug=y
if "%build%" equ "release_debug"     set debug=y

if "%build%" equ "debug"             set debug_info=y
if "%build%" equ "release_debug"     set debug_info=y
if "%build%" equ "release_debuginfo" set debug_info=y

if "%build%" equ "debug"             set hot_reload=y
if "%build%" equ "release_debug"     set hot_reload=y
if "%build%" equ "release_hr"        set hot_reload=y

set build_options=/DXTAL_BUILD_WIN32=1 /D_CRT_SECURE_NO_WARNINGS
if "%hot_reload%" equ "y" (
    set build_options=!build_options! /DXTAL_HOT_RELOAD=1
)

set common_compile_flags=/nologo /utf-8 /W4 /FC /TC /permissive- /diagnostics:caret /I ../source/ /I ../source/xtal/ /external:W0 /external:I ../source/xtal/ext/
set common_link_flags=/dynamicbase:no /incremental:no

set platform_compile_flags=/I../source/xtal
set platform_link_flags=opengl32.lib Gdi32.lib User32.lib ShLwApi.lib shell32.lib /manifest:embed /manifestinput:../data/win32/xtal-win32.manifest
set app_link_flags=

if "%compiler%"=="cl.exe" (
	set common_compile_flags=!common_compile_flags! /FS /std:c17
) else (
	set common_compile_flags=!common_compile_flags! -Xclang -std=c17 -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter -fdiagnostics-absolute-paths -fuse-ld=lld-link
)

if "%debug%" equ "y" (
	set build_options=!build_options! /DXTAL_DEBUG=1
	set platform_link_flags=!platform_link_flags! -subsystem:CONSOLE -entry:WinMainCRTStartup
	set app_link_flags=!app_link_flags! /pdb:%application_name%-%random%.pdb
    set common_compile_flags=!common_compile_flags! /MT
    REM set common_compile_flags=!common_compile_flags! /fsanitize=address
) else (
	set common_compile_flags=!common_compile_flags! /MT
    set common_link_flags=!common_link_flags! /debug:none
    set platform_link_flags=!platform_link_flags! -subsystem:WINDOWS
)

if "%debug_info%" equ "y" (
    set common_link_flags=!common_link_flags! /debug:full
    set common_compile_flags=!common_compile_flags! /Zi /Zo
    if "%compiler%"=="cl.exe" (
    	set common_compile_flags=!common_compile_flags! /Zf
    )
)

echo ~~~ Generating resource file ~~~
if "%optimizations%" equ "y" (
	start /b "" rc /nologo data\win32\xtal_release.rc

	set common_compile_flags=!common_compile_flags! /O2 /Oi /GS- /Gs9999999
	set common_link_flags=!common_link_flags! /opt:ref /opt:icf
	set platform_link_flags=!platform_link_flags! ../data/win32/xtal_release.res
) else (
	start /b "" rc /nologo data\win32\xtal_debug.rc

	set build_options=!build_options! /DXTAL_SLOW=1

	set common_compile_flags=!common_compile_flags! /Od /FC
	if "%debug%" equ "y" (
		set common_compile_flags=!common_compile_flags! /RTC1
	)
	set platform_link_flags=!platform_link_flags! ../data/win32/xtal_debug.res
)

REM Build shader generator if not already built
if not exist build\xtal-gen-shaders.exe (
	setlocal
	call "build_metadesk.bat"
	endlocal
)

echo ~~~ Running shader generator ~~~
pushd source
pushd xtal
if not exist generated mkdir generated
pushd generated
set files=
for %%i in (..\..\..\metadesk\data\*.mdesk) do (
	if "%%~xi"==".mdesk" call set "files=%%files%% %%i" )
)
..\..\..\build\xtal-gen-shaders.exe %files% --shader_output=shaders.c
popd
popd
popd

echo.
echo ~~~ Building %application_name% ~~~
if not exist build mkdir build
pushd build

del %application_name%-*.pdb >nul 2>&1

qprocess "%engine_name%-win32.exe" >nul 2>&1
if %ERRORLEVEL% equ 0 (
	echo Game already running, skipping engine build
	goto build_game
)
echo ~~~ Building platform layer ~~~
ctime -begin %engine_name%-win32.ctm
%compiler% !build_options! !common_compile_flags! !platform_compile_flags! ../source/xtal/win32/build.c /Fe%engine_name%-win32.exe /link !common_link_flags! !platform_link_flags!
ctime -end %engine_name%-win32.ctm !ERRORLEVEL!

:build_game
echo ~~~ Building game ~~~
ctime -begin %application_name%.ctm
%compiler% !build_options! !common_compile_flags! !app_compile_flags! ../source/game/build.c /LD /link !common_link_flags! !app_link_flags! /out:%application_name%.dll
ctime -end %application_name%.ctm !ERRORLEVEL!

popd

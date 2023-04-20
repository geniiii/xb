@echo off

set CACHED_ENV=build\cached_vcvars.env
set PRE_ENV="%TEMP%\pre.env"
set POST_ENV="%TEMP%\post.env"

:: Look for a cached env
if exist %CACHED_ENV% (
    echo Applying previously cached env in '%CACHED_ENV%'..

    FOR /F "tokens=*" %%i in (%CACHED_ENV%) do set %%i
    goto :END
)

echo Couldn't find cached env. Trying to locate vcvars batch file..
echo.

if not defined VCDIR (
    set VCDIR=""
)

if not exist %VCDIR% (
    set VCDIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community"
)

if not exist %VCDIR% (
    set VCDIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools"
)

if not exist %VCDIR% (
    set VCDIR="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community"
)

if not exist %VCDIR% (
    set VCDIR="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools"
)

if not exist %VCDIR% (
    if exist "%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
        for /F "tokens=* USEBACKQ" %%F in (`"%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do set VCDIR="%%F"
    )
)

if not exist %VCDIR% (
	echo Couldn't find Visual Studio folder!
	goto :END
)

echo %PRE_ENV%

:: Store env before running vars command
set | sort > %PRE_ENV%
REM sort %PRE_ENV%

call %VCDIR%\VC\Auxiliary\Build\vcvars64.bat

:: Store new env and compute diff using 'comm' (included in git-for-windows)
set | sort > %POST_ENV%
REM sort %POST_ENV%

comm -13 %PRE_ENV% %POST_ENV% > %CACHED_ENV%

goto :END


:END

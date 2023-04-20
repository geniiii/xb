@echo off

set application_name=xtal-gen-shaders
set compiler=cl.exe

set build_options=/D_CRT_SECURE_NO_WARNINGS

if "%compiler%"=="clang-cl.exe" (set external_flag= /imsvc) else (set external_flag= /external:I)
set compile_flags= -nologo /W4 /FS /FC /TC /O2 /Oi /MD /permissive- %external_flag% ..\metadesk\ext\
if "%compiler%"=="cl.exe" (
	set compile_flags= %compile_flags% /std:c17 /experimental:external /external:W0
) else (
	set compile_flags= %compile_flags% -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter -fdiagnostics-absolute-paths -fuse-ld=lld-link
)

set link_flags=/subsystem:CONSOLE -opt:ref -opt:icf -dynamicbase:no -incremental:no /debug:none

echo.
echo ~~~ Building Metadesk generator ~~~
if not exist build mkdir build
pushd build
ctime -begin %application_name%.ctm
%compiler% %build_options% %compile_flags% ..\metadesk\generate.c /link %link_flags% /out:%application_name%.exe
ctime -end %application_name%.ctm !ERRORLEVEL!
popd


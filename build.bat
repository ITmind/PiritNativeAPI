@echo off
echo **************************************************
echo **  START
echo **************************************************

mkdir bin
set BINDIR=%CD%\bin
xcopy INFO.xml %BINDIR%\ /y
xcopy MANIFEST.xml %BINDIR%\ /y

SET ARCH=x64
call :setenv
call :generate
call :build

SET ARCH=x86
call :setenv
call :generate
call :build

exit /B 0

:setenv
set __VCVARSALL_TARGET_ARCH=%ARCH%
set __VCVARSALL_HOST_ARCH=x64
set "__VCVARSALL_VSDEVCMD_ARGS=-arch=%__VCVARSALL_TARGET_ARCH% -host_arch=%__VCVARSALL_HOST_ARCH%"
echo %__VCVARSALL_VSDEVCMD_ARGS%
call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\vsdevcmd.bat" %__VCVARSALL_VSDEVCMD_ARGS%

if "%ERRORLEVEL%"=="0" (
    if "%__VCVARSALL_HOST_ARCH%" NEQ "%__VCVARSALL_TARGET_ARCH%" (
        echo [%~nx0] Environment initialized for: '%__VCVARSALL_HOST_ARCH%_%__VCVARSALL_TARGET_ARCH%'
    ) else (
        echo [%~nx0] Environment initialized for: '%__VCVARSALL_TARGET_ARCH%'
    )
)

exit /B 0

:generate
echo **************************************************
echo ** GENERATE %ARCH%
echo **************************************************
set CMAKE_INSTALL_PREFIX="%CD%\out\install\%ARCH%-Debug"
set CMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.24.28314/bin/HostX64/%ARCH%/cl.exe"
set CMAKE_MAKE_PROGRAM="C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2019\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe"
set BUILDPATCH= "%CD%\out\%ARCH%-Debug"
cmake.exe  -G "Ninja" -DCMAKE_INSTALL_PREFIX:PATH=%CMAKE_INSTALL_PREFIX% -DCMAKE_CXX_COMPILER:FILEPATH=%CMAKE_CXX_COMPILER% -DCMAKE_C_COMPILER:FILEPATH=%CMAKE_CXX_COMPILER%  -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_MAKE_PROGRAM=%CMAKE_MAKE_PROGRAM% -S "%CD%" -B %BUILDPATCH%
exit /B 0

:build
echo **************************************************
echo ** BUILD %ARCH%
echo **************************************************
pushd %BUILDPATCH%
cmake --build .
xcopy PiritNativeAPI.dll %BINDIR%\PiritNativeAPI%ARCH%.dll /i /y 
popd
exit /B 0
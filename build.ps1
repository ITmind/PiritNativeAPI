Write-Output "**************************************************"
Write-Output "**  START"
Write-Output "**************************************************"
$BINDIR = $PSScriptRoot + "\bin"

Function Add-Bin {
    
    mkdir "bin" -Force
    Copy-Item "INFO.xml" "$BINDIR"
    Copy-Item "MANIFEST.xml" "$BINDIR"
}

Function Set-Env($arch) {
    $__VCVARSALL_TARGET_ARCH = $arch
    $__VCVARSALL_HOST_ARCH = "x64"
    $__VCVARSALL_VSDEVCMD_ARGS = "-arch=$__VCVARSALL_TARGET_ARCH -host_arch=$__VCVARSALL_HOST_ARCH"
    Write-Output $__VCVARSALL_VSDEVCMD_ARGS

    $installationPath = "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community"
    $setenvcommand = "$installationPath\\VC\Auxiliary\Build\vcvars64.bat"
    
    & "${env:COMSPEC}" /s /c "`"$setenvcommand`" && set" | foreach-object {
        if ($_.Contains("=")) {
            $name, $value = $_ -split '=', 2
            set-content env:\"$name" $value
        } 
    }
    

    return 1
}

Function Generate($arch) {
    Write-Output "**************************************************"
    Write-Output "** GENERATE $arch"
    Write-Output "**************************************************"
    $CMAKE_INSTALL_PREFIX = "$PSScriptRoot\out\install\$arch-Debug"
    $BUILDPATCH = "$PSScriptRoot\out\$arch-Debug"
    & cmake.exe  -G "Ninja" -DCMAKE_INSTALL_PREFIX:PATH=$CMAKE_INSTALL_PREFIX -DCMAKE_CXX_COMPILER:FILEPATH="cl.exe" -DCMAKE_C_COMPILER:FILEPATH="cl.exe" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_MAKE_PROGRAM="ninja.exe" -S "$PSScriptRoot" -B $BUILDPATCH
}

function Build($arch) {

    Write-Output "**************************************************"
    Write-Output "** BUILD $arch"
    Write-Output "**************************************************"
    $BUILDPATCH = "$PSScriptRoot\out\$arch-Debug"
    & cmake --build $BUILDPATCH
    Copy-Item "$BUILDPATCH\PiritNativeAPI.dll" "$BINDIR\PiritNativeAPI$arch.dll"
    #xcopy PiritNativeAPI.dll %BINDIR%\PiritNativeAPI%ARCH%.dll /i /y   
}

Add-Bin
Set-Env("x64")
Generate("x64")
Build("x64")
Set-Env("x86")
Generate("x86")
Build("x86")
	
$compress = @{
Path= "$BINDIR\INFO.xml", "$BINDIR\MANIFEST.xml", "$BINDIR\PiritNativeAPIx86.dll", "$BINDIR\PiritNativeAPIx64.dll"
CompressionLevel = "Fastest"
DestinationPath = "$BINDIR\PiritNativeAPI.zip"
}
Compress-Archive @compress -Force
# Builds RetooledNative.dll on Windows, checks what it links to, and copies it into Plugins/x86_64. Needs
# CMake, Ninja, LLVM's clang-cl, the Windows SDK and Python. CI builds the Linux and macOS binaries.
#
#   build.ps1                        uses the PluginAPI of the editor that unity-links.json names, from Unity Hub
#   build.ps1 -PluginApi <folder>    uses another editor's Editor/Data/PluginAPI
#
# The build also writes Native~/compile_commands.json, which editors read for code completion.
#
# A Unity editor that has loaded the plugin keeps the DLL open, so close it before building.
param(
    [string]$PluginApi
)

$ErrorActionPreference = 'Stop'
$native = $PSScriptRoot
$build = Join-Path $native 'build/Release'
$plugins = Join-Path $native '../Plugins/x86_64'
$baseline = Join-Path $native 'unity-links.json'

if (-not $PluginApi) {
    # unity_version reads like "6000.7.0b10 (4f3c1c5c4346)"
    $version = ((Get-Content $baseline -Raw | ConvertFrom-Json).unity_version -split ' ')[0]
    $PluginApi = "C:\Program Files\Unity\Hub\Editor\$version\Editor\Data\PluginAPI"
}
if (-not (Test-Path (Join-Path $PluginApi 'IUnityGraphics.h'))) {
    throw "No Unity plug-in headers in $PluginApi. Pass -PluginApi <editor>/Editor/Data/PluginAPI."
}

# CMake also needs LLVM's linker and manifest tool, so the whole folder goes on PATH.
if (-not (Get-Command clang-cl -ErrorAction SilentlyContinue)) {
    $llvm = 'C:\Program Files\LLVM\bin'
    if (-not (Test-Path (Join-Path $llvm 'clang-cl.exe'))) { throw "clang-cl was not found on PATH or in $llvm" }
    $env:PATH = "$llvm;$env:PATH"
}

cmake -S $native -B $build -G Ninja `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_CXX_COMPILER=clang-cl `
    "-DUNITY_PLUGIN_API=$PluginApi"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed' }

cmake --build $build
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }

Copy-Item (Join-Path $build 'compile_commands.json') $native -Force

$dll = Join-Path $build 'RetooledNative.dll'
python (Join-Path $native 'check-links.py') check $dll --baseline $baseline
if ($LASTEXITCODE -ne 0) { throw 'The plugin links to something that Unity''s player does not' }

Copy-Item $dll $plugins -Force
Write-Host "Copied RetooledNative.dll to $(Resolve-Path $plugins)"

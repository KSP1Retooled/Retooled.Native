# Builds RetooledNative.dll on Windows, checks what it links to, and copies it into Plugins/x86_64. Needs CMake,
# Visual Studio's C++ tools and Python. CI builds the Linux and macOS binaries.
#
#   build.ps1                        uses the PluginAPI of the editor that unity-links.json names, from Unity Hub
#   build.ps1 -PluginApi <folder>    uses another editor's Editor/Data/PluginAPI
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

cmake -S $native -B $build "-DUNITY_PLUGIN_API=$PluginApi"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed' }

cmake --build $build --config Release
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }

$dll = Join-Path $build 'Release/RetooledNative.dll'
python (Join-Path $native 'check-links.py') check $dll --baseline $baseline
if ($LASTEXITCODE -ne 0) { throw 'The plugin links to something that Unity''s player does not' }

Copy-Item $dll $plugins -Force
Write-Host "Copied RetooledNative.dll to $(Resolve-Path $plugins)"

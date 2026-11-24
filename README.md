# Retooled Native

Small native helpers for things that Unity's C# API doesn't expose. The package carries one native plugin,
`RetooledNative`, for Windows, Linux and macOS, all x86_64.

## Helpers

### `SystemInfo.HasUnifiedMemory`

Whether the GPU shares system memory instead of having VRAM of its own, as integrated GPUs and Apple's GPUs do. On
such GPUs, `UnityEngine.SystemInfo.graphicsMemorySize` can count most of system memory as VRAM, so a budget that is a
fraction of it is far too large.

```csharp
using NativeSystemInfo = Retooled.Native.SystemInfo;

if (NativeSystemInfo.HasUnifiedMemory == true)
    budget = minimumBudget;
```

The class shares its name with `UnityEngine.SystemInfo`, so a file that also has `using UnityEngine;` needs an alias
like the one above.

The plugin asks Unity's own graphics device:

| Graphics API | How it tells |
| --- | --- |
| Direct3D 12 | The device's `D3D12_FEATURE_ARCHITECTURE` reports UMA. |
| Direct3D 11 | The device's `D3D11_FEATURE_D3D11_OPTIONS2` reports UMA. It needs Direct3D 11.3, from Windows 10. |
| Vulkan | The physical device is an integrated GPU or a CPU. |
| Metal | The device's `hasUnifiedMemory`. |
| OpenGL | It can't, so the answer is null. |

The answer is also null before Unity has created its graphics device, which it has by the time
`RuntimeInitializeLoadType.BeforeSceneLoad` methods run.

## Building

The `Native plug-in` workflow (`.github/workflows/native.yml`) runs whenever a push changes the plugin's source. It
builds the plugin on Windows, Linux and macOS, checks that each build links only to libraries that Unity's player also
links to, and commits the three binaries to `Plugins/x86_64` on the branch that changed.

To build the Windows plugin locally, run `Native~/build.ps1`. It needs CMake, Ninja, LLVM's clang-cl, the Windows SDK,
Python and the Unity editor that `Native~/unity-links.json` names. Pass `-PluginApi <editor>/Editor/Data/PluginAPI` to
use another editor's plugin headers. Close any Unity editor that has loaded the plugin first, since it keeps the DLL
open. The build also writes `Native~/compile_commands.json`, which clangd and other editors read for code completion.

Format the C++ with `clang-format -i Native~/src/*`, which reads `Native~/.clang-format`.

`Native~/unity-links.json` records what Unity's players link to. When the project moves to a new Unity version, record
it again from that editor's players:

```
python Native~/check-links.py baseline --unity-version "<version> (<changeset>)" \
    --windows <editor>/Data/PlaybackEngines/windowsstandalonesupport/Variations/win64_player_nondevelopment_mono/UnityPlayer.dll \
    --linux <editor>/Data/PlaybackEngines/LinuxStandaloneSupport/Variations/linux64_player_nondevelopment_mono/UnityPlayer.so \
    --macos <editor>/Data/PlaybackEngines/MacStandaloneSupport/Variations/macos_x64_player_nondevelopment_mono/UnityPlayer.app/Contents/Frameworks/UnityPlayer.dylib \
    --output Native~/unity-links.json
```

## Releases

Publishing a GitHub release runs the `Publish to UPM` workflow, which packs the package and adds it to the registry at
`https://ksp1retooled.github.io/upm/`. The tag must be `v` followed by the `version` in `package.json`, and it must
come after the native workflow has committed binaries built from the tagged source.

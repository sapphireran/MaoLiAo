# Building the Windows game

MaoLiAo is a Visual Studio 2019 (toolset v142) Win32 project. It links EasyX (`graphics.h`) and WinMM (`mciSendString`). The portable programs under `examples/` do **not** need either of those; this page is only for the original `.sln`.

## Prerequisites

- Windows 10 or 11
- Visual Studio 2019 or 2022 with the **Desktop development with C++** workload
- [EasyX](https://easyx.cn/) installed so `graphics.h` is on the MSVC include path
- The `MaoLiAo/res/` folder next to the executable at runtime (bitmaps + MP3s)

The committed project file targets:

| Setting | Value |
| --- | --- |
| Project | `MaoLiAo/MaoLiAo.vcxproj` |
| Solution | `MaoLiAo.sln` |
| Platform toolset | v142 |
| Windows SDK | 10.0 |
| Configurations | Debug/Release × Win32/x64 |
| Character set | MultiByte in Debug, Unicode in Release |
| Subsystem | Console |

Debug MultiByte vs Release Unicode is an original setting. The source uses narrow `char` strings and `"黑体"` face names. Prefer **Debug | Win32** or **Debug | x64** if a Release build mis-renders menu text.

## Build

1. Open `MaoLiAo.sln`.
2. Set the startup project to `MaoLiAo`.
3. Build (F7). Output lands in the usual Visual Studio `Debug/` or `Release/` tree.
4. Copy or set the working directory so `res\*.bmp` and `res\*.mp3` resolve. `loadimage` and `mciSendString("open res\\...")` use paths relative to the process current directory, not the `.exe` directory.

If the window opens as a black 512×384 frame and then aborts, the working directory is almost always wrong.

## Icon and resources

`MaoLiAo.rc` plus `MaoLiAo.ico` supply the window icon (`IDI_ICON1`). `resource.h` is the App Studio stub. They are not required to understand gameplay.

## Why there is no CMake for the game

EasyX is a Windows GDI wrapper with no official Linux port. Re-hosting the full game would mean replacing every `putimage` / `GetAsyncKeyState` / MCI call. The `examples/` tree instead extracts the pieces that are plain C++: kinematics, AABB, authored tile lists, and the save-file integer.

## Portable examples

From the repository root:

```bash
make -C examples
make -C examples test
```

See [examples/README.md](../examples/README.md).

# Build (Windows)

猫里奥 is a Visual Studio / EasyX desktop app. The headless examples in `examples/` are the part that builds on Linux.

## Requirements

- Windows 10 or later
- Visual Studio 2019+ with **Desktop development with C++**
- [EasyX](https://easyx.cn/) so that `#include <graphics.h>` resolves
- Windows multimedia: `Winmm.lib` is pulled in with `#pragma comment(lib, "Winmm.lib")` in `main.cpp` and `role.cpp`

## Solution

- `MaoLiAo.sln` — one project `MaoLiAo\MaoLiAo.vcxproj`
- Toolset: `v142`
- `WindowsTargetPlatformVersion`: `10.0`
- Configurations: Debug/Release × Win32/x64

**Character set:** Debug Win32 and Debug x64 are `MultiByte`. Release Win32 and Release x64 are `Unicode`. The sources use narrow string literals (`"黑体"`, `"res\\home.bmp"`) and `_itoa_s` into `char` buffers. Prefer **Debug | Win32** (or change Release to MultiByte) so those literals stay 8-bit.

Subsystem is **Console** (`_CONSOLE`). A console window will appear behind the EasyX window; that is how `_kbhit` is fed.

## Source files compiled

```
main.cpp
control.cpp
scene.cpp
role.cpp
inertia.cpp
```

Headers are not compiled separately. `timer.h` contains the `Timer::Sleep` **definition** (static members included). Include it from one translation unit only (`main.cpp` does). Do not include it from a second `.cpp` or you will get multiple-definition errors.

## Include / link notes

EasyX’s install typically adds include and lib directories globally. If `graphics.h` is missing, add the EasyX include directory to the project’s additional include path.

`inertia.cpp` starts with `#pragma once` (harmless on a `.cpp` but unusual).

## Assets

Place `res\` as described in [resources.md](resources.md). `gameRecord.dat` is optional; the title screen can start without it.

## Linux / this cloud checkout

You cannot link EasyX here. Use:

```bash
python3 -m unittest discover -s examples/python/tests -v
make -C examples/cpp
python3 examples/python/cli.py preview --world 1
```

Those tools encode the same constants and map tables as the game and are the regression net for documentation edits.

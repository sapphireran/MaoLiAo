# Build and run

## Game (Windows)

- **Solution:** `MaoLiAo.sln` (Visual Studio 16 / toolset v142).
- **Project:** `MaoLiAo/MaoLiAo.vcxproj`, Win32 C++ console subsystem.
- **Platforms:** Debug/Release × Win32/x64.
- **Character set:** Debug configurations are `MultiByte`; Release is `Unicode`. The UI strings are narrow `"黑体"` / `"Cooper"` literals. Debug|Win32 is the configuration that matches how the file was written.
- **SDK:** `WindowsTargetPlatformVersion` 10.0.
- **Library:** EasyX (`graphics.h`). Install the EasyX matching your VS version so `graphics.h` is on the include path.
- **Link:** `Winmm.lib` via `#pragma comment` for MCI.

Sources compiled: `main.cpp`, `control.cpp`, `role.cpp`, `scene.cpp`, `inertia.cpp`. `timer.h` is header-only with static member definitions in the header — include it from one `.cpp` only (`main.cpp`).

Working directory must see:

```
res\role.bmp
res\ani.bmp
res\map.bmp
res\mapsky.bmp
res\scenery.bmp
res\home.bmp
res\*.mp3
```

and optionally `gameRecord.dat` one level up from `res` (whatever cwd you choose).

If EasyX reports a character-set mismatch, use the Debug MultiByte configuration (commit `a33467f` was a previous charset fix).

## Examples (any OS with a C++17 compiler)

The EasyX game is not built here. From `examples/`:

```bash
make            # demos + test binary
make test       # run examples/tests/test_maoliao
make clean
```

Override the compiler with `CXX=clang++`. Flags default to `-std=c++17 -Wall -Wextra -Werror`.

There is no EasyX, Win32, or audio dependency in `examples/`.

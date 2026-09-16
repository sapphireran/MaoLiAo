# Build

MaoLiAo is a Visual Studio 2019 (toolset v142) Win32 / x64 C++ project. It links the EasyX `graphics.h` that the installer drops into the VC include path, plus `Winmm.lib` (`#pragma comment` in `main.cpp` and `role.cpp`).

There is no CMake for the game itself. The [`examples/`](../examples/README.md) tree has a Makefile for the portable demos only.

## Prerequisites

- Windows 10/11
- Visual Studio 2019 or 2022 with **Desktop development with C++**
- [EasyX](https://easyx.cn/) matching your VS version
- The `res/` bitmap and mp3 set next to the exe working directory (see [assets.md](assets.md))

Linux / this cloud workspace cannot link EasyX or open a Win32 graphics window. Use the portable examples to exercise formulas here; use Visual Studio on a Windows box for the real `.exe`.

## Configurations

From `MaoLiAo.vcxproj`:

| Config | Platform | Character set | Notes |
| --- | --- | --- | --- |
| Debug | Win32 | MultiByte | Best match for `"黑体"` / `"得分:  "` literals |
| Debug | x64 | MultiByte | Same strings, 64-bit |
| Release | Win32 | **Unicode** | `outtextxy` / `drawtext` with narrow literals is wrong |
| Release | x64 | **Unicode** | Same trap |

If you want a Release build, change `CharacterSet` to `MultiByte` or migrate the UI strings to `TCHAR` / `L"…"` consistently. Until then, ship Debug|Win32 or fix the project.

Subsystem is **Console** in every config (`<SubSystem>Console</SubSystem>`). A console window appears behind the EasyX window. That is how the (rarely used) `printf` around `MessageBox` would show.

Windows target SDK: `10.0`. Icon: `MaoLiAo.rc`.

## Working directory

`loadimage("res\\map.bmp")` and `fopen_s(..., "gameRecord.dat")` are cwd-relative.

- F5 from Visual Studio: set **Project → Properties → Debugging → Working Directory** to `$(ProjectDir)` (`MaoLiAo/`).
- Double-clicking an exe in `MaoLiAo/Debug/` will **not** find `res\` unless you copy `res` next to the exe or start from `MaoLiAo/`.

## Include / compile notes

- Precompiled headers are not used.
- `SDLCheck` is on; some CRT calls already use the `_s` variants (`fopen_s`, `_itoa_s`, `strcat_s`, `_tcscpy_s`).
- `define.h` wraps itself in `#ifndef MYDEFINE` but then `#define MYDIFINE` (typo). The include guard does not work. It still compiles because `#pragma once` is also present.
- `inertia.cpp` starts with `#pragma once` (harmless on a .cpp, unused).
- `max` and `random` macros in `define.h` can clash with `<windows.h>` / `std::max`. Keep them, or rename before adding STL headers.

## Build from a Developer Command Prompt

```bat
msbuild MaoLiAo.sln /p:Configuration=Debug /p:Platform=Win32
```

Output: `MaoLiAo\Debug\MaoLiAo.exe` (exact path depends on VS defaults; this project does not override `OutDir`).

## Portable examples

On any machine with `g++` or `clang++`:

```bash
cd examples
make        # build all demos
make test   # run them; expect "PASS" lines
make clean
```

These do not require EasyX, Windows, or `res/`.

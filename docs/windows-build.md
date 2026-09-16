# Windows build (original game)

The playable binary is a Visual Studio 2019 (toolset v142) Win32/x64
console-subsystem executable that links **EasyX** and **WinMM**.

This environment is Linux and cannot produce `MaoLiAo.exe`. Use the
steps below on a Windows machine. The portable suite under `examples/`
is what CI / this repo can compile anywhere.

## Prerequisites

- Visual Studio 2019 or 2022 with **Desktop development with C++**
- [EasyX](https://easyx.cn/) installed into the VC toolset
  (`graphics.h` on the include path, EasyX lib on the lib path)
- The `res/` art + mp3 pack next to the exe (see
  [resources.md](resources.md))

Character set: Debug configs are **MultiByte**; Release configs are
**Unicode**. The UI strings are narrow `"黑体"` / `"返回游戏"` literals.
If a Release build shows garbage or fails to compile `_T` / `drawtext`
overloads, switch Release to MultiByte to match Debug, or widen the
literals. V2.0 was developed as MultiByte.

## Open and build

1. Open `MaoLiAo.sln`.
2. Set **MaoLiAo** as the startup project.
3. Configuration: `Debug | x86` is the historical default.
4. Project → Properties → Debugging → Working Directory = `$(ProjectDir)`.
5. Build (Ctrl+Shift+B).

`MaoLiAo.vcxproj` already lists:

```
scene.cpp  role.cpp  main.cpp  inertia.cpp  control.cpp
scene.h    role.h    timer.h   define.h     inertia.h  control.h
MaoLiAo.rc + MaoLiAo.ico
```

No extra `.lib` entries are in the vcxproj; EasyX's installer usually
injects `EasyXa.lib` / `EasyXw.lib` via the global toolset. If link
fails on `initgraph`, add the EasyX lib directory and
`#pragma comment` as the EasyX docs for your version describe.

WinMM is pulled by

```cpp
#pragma comment(lib, "Winmm.lib")
```

in `main.cpp` and `role.cpp`.

## Run

Working directory must see `res\`. First launch should show the title
bitmap and the five-button stack. If the window is a black 512×384
frame, `home.bmp` did not load — fix cwd or copy `res\`.

## Portable examples on Windows

```bat
cd examples
g++ -std=c++11 -Iinclude -c src/maoliao_core.cpp -o src/maoliao_core.o
g++ -std=c++11 -Iinclude 01_inertia.cpp src/maoliao_core.o -o 01_inertia.exe
01_inertia.exe
```

Or install make (Git Bash / MSYS2) and run `make test` the same as on
Linux.

## Subsystem note

The vcxproj sets `<SubSystem>Console</SubSystem>`. A console host
appears behind the EasyX window. That is useful for the rare
`printf` on a bad save; it is not required for play. Switching to
Windows subsystem would hide the console but also hide those messages.

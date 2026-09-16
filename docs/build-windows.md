# Build on Windows

The playable game is an EasyX Win32 desktop app. This Linux cloud agent cannot link `graphics.h` or play the mp3 aliases. Use a Windows box for the `.exe`; use `examples/` here to replay the numbers.

## Toolchain

| Piece | What the `.vcxproj` asks for |
|---|---|
| IDE | Visual Studio 2019+ (`VisualStudioVersion = 16.0.30611.23`) |
| Toolset | `v142` |
| SDK | `WindowsTargetPlatformVersion = 10.0` |
| Charset | Debug = MultiByte, Release = Unicode (the source is mostly multibyte string literals) |
| Subsystem | Console (`_CONSOLE`) so `_kbhit` / `printf` work next to the EasyX window |
| Extra lib | `Winmm.lib` via `#pragma comment` in `main.cpp` and `role.cpp` |

Install [EasyX](https://easyx.cn/) so `graphics.h` is on the MSVC include path.

## Open and run

1. Open `MaoLiAo.sln` (one project, guid `{98E934D5-3F42-40E7-B895-806BD281FF07}`).
2. Configuration: **Debug | Win32** is the combination that matches the MultiByte debug settings. x64 works if EasyX is installed for that platform.
3. Set **Debugging → Working Directory** to `$(ProjectDir)` (`MaoLiAo/`). Every `loadimage("res\\...")` and `fopen("gameRecord.dat")` is relative to cwd.
4. F5. Title bar should read `猫里奥 V2.0`.

## Resource compile

`MaoLiAo.rc` is `LANG_CHINESE` / `code_page(936)` and embeds `MaoLiAo.ico` as `IDI_ICON1` (101). No bitmap is compiled into the `.res`; they are loaded at runtime from `res\`.

## Portable lab (any OS with g++)

```bash
cd examples
make          # build into examples/bin/
make test     # run every self-check
make clean
```

Requires g++ with `-std=c++17`. No EasyX, no Win32, no audio.

## Why the game itself is not built in this environment

`#include <graphics.h>` and `<conio.h>` plus `GetAsyncKeyState` / `mciSendString` / `HWND` are Windows-only. A MinGW cross build would still need the EasyX static library, which is not in this repo.

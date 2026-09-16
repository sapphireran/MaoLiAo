# Resources

The game never packages `res\` in this git tree. Every `loadimage` / `mciSendString` path is a working-directory relative string. A complete Windows run needs the following files next to the process cwd (usually `MaoLiAo\` or the VS output folder).

## Bitmaps

| Path in code | Loader | How it is used |
| --- | --- | --- |
| `res\mapsky.bmp` | `Scene` ctor, stretched to `512 × 1536` | Four stacked 384 px skies; `yBg = -(world-1)*384` |
| `res\map.bmp` | `Scene` ctor, native size | 32×32 (or 64×32 / 64×64) tiles, row = `id - 1` |
| `res\scenery.bmp` | `Scene` ctor | 96×64 animated grass / water / trees / sign; row = `id - 11` |
| `res\ani.bmp` | `Scene` + `Role` | Coins (rows 8–9), score pops (10–11), food (12), enemies (0–1), bombs (4–6), bullets (2–3) |
| `res\role.bmp` | `Role` + death HUD | Walk frames (0–1), death (column 2), mirrored walk (columns 3–4) |
| `res\home.bmp` | `Control` ctor and interstitials, stretched to `512 × 1920` | Five stacked 384 px menus / title cards |

Transparent blits are the EasyX `SRCAND` + `SRCPAINT` pair: a mask row immediately under (or over) the colour row.

## Music / SFX (MCI)

| Path in code | Alias |
| --- | --- |
| `res\背景音乐.mp3` | `music_bg` |
| `res\胜利.mp3` | `music_win` |
| `res\通关.mp3` | `music_passedAll` |
| `res\游戏结束.mp3` | `music_end` |
| `res\死亡1.mp3` | `music_died` |
| `res\跳.mp3` | `music_jump` |
| `res\金币.mp3` | `music_coin` |
| `res\踩敌人.mp3` | `music_tread` |
| `res\吃到武器.mp3` | `music_getWeapon` |
| `res\子弹.mp3` | `music_bullet` |
| `res\子弹撞墙.mp3` | `music_boom` |
| `res\子弹打到敌人.mp3` | `music_boom2` |

`#pragma comment(lib, "Winmm.lib")` is in both `main.cpp` and `role.cpp`. Aliases are opened with `mciSendString("open ... alias ...")` and never error-checked.

## Other files

| Path | Role |
| --- | --- |
| `MaoLiAo/MaoLiAo.ico` + `MaoLiAo.rc` | Window icon (`IDI_ICON1`) |
| `MaoLiAo/gameRecord.dat` | Single integer save (`world`). Sample in tree is `1`. |
| `MaoLiAo.sln` / `MaoLiAo.vcxproj` | VS 2019 (v142), Win32 + x64, Debug + Release |

Debug|Win32 and Debug|x64 use **MultiByte** (`CharacterSet`). Release configurations are **Unicode**. The string literals in `control.cpp` (`"返回游戏"`, `"Gill Sans"`, …) match the MultiByte / EasyX `outtextxy(const char*)` overloads used by the original project. Prefer the Debug charset when you rebuild.

## What this checkout cannot run

This cloud workspace is Linux. There is no `graphics.h`, no `windows.h` multimedia timer, and no `res\` folder. That is why the examples are split:

- `examples/physics` — `g++`, no Win32
- `examples/toolkit` — Python 3 stdlib
- `examples/snippets` — documentation C++ (the hit-test file also compiles)

Do not add binary art to git unless you are replacing the original assets on a Windows machine you control.

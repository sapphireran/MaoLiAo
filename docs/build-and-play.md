# Build and play (Windows game)

猫里奥 is a Visual Studio + EasyX desktop program. It uses GDI `putimage`,
`GetAsyncKeyState`, MCI for MP3 playback, and a multimedia-timer `Sleep`.
Those APIs are Windows-only. Linux and macOS machines can still compile the
portable examples under `examples/`.

## Prerequisites

- Windows 10 or 11
- Visual Studio 2019 or newer, with the **Desktop development with C++** workload
- [EasyX](https://easyx.cn/) installed into that Visual Studio
- A working-directory that can see `MaoLiAo/res/` (bitmaps + MP3s)

The solution file `MaoLiAo.sln` targets VS 16 (`VisualStudioVersion = 16.0.30611.23`)
and offers Debug/Release × x86/x64.

## Open and run

1. Open `MaoLiAo.sln`.
2. Set **MaoLiAo** as the startup project.
3. In Project Properties → Debugging → Working Directory, use `$(ProjectDir)`
   so `res\mapsky.bmp` and the MP3 aliases resolve.
4. Build (F7) and run (F5).

The window title is set in `main.cpp`:

```cpp
SetWindowText(hwnd, "猫里奥 V2.0");
```

Resolution is `XSIZE × YSIZE` = **512 × 384**.

## Controls

| Action | Keys |
| --- | --- |
| Walk left / right | `A` / `D` |
| Jump | `W` or `K` |
| Crouch (no gameplay effect) | `S` |
| Shoot | `J` after eating the flower |
| Pause menu | `Esc` |

World 3 ignores the grounded-jump rule: any `CMD_UP` while the world index is 3
applies a fresh jump velocity. That is the Flappy-style flap.

## Title menu

`Control::gameStart` draws five buttons on `res\home.bmp`:

1. **开始 / Start** — leave the menu and enter world `world` (default 1).
2. **介绍 / Introduction** — three-world blurb, credits “PWB”.
3. **指导 / Directions** — the control list above.
4. **退出 / Exit** — `exit(0)`.
5. **读档 / Read Load** — read `gameRecord.dat` as a single integer world index.

Hovering a button swaps the Chinese label for a short English one and fills the
rectangle green (title) or blue (pause).

## Pause menu

`Esc` opens `Control::pauseClick`:

| Button | Effect |
| --- | --- |
| 返回游戏 / Return | `VIR_RETURN`, resume the tick |
| 重新开始 / start again | `VIR_RESTART`, rebuild `Role` and `Scene` for the current world |
| 退出游戏 / The menu | `VIR_HOME`, reset lives and world, return to the title |
| 进行存档 / Write Data | write `world` to `gameRecord.dat`, then resume |

The save file is a one-integer text file. Load rejects values outside `1..3`.

## Music aliases

`main.cpp` and `Role::Role` open these MCI aliases from `res\`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | 背景音乐.mp3 | looping during play |
| `music_win` | 胜利.mp3 | after a flag |
| `music_passedAll` | 通关.mp3 | after world 3 |
| `music_end` | 游戏结束.mp3 | lives reach 0 |
| `music_died` | 死亡1.mp3 | death |
| `music_jump` | 跳.mp3 | jump |
| `music_coin` | 金币.mp3 | coin |
| `music_tread` | 踩敌人.mp3 | stomp |
| `music_getWeapon` | 吃到武器.mp3 | flower |
| `music_bullet` | 子弹.mp3 | shot |
| `music_boom` | 子弹撞墙.mp3 | shot hits a tile |
| `music_boom2` | 子弹打到敌人.mp3 | shot hits an enemy |

Close everything with `mciSendString("close all", ...)` when `main` exits.
The current loop never leaves `while (true)`, so that line is only reached if
you later add a break.

## Linux / macOS

Do not expect `graphics.h` to compile here. Use:

```bash
cd examples
make test
```

Those programs share the same constants and formulas but print text instead of
opening a GDI window.

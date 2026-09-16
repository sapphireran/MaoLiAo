# Resources

The game loads from `MaoLiAo/res/` using Windows paths (`res\\name.ext`). Those binaries are **not** required to compile the portable examples. They **are** required for a full EasyX run.

## Bitmaps (`loadimage`)

| File | Loaded in | How it is used |
| --- | --- | --- |
| `res\home.bmp` | `Control` | Title, pause overlays, die / win slices. Loaded as `512 × 1920` (five stacked 384-high bands). |
| `res\mapsky.bmp` | `Scene` | Sky, `512 × 1536` (four worlds of height; y offset `-(world-1)*384`). |
| `res\map.bmp` | `Scene` | 32-high tile rows, id `1` at y=0. Ids 7–8 also use 64-wide blit. |
| `res\scenery.bmp` | `Scene` | 3×2 tile decorations (grass, sign, water, tree) with a mask strip. |
| `res\ani.bmp` | `Scene`, `Role` | Coins, food, enemies, bombs, bullets — row offsets in `show` / `Scene::show`. |
| `res\role.bmp` | `Role`, `Control::showDied` | Hero walk / death frames, 32×32 cells. |

Window icon: `MaoLiAo/MaoLiAo.ico` via `MaoLiAo.rc` (`IDI_ICON1`).

## Music (`mciSendString` aliases)

Opened in `main`:

| File | Alias | When |
| --- | --- | --- |
| `res\背景音乐.mp3` | `music_bg` | loop in play; `from 0` on restart |
| `res\胜利.mp3` | `music_win` | any clear |
| `res\通关.mp3` | `music_passedAll` | after world 3 win sting |
| `res\游戏结束.mp3` | `music_end` | `life == 0` |

Opened in `Role` constructor:

| File | Alias | When |
| --- | --- | --- |
| `res\死亡1.mp3` | `music_died` | hero death |
| `res\跳.mp3` | `music_jump` | jump |
| `res\金币.mp3` | `music_coin` | coin |
| `res\踩敌人.mp3` | `music_tread` | stomp |
| `res\吃到武器.mp3` | `music_getWeapon` | food |
| `res\子弹.mp3` | `music_bullet` | fire |
| `res\子弹撞墙.mp3` | `music_boom` | bullet vs tile / max range |
| `res\子弹打到敌人.mp3` | `music_boom2` | bullet vs enemy |

`main` ends with `close all` (unreachable today because the loop is `while (true)` without a break).

## Working directory

Paths are relative to the process cwd, not the exe directory. Visual Studio: Project → Properties → Debugging → Working Directory = `$(ProjectDir)`.

`gameRecord.dat` is written next to that cwd (see [controls-and-ui.md](controls-and-ui.md)).

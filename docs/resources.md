# Resources

The Windows build expects a `res\` directory beside the executable. Bitmaps are loaded with EasyX `loadimage`; audio uses MCI (`Winmm.lib`).

This git checkout does **not** currently list `res\` (the assets lived next to the project on the original machine). Paths below are those **hard-coded in the sources**.

## Bitmaps

| Path | Loaded by | Role |
| --- | --- | --- |
| `res\home.bmp` | `Control` | Title / interstitials. Loaded as `XSIZE × 5*YSIZE` (512×1920). Vertical slices: y=0 title, `−YSIZE` game over, `−2*YSIZE` next-level and death backdrop, `−3*YSIZE` all-clear |
| `res\mapsky.bmp` | `Scene` | Parallax sky, `512 × 4*YSIZE`, one strip per world |
| `res\map.bmp` | `Scene` | Tile ids 1–10 |
| `res\scenery.bmp` | `Scene` | Animated grass / sign / water / tree (ids 11–14) |
| `res\ani.bmp` | `Scene` + `Role` | Coins, food, enemies, bullets, bombs |
| `res\role.bmp` | `Role`, `Control::showDied` | Hero walk / death frames |

Window icon: `MaoLiAo.ico` via `MaoLiAo.rc` (`IDI_ICON1`).

## Audio

| Path | Alias | When |
| --- | --- | --- |
| `res\背景音乐.mp3` | `music_bg` | Loop during play |
| `res\胜利.mp3` | `music_win` | Stage clear |
| `res\通关.mp3` | `music_passedAll` | After world 3 |
| `res\游戏结束.mp3` | `music_end` | Lives exhausted |
| `res\死亡1.mp3` | `music_died` | `died = true` |
| `res\跳.mp3` | `music_jump` | Jump |
| `res\金币.mp3` | `music_coin` | Coin |
| `res\踩敌人.mp3` | `music_tread` | Stomp |
| `res\吃到武器.mp3` | `music_getWeapon` | Mushroom |
| `res\子弹.mp3` | `music_bullet` | Shot |
| `res\子弹撞墙.mp3` | `music_boom` | Bullet vs wall / range |
| `res\子弹打到敌人.mp3` | `music_boom2` | Bullet vs enemy |

`Role` constructors open the SFX aliases again every life. `close all` is after the infinite loop (never reached).

## Sprite math (for tools)

All of the blit rectangles used in `scene.cpp` / `role.cpp` are tabulated in [tile-catalog.md](tile-catalog.md). The Python previewer does not need the bmp files; it draws from the `Map` tables only.

## Working directory

`loadimage("res\\role.bmp")` and `fopen_s(..., "gameRecord.dat")` are relative. Run the exe with cwd = `MaoLiAo\` (Visual Studio default for this project) or copy `res\` next to the exe.

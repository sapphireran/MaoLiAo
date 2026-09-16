# Resources

All runtime assets sit in `MaoLiAo/res/`. The `.exe` loads them with
**backslash** paths (`"res\\map.bmp"`), so the process working
directory must be `MaoLiAo/` (Visual Studio's default for this project).

## Window and grid

| Constant | Value |
| --- | --- |
| `XSIZE` × `YSIZE` | 512 × 384 |
| `WIDTH` × `HEIGHT` | 32 × 32 |
| Visible columns × rows | 16 × 12 |

`mapsky.bmp` is loaded as `512 × 4*384` so three world bands plus slack
fit in one sheet. `home.bmp` is loaded as `512 × 5*384` for title,
game-over, level-clear, and credits strips stacked vertically.

## Bitmaps

| File | Consumers | Layout (as used in code) |
| --- | --- | --- |
| `role.bmp` | `Role::show`, `showDied` | Walk frames in a 32-px grid; death at `(64, 0)` / mask `(64, 32)` |
| `ani.bmp` | enemies, coins, score, food, bullets, bombs | Row-major 32-px (food and bombs use wider cells) |
| `map.bmp` | terrain id 1–10 | One 32-px row per id; id 7–10 blit 2× or 2×2 cells |
| `scenery.bmp` | id 11–14 | 96×64 color+mask pairs, 3-frame sway / flow |
| `mapsky.bmp` | `Scene::show` | Horizontal loop; `yBg = -(world-1)*384` |
| `home.bmp` | title + interstitials | Vertical strip album |

Transparency is always two blits: `SRCAND` (mask) then `SRCPAINT`
(color). Enemy / coin / bomb / bullet frames share `ani.bmp`.

### `ani.bmp` rows referenced in code

| Object | Color row | Mask row | Frame count |
| --- | --- | --- | --- |
| Enemy walk | `0 * 32` | `1 * 32` | 2 |
| Bullet | `2 * 32` | `3 * 32` | 2 |
| Bomb | `4 * 32` (2×2) | `6 * 32` | 4 |
| Coin | `8 * 32` | `9 * 32` | 4 |
| Score pop | `10 * 32` | `11 * 32` | 4 |
| Food / weapon | `12 * 32`, cell ≈ 52×26 | mask under that | 2 |

Hero walk-cycle: `hero_iframe` is 1 or 2 from
`(worldX / STEP) % 4`. Facing left uses columns starting at `3 * 32`.

## Audio (MCI aliases)

Opened with `mciSendString("open res\\….mp3 alias …")`.

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `背景音乐.mp3` | Loop during play |
| `music_win` | `胜利.mp3` | Stage clear |
| `music_passedAll` | `通关.mp3` | After world 3, before credits |
| `music_end` | `游戏结束.mp3` | Game over |
| `music_died` | `死亡1.mp3` | Hero death |
| `music_jump` | `跳.mp3` | Jump |
| `music_coin` | `金币.mp3` | Coin |
| `music_tread` | `踩敌人.mp3` | Stomp |
| `music_getWeapon` | `吃到武器.mp3` | Food pickup |
| `music_bullet` | `子弹.mp3` | Shot |
| `music_boom` | `子弹撞墙.mp3` | Bullet vs tile / max range |
| `music_boom2` | `子弹打到敌人.mp3` | Bullet vs enemy |

`main` closes everything with `close all` after the (infinite) loop —
that line is currently unreachable.

## Other files

| File | Role |
| --- | --- |
| `MaoLiAo.ico` / `MaoLiAo.rc` / `resource.h` | Window icon `IDI_ICON1` |
| `gameRecord.dat` | Single integer save (`"%d"`) |
| `MaoLiAo.vcxproj` | v142, Win32 + x64, Console subsystem |

## What the examples do **not** load

Portable samples under `examples/` never open BMP or MP3 files. They
only reuse numbers and tables so Linux CI / a laptop without EasyX can
still check the math. To inspect sheets visually, open them in any
bitmap viewer; the row table above is the index.

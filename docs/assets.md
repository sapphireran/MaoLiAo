# Assets

Runtime paths are Windows-style (`res\role.bmp`). They are resolved from the process working directory, not from the `.exe` location. Keep `res\` beside the debugger CWD.

Bitmaps and MP3s are often **not** in git (they are large and were added/removed across the 2020 commits). This page records how the **code** crops them so a restored `res\` still matches.

## `res\role.bmp` — hero

32×32 cells.

| Crop (x0, y0) | Use |
| --- | --- |
| `(0, 0)` / `(32, 0)` | walk color frames, facing right |
| `(0, 32)` / `(32, 32)` | walk masks, facing right (`SRCAND`) |
| `(96, 0)` / `(128, 0)` | walk color, facing left (`+ 3*WIDTH`) |
| `(96, 32)` / `(128, 32)` | walk masks, facing left |
| `(64, 0)` / `(64, 32)` | death color / mask |

`hero_iframe` flips 1 ↔ 2 from `(-x0 + x) / STEP % 4` with `STEP = 10`.

`showDied` also blits the death **color** frame as a life icon (1–4 copies).

## `res\ani.bmp` — shared FX

Used by enemies, bombs, bullets, coins, food, and the +10 pop.

| Rows (y / 32) | Use |
| ---: | --- |
| 0 / 1 | enemy color / mask, 2 walk frames |
| 2 / 3 | bullet color / mask, 2 frames |
| 4–5 / 6–7 | bomb (64×64), 4 frames, then slot cleared |
| 8 / 9 | coin color / mask, 4 frames |
| 10 / 11 | score pop color / mask |
| 12 | food / weapon (52×25 color + mask stacked) |

`enemy_iframe` advances `TIME * 5` and wraps at 3 → 1.
`coin_iframe` advances `TIME * 7` and wraps at 5 → 1.
`food_iframe` wraps at 3 → 1.

## `res\map.bmp` — tiles

One column, 32 px (or 64 px for pipes) per `id`.

`Scene::show` loops `xAmount * yAmount` and draws:

- id 7: 64×32
- id 8 and 10: 64×64
- id 1–6, 9: 32×32 at `y0 = (id-1)*HEIGHT`

## `res\scenery.bmp` — ids 11–14

3×2 tiles (96×64) with a mask row. `y0 = (id-11)*4*HEIGHT`. `scenery_iframe` 1–2, step `F` (`TIME*0.3`). Trees sway, water flows.

## `res\mapsky.bmp`

Loaded as `XSIZE × 4*YSIZE` (512×1536). Each world shows a 512×384 band:

```
yBg = -(world - 1) * 384
```

World 1: top sky. World 2: second band. World 3: third. The bitmap is wide-scrolled via `xBg` and a wrapped second blit.

## `res\home.bmp`

Loaded as `512 × 5*384` (512×1920). Vertical strips:

| `putimage` y | Screen |
| --- | --- |
| 0 | title |
| `-YSIZE` | game over (`showGameOver`) |
| `-2*YSIZE` | death continue (`showDied`) and “next level” (`showPassed`) |
| `-3*YSIZE` | all-clear (`showPassedAll`) |

Five virtual pages stacked in one file.

## `res\*.mp3`

See the alias table in [architecture.md](architecture.md). Missing files make `mciSendString` fail silently; the game still runs.

## `MaoLiAo.ico` / `MaoLiAo.rc`

`IDI_ICON1` (101). Window icon only.

## `gameRecord.dat`

ASCII world number, no newline required. See [controls-and-save.md](controls-and-save.md).

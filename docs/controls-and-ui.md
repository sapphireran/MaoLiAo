# Controls, menus, save file

## In-game keys

`Control::GetCommand` uses `GetAsyncKeyState` with bit `0x8000` (key down now).

| Macro | Value | Key | Used by |
| --- | ---: | --- | --- |
| `CMD_LEFT` | 1 | A | `Role::action` accel −`A_ROLE` |
| `CMD_RIGHT` | 2 | D | accel +`A_ROLE` |
| `CMD_UP` | 4 | W or K | jump |
| `CMD_DOWN` | 8 | S | no-op in `action` |
| `CMD_SHOOT` | 16 | J | bullets if `Hero.isShoot` |
| `CMD_ESC` | 32 | Esc | `pauseClick` |

`Control::getKey` updates `key` only when `_kbhit()` is true, then if `key & CMD_ESC` it enters the pause loop. After resume it may still return the last bitmask until another console key arrives.

## Title (`Control::gameStart`)

Background: `res\home.bmp` stretched to `XSIZE × 5*YSIZE`, first band (`putimage(0, 0, …)`).

Five stacked buttons, 90×30, centered at `x = 256 ± 45`, `y` from `YSIZE/3`:

| Order | Label | Hover | Action |
| --- | --- | --- | --- |
| 0 | 开始 | Start | leave the loop; `main` constructs `Role`/`Scene` |
| 1 | 介绍 | Introduction | three-world blurb, credit PWB |
| 2 | 指导 | Directions | A/D/J/W·K/Esc |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | read `gameRecord.dat` into global `world` |

介绍 / 指导 have a 返回 button at the bottom-right. That path calls `gameStart()` again (nested), then falls out of the inner loop.

读档: `fopen_s(..., "r")` + `fscanf_s` one `int`. Valid range is **1–3**. Outside that, a `MessageBox` runs (caption/text arguments are swapped in source — [quirks.md](quirks.md)). On success, `world` is set and the title loop ends so `main` starts that world with full lives.

## Pause (`Control::pauseClick`)

Green 90×120 panel. Hover recolors a row blue and switches the label to English.

| Row | Idle | Hover | Return |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` (64) |
| 1 | 重新开始 | start again | `VIR_RESTART` (128) |
| 2 | 退出游戏 | The menu | `VIR_HOME` (256) — **title, not process exit** |
| 3 | 进行存档 | Write Data | write world, then `VIR_RETURN` |

`main` handles restart / home. Save does not pause the simulation beyond the menu loop; it only writes the current global `world`.

## HUD

`showScore` / `showLevel` use font `Cooper`, transparent background:

- Score at `(10, 10)`: `得分:  ` + integer
- Level at `(XSIZE - 90, 10)`: `关卡:  ` + `world`

Scoring in `Role`:

| Event | Points |
| --- | ---: |
| Coin | +10 |
| Stomp enemy | +5 |
| Bullet hits enemy | +5 |

Weapon pickup plays `music_getWeapon` and sets `isShoot`; it does not add points.

## Overlays

All of these reload `res\home.bmp` at `512 × 1920` and `putimage` a vertical slice:

| Function | Y offset | Duration |
| --- | --- | --- |
| `showGameOver` | `-YSIZE` | 6500 ms |
| `showPassed` | `-2*YSIZE` | 2000 ms + `LEVEL: n` |
| `showPassedAll` | `-3*YSIZE` | 7800 ms |
| `showDied` | `-2*YSIZE` | 2000 ms + remaining-life sprites from `role.bmp` |

`showDied` draws 1–4 hero faces. With `LIFE == 5`, the first death (`life == 4`) shows four icons; the last life before Game Over shows one.

`main` waits 3500 ms on a frozen frame before `showDied` / Game Over. On a normal clear it waits 6500 ms on win music before `showPassed` / credits.

## `gameRecord.dat`

Path: `MaoLiAo/gameRecord.dat` relative to the process cwd.

Format: one ASCII integer, no newline required. The copy in git currently contains `3`.

| Value | Meaning |
| --- | --- |
| 1, 2, 3 | start that world after 读档 |
| other | MessageBox, stay on title |

Not stored: score, lives, `isShoot`, position, coins.

Write (pause → 进行存档):

```c
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
```

Read (title → 读档):

```c
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
```

There is no null check on `fopen_s`. A missing file is undefined (often a crash on `fscanf_s`). Keep a valid file next to the exe, or use the samples under `examples/save_file/samples/`.

Portable parser: `examples/save_file`.

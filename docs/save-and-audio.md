# Save file and audio aliases

## `gameRecord.dat`

The pause row **进行存档** writes a single integer — the global `world` — with `fprintf_s`:

```text
fopen_s(..., "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
```

Home **读档** reads it back:

```text
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)  → MessageBox "存档缺失，请检查存档"
else                         → world = flag, leave the home loop
```

There is no checksum, no score, no life count, no hero position. Loading a `2` starts world 2
with a fresh `Role` / `Scene` and `LIFE` lives because `main` only rebuilds those objects after
`gameStart` returns.

The file is opened with a path relative to the process working directory (`gameRecord.dat`), not
under `res\`. A committed copy in `MaoLiAo/gameRecord.dat` currently holds `3`.

Rejected values: `0`, negatives, and `>= 4`. The MessageBox argument order is
`("提醒", "存档缺失，请检查存档")` — title and text are swapped relative to the usual
`MessageBox(hwnd, text, caption, type)` convention, so the dialog reads backwards.

`fopen_s` failure is not checked. A missing file can crash the read path; the portable example
treats a missing file as “no save” and only accepts 1..3.

## Audio aliases

Opened in `main`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | Loop during play; rewind on respawn |
| `music_win` | `res\胜利.mp3` | Any clear |
| `music_passedAll` | `res\通关.mp3` | After world 3 |
| `music_end` | `res\游戏结束.mp3` | Lives exhausted |

Opened in `Role` constructor:

| Alias | File | When |
| --- | --- | --- |
| `music_died` | `res\死亡1.mp3` | Pit or enemy contact |
| `music_jump` | `res\跳.mp3` | Jump impulse |
| `music_coin` | `res\金币.mp3` | Coin |
| `music_tread` | `res\踩敌人.mp3` | Stomp |
| `music_getWeapon` | `res\吃到武器.mp3` | Food |
| `music_bullet` | `res\子弹.mp3` | Fire |
| `music_boom` | `res\子弹撞墙.mp3` | Bullet vs tile / range |
| `music_boom2` | `res\子弹打到敌人.mp3` | Bullet vs enemy |

`Winmm.lib` is pulled in with `#pragma comment` from both `main.cpp` and `role.cpp`. Aliases are
not closed when a `Role` is replaced (restart / next world), so MCI keeps the last open handle.

`examples/save/save_format.cpp` writes and reads temp files with the same grammar.

# Input, HUD, and save file

## Live keys

`Control::GetCommand` ORs `GetAsyncKeyState` high bits:

```
A → CMD_LEFT     D → CMD_RIGHT
W or K → CMD_UP  S → CMD_DOWN
J → CMD_SHOOT    Esc → CMD_ESC
```

`getKey` only *samples* when `_kbhit()` is true, but it **returns the last
sampled `key` every frame**. The practical effect is sticky input: a tap
that filled the console buffer keeps applying until another `_kbhit` overwrites
it, and there is no “key up” clear except by reading a new combination.
`Esc` additionally opens the pause overlay and may replace `key` with a
`VIR_*` bit.

`main` then:

- `VIR_RESTART` → rebuild `Scene`/`Role` for the current world, rewind BGM
- `VIR_HOME` → reset lives and world to 1, `gameStart()`, rebuild, rewind BGM

## Pause overlay

Four 90×30 hit boxes centered at `XSIZE/2`, starting at `YSIZE/3`:

| Index | Chinese label | Hover English | Result |
| --- | --- | --- | --- |
| 0 | 返回游戏 | Return | `VIR_RETURN` (resume) |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` (not `exit`) |
| 3 | 进行存档 | Write Data | write save, then `VIR_RETURN` |

Hover recolors the cell blue and swaps in the English caption; leaving
restores green + Chinese. Implementation polls `GetMouseMsg` in a nested
loop, so the game timer is frozen while paused.

## Home menu

`gameStart` draws `home.bmp` strip 0 and five cells: 开始 / 介绍 / 指导 /
退出 / 读档. 退出 *does* `exit(0)`. 介绍 and 指导 swap to text pages with a
bottom-right 返回. 读档 reads `gameRecord.dat`.

`home.bmp` is loaded at `XSIZE × 5*YSIZE` (512×1920). Other strips:

| `putimage` y | Screen |
| --- | --- |
| 0 | Home / intro / controls |
| `-YSIZE` | Game over (`showGameOver`, 6.5 s) |
| `-2*YSIZE` | Level card (`showPassed`) and death life-icons |
| `-3*YSIZE` | All-clear (`showPassedAll`, 7.8 s) |

Death leftover lives are drawn as 1–4 copies of the hero death frame, not as
a numeral. 0 lives goes to game over instead.

## Save file

Path: `gameRecord.dat` in the working directory (the copy under
`MaoLiAo/gameRecord.dat` currently contains ASCII `3`).

Write (pause → 进行存档):

```cpp
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
```

Read (home → 读档):

```cpp
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)  /* MessageBox, stay on menu */
else world = flag;
```

Only the world index is stored. Score, lives, `isShoot`, and pipe RNG are
not. A missing file is not handled: `fopen_s` failure still `fscanf_s`s.
Python helper: `examples.maoliao_lab.savefile`.

## HUD

Each gameplay frame, after batch-draw of scene + role:

- `showScore`: `"得分:  "` + itoa, Cooper, `(10, 10)`
- `showLevel`: `"关卡:  "` + itoa, Cooper, `(XSIZE - 90, 10)`

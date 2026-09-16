# Input, UI, and save

## Command bits

`Control::GetCommand` ORs `GetAsyncKeyState` high bits. Several keys can be down in one frame.

| Macro | Value | Key | Meaning |
|---|---|---|---|
| `CMD_LEFT` | 1 | `A` | walk left |
| `CMD_RIGHT` | 2 | `D` | walk right |
| `CMD_UP` | 4 | `W` or `K` | jump (world 3: air jump) |
| `CMD_DOWN` | 8 | `S` | read, unused in `Role::action` |
| `CMD_SHOOT` | 16 | `J` | fire if `isShoot` |
| `CMD_ESC` | 32 | `Esc` | open pause |
| `VIR_RETURN` | 64 | pause click | resume |
| `VIR_RESTART` | 128 | pause click | rebuild this world |
| `VIR_HOME` | 256 | pause click | `life=5`, `world=1`, home menu |

`getKey` only refreshes `key` when `_kbhit()` is true, so a held WASD combo **latches** until another console key event arrives. Esc is checked on the latched value, which is why the pause menu can open from a sticky `CMD_ESC`.

`examples/src/command_lab.cpp` decodes the same bit masks.

## Home menu (`gameStart`)

Five stacked 90×30 hit boxes centered at `x = 256`, starting at `y = YSIZE/3 = 128`:

| Slot | Chinese | Hover English | Action |
|---|---|---|---|
| 0 | 开始 | Start | leave the menu, start `world` (or the loaded world) |
| 1 | 介绍 | Introduction | three-line blurb, credit `PWB`, 返回 |
| 2 | 指导 | Directions | A/D/J/W·K/Esc |
| 3 | 退出 | Exit | `exit(0)` |
| 4 | 读档 | Read Load | parse `gameRecord.dat` |

Introduction copy in the binary:

```
这是一款横版过关
游戏。游戏主角叫
猫里奥，共有三关
第三关为跳跃关卡
游戏开发者：PWB
```

“读档” accepts `1..3`. Anything else (`<= 0` or `>= 4`) is supposed to warn, but the `MessageBox` argument order is swapped (`text` / `caption` are reversed) and the file pointer is not closed on that path. See [quirks.md](quirks.md).

Hover uses `getpixel` of the button interior: green on home, red on 返回.

## Pause menu (`pauseClick`)

Opened from `CMD_ESC`. Four 90×30 rows, green panel:

| Slot | Chinese | Hover English | `key` returned |
|---|---|---|---|
| 0 | 返回游戏 | Return | `VIR_RETURN` |
| 1 | 重新开始 | start again | `VIR_RESTART` |
| 2 | 退出游戏 | The menu | `VIR_HOME` |
| 3 | 进行存档 | Write Data | write save, then `VIR_RETURN` |

Comments still say slot 2 is “主菜单”; the drawn label is 退出游戏. `main` treats `VIR_HOME` as a full reset to world 1 + home screen, not `exit`.

## Save file

Path: `gameRecord.dat` in the **process working directory** (so run with cwd = `MaoLiAo/`).

Format: a single `fprintf` / `fscanf` integer, no newline required.

```
3
```

| Writer | Reader |
|---|---|
| pause → 进行存档 (`fprintf_s(fp, "%d", world)`) | home → 读档 (`fscanf_s(fp, "%d", &flag)`) |

This checkout’s committed file contains `3`.

`examples/src/save_lab.cpp` round-trips the same format against `examples/testdata/gameRecord.sample.dat`.

## Interstitial art (home.bmp strips)

`home.bmp` is 512×1920 = five stacked 384 px pages. `putimage(0, -n * YSIZE, &img)` reveals page `n`:

| Call | `n` | When |
|---|---|---|
| `gameStart` | 0 | title |
| `showGameOver` | 1 | lives exhausted (`Sleep(6500)`) |
| `showPassed` / `showDied` | 2 | next-level card / remaining lives |
| `showPassedAll` | 3 | credits-style clear (`Sleep(7800)`) |
| (unused) | 4 | extra art |

Death with lives left draws 1–4 hero faces from `role.bmp` column 2, then `Sleep(2000)`.

## HUD

- Score: font `Cooper`, `得分:  N` at (10, 10).
- Level: `关卡:  N` at (422, 10).

Both toggle `TRANSPARENT` then restore `OPAQUE`.

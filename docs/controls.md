# Controls, menus, and saves

Input is a bitfield. `Control::GetCommand` ORs Windows async key state into the macros from `define.h`. `getKey` latches the last non-zero command: `_kbhit()` must fire before bits update, then the same `key` is returned every frame until another `_kbhit()`. Holding a key therefore “sticks” across frames that miss the poll, and releasing a key does **not** clear `key` until some other key event arrives. The examples under `examples/input/` show the bit layout without Win32.

## Gameplay bits

| Macro | Value | Key |
| --- | --- | --- |
| `CMD_LEFT` | 1 | `A` |
| `CMD_RIGHT` | 2 | `D` |
| `CMD_UP` | 4 | `W` or `K` |
| `CMD_DOWN` | 8 | `S` (read, unused by `Role::action`) |
| `CMD_SHOOT` | 16 | `J` |
| `CMD_ESC` | 32 | `Esc` |

`Role::action` tests bits with `KEY & CMD_*`. Combinations are legal (left+jump, right+shoot, …).

## Virtual (menu) bits

| Macro | Value | Produced by |
| --- | --- | --- |
| `VIR_RETURN` | 64 | Pause: “返回游戏” |
| `VIR_RESTART` | 128 | Pause: “重新开始” |
| `VIR_HOME` | 256 | Pause: “退出游戏” (label says 主菜单 in comments) |

`main` only special-cases `VIR_RESTART` and `VIR_HOME`. `VIR_RETURN` is a no-op in the loop: play continues with whatever movement bits were latched before pause.

## Title screen (`Control::gameStart`)

Drawn over `home.bmp` (top 384 px of the 5-screen atlas). Five stacked 90×30 buttons at x = 211–301:

1. **开始** — leave the menu, start `world` as-is (usually 1).
2. **介绍** — short copy: three worlds, third is a jump stage, credit “PWB”.
3. **指导** — the key list above.
4. **退出** — `exit(0)`.
5. **读档** — read `gameRecord.dat`.

Hover swaps the Chinese label for an English one (`Start`, `Introduction`, `Directions`, `Exit`, `Read Load`) and fills the button green. Intro / guide pages have a **返回** corner button that recursively calls `gameStart()`.

Load path (`fopen_s` + `fscanf_s` of one `%d`):

- If the integer is outside `1..3`, a `MessageBox` says the save is missing.
- Otherwise `world` is set and the title loop ends. Lives are **not** restored; `life` stays at the current global (5 on a fresh boot).

## Pause screen (`Control::pauseClick`)

`Esc` sets `CMD_ESC` on the latched key. `getKey` then enters a blocking mouse loop (the game timer is not ticking). Four buttons:

1. Return to play (`VIR_RETURN`).
2. Restart current world (`VIR_RESTART`) — `main` rebuilds `Scene` and `Role`, rewinds BGM.
3. Title (`VIR_HOME`) — `main` resets `life = 5`, `world = 1`, shows the title, rebuilds, restarts BGM.
4. Save — writes `fprintf_s(fp, "%d", world)` to `gameRecord.dat`, then returns as if Resume.

Hover uses English labels (`Return`, `start again`, `The menu`, `Write Data`).

`FlushMouseMsgBuffer` runs before the loop because the 10 ms frame delay otherwise leaves stale clicks.

## HUD

Every play frame, after `Scene.show` and `Role.show`:

- `showScore` — `"得分:  "` at (10, 10), font Cooper.
- `showLevel` — `"关卡:  "` at (422, 10).

## Transitions

| Event | Visual | Sleep |
| --- | --- | --- |
| Death, lives remain | `home.bmp` band at `-2*YSIZE`, 1–4 dead-cat sprites for remaining lives | 2000 ms after a 3500 ms freeze of the playfield |
| Life == 0 | `home.bmp` band at `-YSIZE` (`showGameOver`) | 6500 ms, plus the 3500 ms freeze |
| Clear world 1 or 2 | `home.bmp` band at `-2*YSIZE` and `"LEVEL: N"` | 6500 ms sting + 2000 ms card |
| Clear world 3 | win sting 6500 ms, then `home.bmp` band at `-3*YSIZE` for 7800 ms | then title |

`showDied` only has branches for `life` 1–4. After a death `main` does `life--` and then `showDied(life)`, so five lives remaining is never drawn on that card.

## Save format

`MaoLiAo/gameRecord.dat` is a tiny text file: the current world as decimal ASCII, no newline required.

```
3
```

That is the entire format. Score, lives, weapon flag, and camera `x0` are not stored. The copy in the repo currently contains a lone `3` (world 3).

Python helpers:

```
python3 examples/save/record_demo.py --read MaoLiAo/gameRecord.dat
python3 examples/save/record_demo.py --write /tmp/gameRecord.dat --world 2
```

Invalid values (`<= 0` or `>= 4`) are rejected by both the game and the helper, matching `control.cpp`.

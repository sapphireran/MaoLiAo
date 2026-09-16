# Controls and saves

## Keyboard bits

`Control::GetCommand` samples `GetAsyncKeyState` every time `_kbhit()` is true and OR-s:

| Bit | Macro | Key |
| --- | --- | --- |
| 1 | `CMD_LEFT` | A |
| 2 | `CMD_RIGHT` | D |
| 4 | `CMD_UP` | W **or** K |
| 8 | `CMD_DOWN` | S (read, never used) |
| 16 | `CMD_SHOOT` | J |
| 32 | `CMD_ESC` | Esc |

`getKey` keeps returning the last combo until another `_kbhit()`. That means a tap can “stick” for extra frames if you do not press something else. Esc opens `pauseClick` and may replace `key` with a virtual bit (64 / 128 / 256).

Arrow keys are not bound. There is no gamepad path.

## Title menu

`home.bmp` is blitted at (0,0). Five stacked 90×30 hit boxes centered at x=256, starting y=128:

1. **开始** — leave the menu, keep current `world` (1 unless 读档 succeeded).
2. **介绍** — overlay text: three levels, third is the jump stage, credit `PWB`.
3. **指导** — A/D/J/W·K/Esc.
4. **退出** — `exit(0)`.
5. **读档** — read `gameRecord.dat`.

Hover swaps the Chinese labels for English (`Start`, `Introduction`, `Directions`, `Exit`, `Read Load`). The pause menu does the same (`Return`, `start again`, `The menu`, `Write Data`).

## Pause menu (Esc)

Drawn on top of the live world (the world is not frozen underneath; `GetMouseMsg` blocks the loop, so time stops anyway).

| Box | Chinese | Effect |
| --- | --- | --- |
| 0 | 返回游戏 | `VIR_RETURN` — resume with the latched key |
| 1 | 重新开始 | `VIR_RESTART` — new `Scene`/`Role` for the **current** `world`; BGM seek 0 |
| 2 | 退出游戏 | `VIR_HOME` — despite the label this is the title menu; `life=5`, `world=1` |
| 3 | 进行存档 | write `world` as `%d` to `gameRecord.dat`, then `VIR_RETURN` |

Save does **not** store life, score, flower, or camera. It is a world index only.

## `gameRecord.dat`

Path is relative to the process working directory: `"gameRecord.dat"` (not `MaoLiAo\gameRecord.dat`). Running the exe from the solution root will create/read a different file than the one in this repo.

Format: `fprintf_s(fp, "%d", world)` on write; `fscanf_s(fp, "%d", &flag)` on read.

`Control::gameStart` treats a parsed value as missing when `flag >= 4 || flag <= 0`. It then `MessageBox(..., "提醒", "存档缺失，请检查存档", 1)` and stays on the title menu. Valid values are therefore **1, 2, 3**.

Failure modes the C++ does not handle:

- File missing: `fopen_s` leaves `fp == NULL`, then `fscanf_s` is undefined. The fixtures mark this as `crash-open`, not as the MessageBox path.
- Non-numeric content: `fscanf_s` fails and `flag` is uninitialized; the `>=4 \|\| <=0` check is then garbage. Fixtures label that `undefined-parse`.
- Trailing whitespace or a newline after the digit is fine (`%d`).

Checked-in file: `MaoLiAo/gameRecord.dat` is the single byte `3` (world 3). Copies for tests live in `fixtures/saves/`:

| File | Bytes | Class |
| --- | --- | --- |
| `world-1.dat` | `1` | valid |
| `world-2.dat` | `2` | valid |
| `world-3.dat` | `3` | valid |
| `invalid-zero.dat` | `0` | missing-archive |
| `invalid-four.dat` | `4` | missing-archive |
| `invalid-negative.dat` | `-1` | missing-archive |
| `empty.dat` | (empty) | undefined-parse |
| `garbage.dat` | `abc` | undefined-parse |

`tools/validate_fixtures.py` classifies them with the same `<=0 \|\| >=4` rule after a strict `int` parse, and records the crash-open / undefined-parse cases separately.

## HUD

`showScore` / `showLevel` use the `Cooper` face if present, else EasyX’s fallback. Positions: score at (10,10), level at (422,10). Death intermission (`showDied`) draws 1–4 hero faces (not 5 — if you die with `life` going 5→4 the code shows four sprites; the `life==5` branch does not exist). Starting life is 5, so the first death shows four remaining faces. The fifth face is never drawn because `life==0` goes to game over instead of `showDied`.

# 09 — Save system

There is one save file and one integer.

## Location

```text
gameRecord.dat
```

Relative to the **process working directory** (usually the folder of the `.exe` when launched from Visual Studio). A copy already sits at `MaoLiAo/gameRecord.dat` in this repo and currently contains `3`.

## Format

ASCII digits of the world index, no newline required. The writer is:

```cpp
FILE* fp = NULL;
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
fclose(fp);
```

The reader:

```cpp
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0) { /* MessageBox: 存档缺失 */ }
else { world = flag; }
```

Accepted values: **1, 2, 3**.

## What is *not* saved

- Lives (always reset to `LIFE` on home; death keeps the decremented `life` only in RAM)
- Score
- Hero position, velocity, or `isShoot`
- Remaining coins / enemies
- World-3 randomized pipe heights

A load is “start this world from the beginning.”

## When it is written

Only from the pause menu row 进行存档. There is no autosave on clear or death. Clearing world 1 and quitting without pausing loses the fact that you were about to play world 2.

## Failure modes

- File missing: `fopen_s` + `fscanf_s` on a null / empty stream — the shipped code does not check `fp` before `fscanf_s`.
- Value outside 1–3: MessageBox. Arguments are `(hwnd, "提醒", "存档缺失，请检查存档", 1)` so the **caption** is `提醒` and the **text** is the missing-save sentence only if you swap them mentally — actually Win32 is `(text, caption)`, so the user sees title 「存档缺失，请检查存档」 and body 「提醒」.
- `fopen_s` on write is not null-checked either.

The portable reader/writer in `examples/04_save_record` rejects out-of-range values without crashing and covers the empty-file case.

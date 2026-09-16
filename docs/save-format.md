# Save format

There is one save file: `gameRecord.dat` in the **process working directory** (same caveat as `res/` — usually `MaoLiAo/`).

## Layout

Plain text, one integer, no newline required:

```text
<world>
```

Examples of valid files:

```text
1
```

```text
2
```

```text
3
```

That integer is the world the title “读档” button assigns to the global `world` before `gameStart` returns. Lives are **not** saved; a load still uses the current `life` (reset to 5 only when you die out or finish world 3 or hit 退出游戏 on pause).

## Write path

Pause → 进行存档:

```cpp
FILE* fp = NULL;
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
fclose(fp);
```

`fp` is not null-checked. A failed open (missing cwd, read-only tree) will crash on `fprintf_s`.

The file is truncated on each save. Score, position, `isShoot`, and remaining lives are discarded.

## Read path

Title → 读档:

```cpp
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0) {
    MessageBox(..., "提醒", "存档缺失，请检查存档", 1);
} else {
    world = flag;
    fclose(fp);
    /* leave the title loop */
}
```

Issues worth knowing (also listed in [known-issues.md](known-issues.md)):

1. Missing file: `fopen_s` failure is ignored; `fscanf_s` on a null `fp` is undefined.
2. `MessageBox` argument order is title/text swapped relative to the usual `(hwnd, text, caption, type)` — the dialog will look odd.
3. `fp` is only `fclose`d on the success branch.
4. Values outside 1–3 are rejected, but a file containing `3` is accepted even if you never reached world 3.

A portable read/write helper (no `fopen_s`) is `examples/save_record.cpp`.

## Suggested extensions (not implemented)

If you personally extend the format, keep it text and versioned, for example:

```text
MLA1
world=2
life=4
score=80
shoot=1
```

The current parser cannot read that — you would need a new reader and a fallback for a bare integer so old `gameRecord.dat` files still work.

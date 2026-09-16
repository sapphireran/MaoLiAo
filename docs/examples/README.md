# Examples

These are **data** examples. They do not launch EasyX. Use them when you change a map literal or the save rules and want a diff that is not “open Visual Studio.”

## 1. Re-check every fixture

From the repo root:

```bash
python3 tools/validate_fixtures.py
```

Expected last line: `OK: N checks passed.` The script exits `1` on the first class of failures and prints a unified-style list (physics drift, C++ vs JSON, ASCII golden mismatch, bad save classification).

Quiet mode:

```bash
python3 tools/validate_fixtures.py --quiet
```

## 2. Print an ASCII map

```bash
python3 tools/render_level.py --world 1
python3 tools/render_level.py --world 2
python3 tools/render_level.py --world 3
```

World 3 uses the **example** height table in `fixtures/levels/world-3.json`, not live `rand()`.

After an intentional map edit:

```bash
python3 tools/render_level.py --world 1 --write-expected
python3 tools/validate_fixtures.py
```

`--write-expected` overwrites `fixtures/expected/world-N.ascii.txt`. Do not do that unless the C++ literals and the JSON already agree.

Legend (same as the golden files):

```text
.  empty
#  grass brick (1)
=  snow surface (5)
~  dirt / snow under (3, 4)
P  pipe body (6)
v  pipe shaft / down mouth (7, 8)
^  up pipe mouth (10)
C  cloud (2)
o  coin
e  enemy facing right    E  enemy facing left
*  flower (snapped to the tile it sits on)
```

Decorative IDs 11–14 are omitted from the collision ASCII so the playable silhouette stays readable. They are still listed in the JSON.

## 3. Save-file round trip

`fixtures/saves/` is a copy of what `Control` reads with `fscanf_s("%d")`.

```bash
python3 tools/validate_fixtures.py --list-saves
```

| Example | You should see |
| --- | --- |
| `world-1.dat` … `world-3.dat` | `valid` → title **读档** sets `world` |
| `invalid-zero.dat`, `invalid-four.dat`, `invalid-negative.dat` | `missing-archive` → MessageBox 存档缺失 |
| `empty.dat`, `garbage.dat` | `undefined-parse` → do not ship; C++ leaves `flag` uninitialized |
| (file absent) | `crash-open` → `fopen_s` + null `fscanf_s` |

To recreate the checked-in slot from a fixture on Windows (cwd = `MaoLiAo/`):

```bat
copy /Y ..\fixtures\saves\world-2.dat gameRecord.dat
```

Then **读档** should start world 2. The repo’s own `MaoLiAo/gameRecord.dat` matches `fixtures/saves/world-3.dat`.

## 4. Diff fixtures against the C++ source

The validator already does this. To see only the extractor:

```bash
python3 tools/extract_source.py
```

It prints JSON on stdout: physics macros, world 1–2 tile/coin/food/enemy tables, and the world 3 generation recipe. Compare by eye with `fixtures/levels/` if you prefer a pager to an assertion.

## 5. What “good” looks like

A passing run on this branch reported **455 checks passed, 0 failed**:

- physics macros identical to `define.h`
- world 1: 32 authored tiles, 30 runtime, 20 coins, 10 enemies, 1 flower
- world 2: 36 authored tiles, 30 runtime, 11 coins, 6 enemies, 1 flower
- world 3 example: 30 tiles from 7 pipe columns + runway + flag
- 8 save fixtures classified as in the table above
- ASCII goldens matching `fixtures/expected/`

If your numbers differ, the C++ changed and the JSON was not updated — or the other way around.

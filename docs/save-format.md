# Save format

There is one save file: `gameRecord.dat` in the process working directory (next to the exe when launched from Visual Studio with the default working dir `MaoLiAo/`).

## Layout

Plain ASCII decimal integer, no newline required, no other fields.

```
<world>
```

`world` must be `1`, `2`, or `3`. The title-screen loader rejects `flag <= 0 || flag >= 4`.

The copy in this repo currently contains `3`.

## Writer (pause menu)

```cpp
FILE* fp = NULL;
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
fclose(fp);
```

Truncates the file. Does not check `fp == NULL`. Does not save:

- remaining lives
- score
- `isShoot`
- camera / hero position
- world-3 `rand` state

Loading a `2` starts **world 2 from the spawn point** with full default `Role` / `Scene` constructors, but `life` is whatever the global currently is (title load does not reset `life`).

## Reader (title “读档”)

```cpp
FILE* fp;
int flag;
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0) {
    MessageBox(...);
} else {
    world = flag;
    fclose(fp);
    // leave title loop
}
```

If `fopen` fails, `fp` is null and `fscanf_s` is still called — undefined. The examples’ loader treats missing files as an error instead of crashing.

## Headless tools

```bash
python3 examples/python/cli.py save-read path/to/gameRecord.dat
python3 examples/python/cli.py save-write path/to/gameRecord.dat --world 2
```

The Python writer uses the same `"%d"` payload so a file it produces is a valid game save.

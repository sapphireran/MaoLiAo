# Save format

There is one save file, created next to the working directory (not under `res/`):

```
gameRecord.dat
```

The copy committed in `MaoLiAo/gameRecord.dat` contains the single character `3` (world 3).

## Layout

Plain text, one integer, no newline required:

```
<world>
```

`world` is the global stage index `1`, `2`, or `3`. Score, lives, camera, pickups, and enemy state are **not** stored. Loading a save only changes which `Role`/`Scene` constructors run after the title menu.

## Write

Pause menu → 进行存档:

```cpp
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
fclose(fp);
```

`"w"` truncates. No version byte, no checksum.

## Read

Title → 读档:

```cpp
fopen_s(&fp, "gameRecord.dat", "r");
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0)
    MessageBox(..., "提醒", "存档缺失，请检查存档", 1);
else {
    world = flag;
    fclose(fp);
}
```

Issues worth knowing:

1. `fopen_s` failure is ignored. A missing file passes a null `FILE*` to `fscanf_s`.
2. The success path closes the file; the failure path does not.
3. Values outside 1..3 are rejected, but a file containing `2 extra` still loads 2.
4. The working directory must be the folder that contains `gameRecord.dat` when the exe starts (often `MaoLiAo/` or `MaoLiAo/Debug/`). The committed file sits beside the sources.

## Portable I/O

`examples/include/maoliao/savefile.h` reads/writes the same one-integer format, rejects out-of-range values, and treats a missing file as an error instead of crashing. `examples/demos/savefile_demo` round-trips worlds 1–3 and refuses 0 and 4.

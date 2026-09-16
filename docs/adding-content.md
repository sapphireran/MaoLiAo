# Adding content (personal cookbook)

Worked recipes against the current sources. Edit the arrays in `scene.cpp` / `role.cpp`, keep [levels.md](levels.md) in sync, and stay under the capacity constants.

## Add a solid platform (world 1)

In `Scene::createMap`, world `== 1`, append a `Map` row **before** the closing `};` of `m[]`:

```cpp
{ 50, 6, 1, 3, 1 },   // x=50, y=6, grass, 3 tiles wide, 1 tall
```

Meaning:

- `x`, `y` — top-left cell.
- `id` — 1 for grass you can stand on (see the id table in levels.md).
- `xAmount`, `yAmount` — how many 32 px cells to stamp and how big the hit box is.

Then count the rows. If you now have more than `MAP_NUMBER` (30), raise that constant in `scene.h` **and** confirm `map[MAP_NUMBER]` still fits your comfort with a 30-ish linear scan every tick.

Friction `u` is assigned in the `switch (m[i].id)` that follows. You do not set `u` in the initializer.

## Add a coin

World 1 `createCoin`:

```cpp
POINT p[] = {
    { 10, 5 }, /* existing */
    { 51, 5 }, /* new: one cell above the platform at y=6 */
};
```

Coins use **tile** coordinates. The collect spark needs a free slot in `score[SCORE_NUMBER]` (5). Do not place a coin at `(0, 0)` — `show` treats `x == 0` as “empty slot.”

## Add food (gun / world-3 immunity)

World 1 `createFood` is already in **pixels**:

```cpp
POINT p[] = { { 14 * WIDTH, 5 * HEIGHT }, { 51 * WIDTH, 5 * HEIGHT } };
```

Zeroing a collected food uses `x == 0 && y == 0` as empty, so do not spawn at the origin.

## Add an enemy

`Role::createEnemy`, world 1:

```cpp
Enemy emy[] = {
    { 3, 8, 1 },
    { 51, 5, -1 },   // cell x, cell y, +1 right / -1 left
};
```

The loader multiplies x/y by `WIDTH`/`HEIGHT`. `turn == 0` means dead / unused. Patrol AI needs a floor: a 1-wide pillar with no ledge check space will flip every tick.

Stay under `ENEMY_TOTE` (30). The copy loop is `i <= sizeof/sizeof` (off-by-one) — prefer changing that loop to `i < n` when you touch it.

## Add a world-3 pipe column

World 3 is generated. To add an eighth column, uncomment the `x[7]` block in `createMap` **and** make sure `MAP_NUMBER` can hold `8 * 4 + 2` (runway + flag) = 34 entries. Today only seven columns are live, which already consumes 28 + 2 = 30 slots — the array is full. Raising `MAP_NUMBER` is mandatory before enabling more pipes.

## Add a sound

1. Drop an mp3 in `res\\`.
2. `mciSendString("open res\\your.mp3 alias music_foo", NULL, 0, NULL);` next to the other opens in `Role` or `main`.
3. `mciSendString("play music_foo from 0", NULL, 0, NULL);` at the event.

There is no mixer. Keep clips short.

## Add a HUD number

`Control::showScore` / `showLevel` are the pattern: format a `char[]` with `_itoa_s` + `strcat_s`, `outtextxy` at a fixed pixel. Call it from `main` inside the `BeginBatchDraw` block so it composite on the same frame.

## Verify without Windows

After you change a formula (`G`, `V_MAX`, jump, AABB inset), update the matching constant in `examples/portable/maoliao_math.hpp` and run:

```bash
cd examples && make test
```

Layout-only edits (new tiles) can be reflected in `examples/portable/level_data.hpp` and checked with `./level_dump`.

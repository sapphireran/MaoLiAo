# Adding a level

The shipped game has a hard `world == 1|2|3` split in **four** places. A fourth world is a copy-paste job, not a data load. This page is the checklist that matches the current sources.

Use `examples/levels/example-world-4-canyon.json` (or `example-tutorial-parkour.json`) as the design file, then:

```bash
python3 examples/toolkit/maoliao_levels.py validate examples/levels/example-world-4-canyon.json
python3 examples/toolkit/maoliao_levels.py export-cpp examples/levels/example-world-4-canyon.json
```

Paste the printed blocks into the four functions below.

## 1. `Scene::createMap` — `MaoLiAo/scene.cpp`

Add `else if (world == 4)` next to the world 3 branch. Keep the record cap in mind:

- at most **30** `Map` rows (`MAP_NUMBER`)
- ids 1–10 are solid; 11–14 are scenery
- set `u` with the same `switch` the other worlds use, or copy the `overworld` / `pipes` profile from the JSON

The export prints a `Map m[] = { ... };` plus the friction loop. World 3’s loop is `i <= sizeof(...)` (off-by-one); prefer the world 1 form:

```cpp
int i = 0;
while (m[i].id > 0 && m[i].id < 15 && i < MAP_NUMBER) {
    // assign u, then map[i] = m[i]; i++;
}
```

## 2. `Scene::createCoin`

Same `world == 4` branch. Coins are tile `POINT`s. The install loops in the original file use `i <= sizeof(p)/sizeof(p[0])`, which writes **one past** the literal (and can walk off `coins[]` if you fill all 70). Prefer:

```cpp
int n = sizeof(p) / sizeof(p[0]);
for (int i = 0; i < n && i < COINS_NUMBER; i++)
    coins[i] = p[i];
```

## 3. `Scene::createFood`

Food is **pixels**. Convert tiles with `x * WIDTH`, `y * HEIGHT` unless you are placing a sub-tile offset like world 2 (`- WIDTH/3`, `+ HEIGHT/5`).

`FOOD_NUMBER` is 5. The pickup sets `Hero::isShoot = true` (gun + world-3 star).

## 4. `Role::createEnemy` — `MaoLiAo/role.cpp`

Tile coordinates, `turn` ∈ {1, −1}. The function multiplies by `WIDTH`/`HEIGHT` for you. Same off-by-one warning as coins: the shipped loops use `<=`.

## 5. `Scene::isEnding`

```cpp
case 4:
    if (distance > YOUR_TILES * WIDTH)
        return true;
    return false;
```

`YOUR_TILES` must match `ending_tiles` in the JSON. Auto-run starts there; `passed` fires when the sprite’s **screen** x exceeds 512.

## 6. Sky row

`Scene::show` picks the sky with:

```cpp
yBg = -(world - 1) * YSIZE;
```

`mapsky.bmp` is loaded as `XSIZE × 4*YSIZE`, so worlds 1–4 each have a 384 px band (y = 0, −384, −768, −1152). A fifth world would wrap or need a taller bitmap.

## 7. `main.cpp` clear logic

```cpp
if (world == 3) { /* credits */ }
else { world++; showPassed(world); ... }
```

If world 4 is the new finale, change the comparison to `world == 4`. If world 4 is just another stage, leave it: world 3 will currently jump to 4, and 4 will then try to become 5 (`Role(5)` / `Scene(5)` with empty maps). Either extend the finale test or stop incrementing.

Lives and the start menu still reset `world = 1`.

## 8. Save file

读档 rejects `flag >= 4`. Raise that bound when you add world 4 (`flag >= 5`).

## 9. Jump budget

From [physics.md](physics.md):

- apex ≈ **101 px ≈ 3.16 tiles**
- world 3 style gaps need either clouds (`id` 2) or infinite jump (special-case `world` in `Role::action`)
- slick `id` 6 shortens stopping distance (T1)
- do not place a required coin more than ~3 tiles above the floor the hero can stand on

`maoliao_levels.py jump` prints the numbers. `maoliao_levels.py validate` warns when a coin sits more than 4 tiles above any solid.

## 10. Local play test

Windows + EasyX only. After pasting:

1. Start from the menu (not 读档) so you begin at world 1, or temporarily set `int world = 4;` in `main.cpp`.
2. Confirm the camera rail still pins at x=192.
3. Confirm the goal sign is to the right of `ending_tiles`.
4. Confirm you cannot walk through scenery ids 11–14 (you should not — they are non-solid).
5. Confirm a world-3-style pipe still kills if you reused `world == 3` logic by mistake.

## Snippets

`examples/snippets/world4_create_map.cpp` is the exported canyon world, ready to paste. `examples/snippets/hit_test_example.cpp` is a portable copy of the inset-quad test used by `Role::isHit`.

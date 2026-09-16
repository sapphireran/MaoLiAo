# Design journal (personal, 2020 → now)

猫里奥 was a one-semester Windows game: EasyX blits, a handful of mp3s, and three handmade
worlds. Coming back to the tree years later, the useful part is not the `.vcxproj` — it is the
set of numbers that still describe a complete little platformer.

## What I would tell 2020-me

The camera is the whole game. The cat almost never leaves `x ∈ [0, 192]`. Everything else —
tiles, coins, enemies — is drawn at `x0 + world`. Once that click happens, parallax
(`K_MAP_BG = 5`), ending walks, and bullet range all make sense.

`Inertia::move` is a first-year kinematics line. The jump is “give me 3.5 m of hang time and
pretend the screen is 101 px tall.” Friction is “pick a time T to `V_MAX` and back out µ,” then
a `±3` fudge doubles it. That fudge is why the cat stops twice as fast as the comment claims.

World 3 is a weekend Flappy-Bird sketch stapled onto the same `Role`. Infinite jump, lethal
pipes, random heights, seven of ten planned columns. It is the only world that fits in
`MAP_NUMBER`.

## What the portable tree is for

The EasyX window needs a Windows desktop. The formulas do not. `examples/` is a Linux-friendly
lock on those formulas so a future edit to `define.h` has somewhere to fail besides “the jump
feels weird.”

This journal is not a rewrite plan. The shipped `MaoLiAo/*.cpp` files stay the 2020 course
binary. Docs and examples sit beside them.

## File-size map of the original

| Translation unit | Approx. lines | Density |
| --- | --- | --- |
| `control.cpp` | ~520 | UI hit tests, bilingual hover |
| `role.cpp` | ~660 | almost every gameplay rule |
| `scene.cpp` | ~450 | data tables + blit |
| `main.cpp` | ~140 | the loop |
| `inertia.cpp` | 9 | the integrator |
| `timer.h` | header-only | QPC sleep |

If something gameplay-shaped is “wrong,” it is in `role.cpp`. If a tile is missing, it is
either not in the table or it lost the `MAP_NUMBER` raffle.

## Resource sheet (runtime, not compiled)

Under `MaoLiAo/res/`, the original load names are:

- `home.bmp` — five stacked 512×384 cards
- `mapsky.bmp` — four stacked skies
- `map.bmp` — vertical tile ids
- `scenery.bmp` — animated tufts / water / trees
- `role.bmp` — cat walk / death / mirror
- `ani.bmp` — enemy, bomb, bullet, coin, score, weapon
- mp3s for bg, jump, coin, stomp, weapon, bullet, two booms, die, win, all-clear, game-over

The portable examples never open those files. They only repeat the numbers those blits were
drawn against.

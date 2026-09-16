# Known quirks (personal 2020 notes)

These are observations about the original course code, not tasks to silently “fix” in the game
binary. The portable examples implement the **intended** formulas and call the quirks out when a
test would otherwise lie.

## Include guard does not match

`define.h` opens with `#ifndef MYDEFINE` and then `#define MYDIFINE`. A second include would not
be skipped. Today every translation unit includes it once, so the typo is latent.

## `LIFE` macro ends with a semicolon

`#define LIFE 5;` expands `int life = LIFE;` into `int life = 5;;`. Harmless. `life = LIFE;`
inside branches becomes `life = 5;;` as well. Do not write `if (life == LIFE)` expecting a raw
token.

## Enemy / coin copy loops are `<=`

`createEnemy`, `createCoin`, and `createFood` use `while (i <= sizeof(arr)/sizeof(arr[0]))`.
The last iteration reads one element past the stack array. It has survived because the leftover
bytes are often zeroes and `MAP`/`ENEMY` slots are pre-cleared. The catalog example copies with
`<`, which is what the author meant.

World 3’s `createMap` uses the same `<= sizeof` pattern on a 30-element `Map` array and then
assigns `map[i] = m[i]` — that last iteration is undefined.

## `MAP_NUMBER == 30` truncates worlds 1 and 2

World 1 authors 32 tiles (last two water sprites lost). World 2 authors 36 tiles. The first 30
are solids and the cloud staircase; background grass, the **goal sign**, and the four trees never
load. World 2 still clears because `isEnding` is a distance check, not a sprite touch.

## Friction factor of two

`k` is ±2, not ±1, so deceleration is `2 * V_MAX / T` rather than `V_MAX / T`. The comments
still say “time to max speed is T”. The feel is “stops twice as fast as the comment claims”.

## `hitMap(..., world)` vs `hitMap(..., 1)`

World-3 lethality is only applied when the caller passes the real `world`. Horizontal wall
resolution and enemy foot tests pass `1` on purpose so a pipe bump does not kill during those
queries. Vertical landing and the main overlap test pass `world` and **can** kill.

## Held keys and `_kbhit`

`GetCommand` is skipped when the console keyboard buffer is empty, but `GetAsyncKeyState` is the
actual source of truth when it does run. The last mask sticks. After pause, `key` may remain a
`VIR_*` bit until another `_kbhit`. That can make the first frame after resume ignore WASD.

## Recursion on “返回”

Home intro / directions call `gameStart()` again instead of looping. Repeated open/close of those
pages grows the stack. Deep enough clicking will overflow.

## MessageBox argument order

Load-failure dialog uses `MessageBox(..., "提醒", "存档缺失，请检查存档", 1)` so the short word
is the body and the sentence is the caption.

## Unreachable `closegraph`

`main`’s `while (true)` never breaks. `mciSendString("close all")` and `closegraph()` are dead.
Exit from the home menu uses `exit(0)` and skips them too.

## Hero spawn uses `X0` for both axes

`myHero.y = X0` (64), not `Y0` (96). The cat starts one tile higher than the comment in `define.h`
describes, then falls onto the ground because `isFly` starts true.

## Off-screen bullet vs camera

Bullets store **screen** x. `MAX_DISTANCE` is 480 in screen space, so you cannot snipe far into
a scrolled world. Hits against tiles convert through `x0`, which is why a bullet can explode on
geometry the sprite is no longer overlapping.

## Score pop coordinates mix spaces

Coins are stored in tile units; food is stored in pixels. `setScorePos` is called with both.
The pop sprite treats the pair as tiles (`x * WIDTH`), so a food pickup flash appears at a
wildly wrong place. World 3’s weapon at (10, 10) pixels happens to look almost like tile (10, 10)
if you ignore the mismatch — coincidence.

## `random` is unseeded

World 3 pipe heights call `rand()` through `random(a,b)` with no `srand`. Successive runs in the
same process (restart) keep advancing the generator; a fresh process repeats the same sequence
on a given CRT.

# Game loop

All of the run-state machine lives in `MaoLiAo/main.cpp`. There is one `while (true)` after the title screen. Nothing in `Role` or `Scene` decides “go to world 2”; they only raise `died` / `passed` flags.

## Startup

```
initgraph(512, 384)
SetWindowText(..., "猫里奥 V2.0")
Control::gameStart()          // blocking; may set world from gameRecord.dat
Role(world)
Scene(world)
open MCI aliases, play music_bg repeat
```

`life` is a global, initialised to `LIFE` (the macro is `5;`, so the declaration is `int life = 5;`). `world` starts at `1` unless the title-screen “读档” path overwrites it.

## One play frame

```
key = Control::getKey()

if key == VIR_RESTART:
    rebuild Scene(world), Role(world); rewind bgm
if key == VIR_HOME:
    stop bgm; life = 5; world = 1
    gameStart(); rebuild Role + Scene; play bgm

Role::action(key, &scene, world)
Scene::action(&role)

if role.isDied():
    freeze a frame, wait 3.5 s, life--
    if life == 0: game over page → title → world 1
    else:         show remaining lives → rebuild this world

if role.isPassed():
    stop bgm
    if world == 3: win fanfare → passed-all page → title → world 1
    else:          win sting → world++ → “LEVEL: N” page → rebuild

BeginBatchDraw
    scene.show()
    role.show()
    HUD score + level
EndBatchDraw
Timer::Sleep(10)
```

`VIR_*` values are **not** keyboard bits. The pause overlay writes them into `Control::key` and `getKey()` returns that stored value on later frames until a real key replaces it. `main` therefore treats `VIR_RESTART` / `VIR_HOME` as sticky menu results, not as `GetAsyncKeyState` codes.

## Death

`Role::action` sets `myHero.died` when:

- the hero's screen `y` is greater than `YSIZE` (fell off);
- the hero overlaps an enemy and `vY <= 0` (walked into them, or jumped up into them);
- world 3, the hero overlaps a solid tile that is not a cloud (`id != 2`), and `isShoot` is still false.

`main` then batch-draws one last pose (the death frame in `role.bmp` column 2), waits 3500 ms, and decrements `life`. `showDied(life)` blits 1–4 cat faces for the lives **remaining**, not the lives just lost. With 5 starting lives you can see at most 4 faces; the fifth death goes to game over instead.

After a non-final death the same `world` is reconstructed. Score resets because `Role`'s constructor zeroes it. The weapon flag resets too.

## Clear

`Role::action` sets `ending` when `Scene::isEnding(x - x0)` is true (the hero has walked far enough in world space). While `ending` is set:

- left input is ignored;
- the hero is forced to keep accelerating right (`CMD_RIGHT` is treated as held);
- the camera rail still applies until `x` exceeds `XSIZE` (the sprite walks off the right of the window).

Then `passed` becomes true and `main` handles the intermission. World 3 plays `music_win` (6.5 s) and then `music_passedAll` under the credits page (7.8 s). Worlds 1 and 2 only play `music_win` and show `LEVEL: N` for 2 seconds after a 6.5 s wait.

## Restart and home

Both paths build brand-new `Role` and `Scene` objects with the assignment operator (compiler-generated memberwise copy of the freshly constructed temporary). EasyX `IMAGE` handles are copied by value; the new constructor has already `loadimage`d into the temporary, so the assignment is “good enough” for this codebase even though `IMAGE` is not a carefully designed value type.

`VIR_HOME` also resets `life` and `world` before re-entering the title loop. `VIR_RESTART` does not.

## Tick rate

`TIME` is `0.01` seconds. The integrator and the frame sleep both use that constant. Sprite frame counters advance with `TIME * n` (coins `* 7`, enemies `* 5`, bombs `* 10`). If a machine cannot keep 100 Hz, motion still assumes it did — there is no catch-up or drop.

`Timer::Sleep` uses `QueryPerformanceCounter`. It targets an **absolute** timeline (`m_oldclk += ms * freq`) so a slightly late wake still keeps the next deadline on the original cadence, unless the frame already overran, in which case it snaps `m_oldclk` forward.

## Music around the loop

| Event | MCI |
| --- | --- |
| Enter play | `play music_bg repeat` |
| Restart current world | `play music_bg from 0` |
| Death pose | `stop music_bg` |
| After death / game over / title | `play music_bg from 0` |
| Clear | `stop music_bg`, then `music_win` (and `music_passedAll` on the last world) |

Gameplay one-shots (jump, coin, shot) are fired from `Role` and are independent of this table.

## Why the loop is a single file

`life` and `world` need to outlive any one `Role`. Putting the state machine in `main` keeps `Role` to “simulate this world” and `Control` to “talk to the player”. The portable examples follow the same idea: they expose flags and distances, not a full application loop.

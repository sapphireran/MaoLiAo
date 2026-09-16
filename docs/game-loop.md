# Game loop

`main.cpp` owns the life machine. After `initgraph(XSIZE, YSIZE)` it sets the window title to `猫里奥 V2.0`, shows the blocking title screen, constructs `Role` / `Scene` for `world`, opens four music aliases, and enters `while (true)`.

## Per-tick sequence

```
key = gameCtrl.getKey()
if key == VIR_RESTART:  rebuild Scene(world), Role(world); rewind bgm
if key == VIR_HOME:     stop bgm; life = LIFE; world = 1; title; rebuild; play bgm

gameRole.action(key, &gameScene, world)
gameScene.action(&gameRole)

if hero died:
    freeze one batched frame (scene + role + HUD)
    sleep 3500 ms
    life--
    if life == 0:  play music_end; showGameOver; reset life & world; title
    else:          showDied(life)   # remaining lives as cat icons
    rebuild Role/Scene; play bgm

if hero passed:
    stop bgm
    if world == 3:  win fanfare; showPassedAll; reset; title
    else:            play music_win; sleep 6500; world++; showPassed(world)
    rebuild Role/Scene; play bgm

BeginBatchDraw
  scene.show(); role.show(); HUD
EndBatchDraw
Timer.Sleep(10 ms)
```

`getKey()` returns a **sticky** bitmask. `Control::key` is only updated when `_kbhit()` is true, so a held `A` continues to report `CMD_LEFT` even on ticks where the console keyboard buffer is empty. Escape latches `CMD_ESC` and then `pauseClick()` overwrites `key` with a `VIR_*` value.

## Life and world transitions

| Event | `life` | `world` | Objects |
| --- | --- | --- | --- |
| Title “开始” | unchanged | as-is (1 unless load) | already constructed after return |
| Death, lives remain | decremented | same | reconstructed |
| Game over | reset to 5 | 1 | title, then reconstructed |
| Stage clear, world 1 or 2 | unchanged | `world++` | reconstructed for the new index |
| Stage clear, world 3 | reset to 5 | 1 | title, then reconstructed |
| Pause “重新开始” | unchanged | same | reconstructed |
| Pause “退出游戏” | reset to 5 | 1 | title path (`VIR_HOME`) |

The pause label **退出游戏** is wired to `VIR_HOME`, not `exit(0)`. Only the title-screen **退出** button calls `exit(0)`.

## Ending walk-off

`Scene::isEnding(distance)` uses camera-space hero x (`-x0 + x`):

| World | Distance threshold | Tiles (÷32) |
| --- | --- | --- |
| 1 | `94 * WIDTH` | 94 |
| 2 | `104 * WIDTH` | 104 |
| 3 | `94 * WIDTH` | 94 |

When true, `Hero.ending` is set. `action` then forces `CMD_RIGHT`-like acceleration every tick (`KEY & CMD_RIGHT || myHero.ending`) and ignores left / jump (jump is also blocked by `ending` except the world-3 air-jump clause still checks `CMD_UP && world == 3` first — world 3 jump is allowed even during ending if the first condition’s `world == 3` branch fires).

Once `Hero.x > XSIZE` (512), `passed` becomes true and `main` handles the cutscene.

## Music aliases

Opened in `main` (looping bed plus stingers):

| Alias | File |
| --- | --- |
| `music_bg` | `res\背景音乐.mp3` |
| `music_win` | `res\胜利.mp3` |
| `music_passedAll` | `res\通关.mp3` |
| `music_end` | `res\游戏结束.mp3` |

Opened in `Role::Role` (re-opened every reconstruct):

| Alias | File |
| --- | --- |
| `music_died` | `res\死亡1.mp3` |
| `music_jump` | `res\跳.mp3` |
| `music_coin` | `res\金币.mp3` |
| `music_tread` | `res\踩敌人.mp3` |
| `music_getWeapon` | `res\吃到武器.mp3` |
| `music_bullet` | `res\子弹.mp3` |
| `music_boom` | `res\子弹撞墙.mp3` |
| `music_boom2` | `res\子弹打到敌人.mp3` |

## Frame budget

`TIME` is `0.01` seconds. Physics integrates with that `dt`. The timer sleeps 10 ms after the draw. There is no delta-time measurement: a slow frame still steps `0.01` s of physics, then waits, so the game slows down rather than skipping.

`BeginBatchDraw` / `EndBatchDraw` (EasyX) compose the frame off-screen. Pause and title UIs mix `BeginBatchDraw` with `FlushBatchDraw` because they are nested modal loops.

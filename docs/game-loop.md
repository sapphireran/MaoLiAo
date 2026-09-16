# Game loop and input

Everything interactive is in `MaoLiAo/main.cpp` and `MaoLiAo/control.cpp`.

## Startup

```cpp
initgraph(XSIZE, YSIZE);          // 512 × 384
SetWindowText(hwnd, "猫里奥 V2.0");
Control gameCtrl;
gameCtrl.gameStart();             // blocks on the mouse menu
Role  gameRole(world);
Scene gameScene(world);
// open four MCI aliases, loop the BGM
```

`world` starts at `1`. `life` starts at `LIFE` (the macro is `5;` — see [known-quirks.md](known-quirks.md)).

`gameStart()` does not return until the player clicks **开始** or a valid **读档**. **退出** calls `exit(0)`. **介绍** / **指导** redraw in-place and can recurse back into `gameStart()`.

## Live loop

The `while (true)` body is the whole runtime. There is no fixed-update accumulator; `Timer::Sleep` tries to keep a 10 ms cadence after each frame.

### 1. Input

`Control::getKey()`:

1. If `_kbhit()`, replace `key` with `GetCommand()`.
2. `GetCommand()` ORs `GetAsyncKeyState` bits:
   - `A` → `CMD_LEFT` (1)
   - `D` → `CMD_RIGHT` (2)
   - `W` or `K` → `CMD_UP` (4)
   - `S` → `CMD_DOWN` (8) — accepted, unused by `Role`
   - `J` → `CMD_SHOOT` (16)
   - `Esc` → `CMD_ESC` (32)
3. If `CMD_ESC` is set, `pauseClick()` **blocks** on `GetMouseMsg` and may overwrite `key` with a virtual command:
   - `VIR_RETURN` (64) — dismiss pause
   - `VIR_RESTART` (128) — rebuild this world
   - `VIR_HOME` (256) — back to `gameStart()`
   - Save writes `world` as ASCII to `gameRecord.dat` and then returns `VIR_RETURN`

`getKey()` returns the **sticky** last mask. If you tap `D` and then release, `_kbhit()` is false and `key` stays `CMD_RIGHT` until another key event arrives. That is why the hero can keep running after the key is up. A new key-down (including a key you do not care about) is required to refresh the mask.

### 2. Virtual commands in `main`

```cpp
if (key == VIR_RESTART) { gameScene = Scene(world); gameRole = Role(world); }
else if (key == VIR_HOME) {
    life = LIFE; world = 1;
    gameCtrl.gameStart();
    gameRole = Role(world);
    gameScene = Scene(world);
}
```

Restart does **not** restore lives. Home does.

### 3. Simulate

```cpp
gameRole.action(key, &gameScene, world);
gameScene.action(&gameRole);
```

Order matters. The hero moves first (and may set `ending` / `passed` / `died`). The scene then copies `hero.x0` into `xMap` and parallax-scrolls the sky only when the hero is pinned to `XRIGHT` and still has `vX > 0`.

### 4. Death

`Role::isDied()` is `Hero::died`.

Causes (all in `role.cpp`):

- `y > YSIZE` while falling
- `hitEnemy` while `vY <= 0` (side or head bump)
- World 3: `hitMap` on any solid tile whose `id != 2`, unless `isShoot` is already true

On death, `main` freezes the last frame for 3.5 s, then `--life`. At 0 it plays `music_end`, shows the Game Over slice of `home.bmp` for 6.5 s, resets `life` and `world`, and returns to the start menu. Otherwise it shows remaining lives as death-pose sprites and rebuilds the current world.

### 5. Clear

`Role::isPassed()` is `Hero::passed`, set when `hero.x > XSIZE` (walked off the right of the 512 px window). That only happens after `Scene::isEnding` has flipped `ending`, which forces `CMD_RIGHT` and disables jump/left.

Ending distances (hero map-x in pixels):

| World | Predicate in `Scene::isEnding` | Pixels |
| --- | --- | --- |
| 1 | `distance > 94 * WIDTH` | 3008 |
| 2 | `distance > 104 * WIDTH` | 3328 |
| 3 | `distance > 94 * WIDTH` | 3008 |

World 3 then plays the win sting, waits 6.5 s, plays the credits track, shows `showPassedAll()` (7.8 s), and resets to world 1 + the start menu. Worlds 1 and 2 increment `world` and call `showPassed`.

### 6. Draw

`BeginBatchDraw` / `EndBatchDraw` wrap `scene.show()`, `role.show()`, score, and level. `role.show()` also steps bullets, so projectile motion is tied to the display path. If you ever skip drawing, shots freeze.

## Save file

`gameRecord.dat` is a single integer: the current `world`.

Write (pause → 进行存档):

```cpp
fopen_s(&fp, "gameRecord.dat", "w");
fprintf_s(fp, "%d", world);
```

Read (menu → 读档):

```cpp
fscanf_s(fp, "%d", &flag);
if (flag >= 4 || flag <= 0) MessageBox(...);
else world = flag;
```

There is no checksum, no life count, no score, and no hero position. A missing file is not handled: `fopen_s` + `fscanf_s` on failure is undefined in this code. The checkout includes a sample `MaoLiAo/gameRecord.dat` containing `1`.

## Music aliases

Opened once in `main` or `Role::Role`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | loop during play |
| `music_win` | `res\胜利.mp3` | level clear |
| `music_passedAll` | `res\通关.mp3` | credits |
| `music_end` | `res\游戏结束.mp3` | game over |
| `music_died` | `res\死亡1.mp3` | death |
| `music_jump` | `res\跳.mp3` | jump |
| `music_coin` | `res\金币.mp3` | coin |
| `music_tread` | `res\踩敌人.mp3` | stomp |
| `music_getWeapon` | `res\吃到武器.mp3` | pickup |
| `music_bullet` | `res\子弹.mp3` | fire |
| `music_boom` | `res\子弹撞墙.mp3` | bullet vs tile |
| `music_boom2` | `res\子弹打到敌人.mp3` | bullet vs enemy |

All paths are relative to the process working directory, not the `.exe` location.

## HUD and interstitial art

`res\home.bmp` is loaded as `XSIZE × 5*YSIZE` and shifted:

| Call | `putimage` y | Meaning |
| --- | --- | --- |
| `gameStart` | 0 | title / menu backdrop |
| `showGameOver` | `-YSIZE` | game over |
| `showPassed` / `showDied` | `-2*YSIZE` | “next level” / death |
| `showPassedAll` | `-3*YSIZE` | credits |

Score and level are `outtextxy` with the Cooper typeface.

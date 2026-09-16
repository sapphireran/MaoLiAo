# Resources

The Visual Studio project does not list a `res/` filter. The shipped
tree expected a `MaoLiAo/res/` folder next to the exe (or copied to
the debug/working directory). Paths are hard-coded with backslashes.

If `res/` is absent locally, the game still **links**; `loadimage` /
MCI open fail at runtime and surfaces stay blank.

## Images (`loadimage`)

| File | Loaded by | Usage |
| --- | --- | --- |
| `res\mapsky.bmp` | `Scene` | Sky strip, `512 × 4*384`. Row `-(world-1)*YSIZE` picks the climate. Looped on X. |
| `res\map.bmp` | `Scene` | 32-px-tall tile rows, id 1–10 (+ pipe variants). |
| `res\scenery.bmp` | `Scene` | Animated decor (tree, water, grass). 3×2 tile frames, 2-frame swing. |
| `res\ani.bmp` | `Scene`, `Role` | Coins, score pop, food, enemies, bombs, bullets. Rows selected by `iframe`. |
| `res\role.bmp` | `Role`, `Control::showDied` | Hero walk (2 frames × 2 facings) + death frame at column 2. |
| `res\home.bmp` | `Control` | Title / interstitials. `512 × 5*384`. Y offsets: 0 title, -YSIZE game over, -2YSIZE level card / death, -3YSIZE credits. |

Mask convention: for a sprite of height `H`, the color sheet sits at
some `y0` and the AND mask at `y0 + H` (hero) or the reverse order
used by coins (`mask` at `9*HEIGHT`, color at `8*HEIGHT`). Pair
`SRCAND` then `SRCPAINT`.

Hero sheet layout (32×32 cells):

```
row 0 (color):  [walk0] [walk1] [dead] [walk0 flip] [walk1 flip]
row 1 (mask):   matching columns
```

`Role::show` picks column `hero_iframe-1` (1 or 2 → column 0 or 1)
and adds `3*WIDTH` when `turn == -1`.

## Audio (`mciSendString` aliases)

Opened in `main`:

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | Loop during play |
| `music_win` | `res\胜利.mp3` | World clear |
| `music_passedAll` | `res\通关.mp3` | After world 3 win |
| `music_end` | `res\游戏结束.mp3` | Life == 0 |

Opened in `Role` constructor (again every rebuild):

| Alias | File | When |
| --- | --- | --- |
| `music_died` | `res\死亡1.mp3` | Death |
| `music_jump` | `res\跳.mp3` | Jump impulse |
| `music_coin` | `res\金币.mp3` | Coin |
| `music_tread` | `res\踩敌人.mp3` | Stomp |
| `music_getWeapon` | `res\吃到武器.mp3` | Mushroom |
| `music_bullet` | `res\子弹.mp3` | Shot |
| `music_boom` | `res\子弹撞墙.mp3` | Bullet vs tile |
| `music_boom2` | `res\子弹打到敌人.mp3` | Bullet vs enemy |

MCI alias names are not closed between `Role` reconstructions. Re-open
of the same alias can fail silently; playback still often works if the
first open succeeded.

## Icon

`MaoLiAo.ico` + `MaoLiAo.rc` / `IDI_ICON1` (101) give the process its
window icon. Unrelated to gameplay.

## Save file (not under `res/`)

`gameRecord.dat` — see [controls.md](controls.md). A sample with
`world = 1` may already sit in `MaoLiAo/gameRecord.dat`.

## Working directory checklist (Windows)

From the `MaoLiAo` project directory:

```
MaoLiAo.exe
res\
  mapsky.bmp  map.bmp  scenery.bmp  ani.bmp  role.bmp  home.bmp
  背景音乐.mp3  胜利.mp3  通关.mp3  游戏结束.mp3
  死亡1.mp3  跳.mp3  金币.mp3  踩敌人.mp3
  吃到武器.mp3  子弹.mp3  子弹撞墙.mp3  子弹打到敌人.mp3
```

Debug builds look in the cwd configured in
Project → Properties → Debugging → Working Directory. Set that to
`$(ProjectDir)` so `res\` resolves.

# Sprites and audio

Assets live in `MaoLiAo/res/`. Paths in source use backslashes (`res\\role.bmp`).

## Bitmaps

| File | Used by | Layout (as sampled in code) |
| --- | --- | --- |
| `role.bmp` | Hero, death HUD | 32×32 cells. Walk right: frames 0–1 on row 0 (color) + row 1 (mask). Walk left: frames 3–4. Death: cell `(2, 0)` color / `(2, 1)` mask. |
| `ani.bmp` | Enemies, bombs, bullets, coins, food, score pops | Shared atlas. Enemy walk: frames 0–1, rows 0 (color) / 1 (mask). Bullets: rows 2–3. Bombs: 64×64 cells on rows 4–6. Coins: rows 8–9, 4 frames. Score pop: rows 10–11. Food: row 12, 52×25 cells. |
| `map.bmp` | Solid tiles | Vertical strip, one 32-high row per id (`y0 = (id-1)*HEIGHT`). Ids 7–10 blit 64-wide pipe art. |
| `scenery.bmp` | Ids 11–14 | 96×64 frames, two-frame sway. `y0 = (id-11)*4*HEIGHT`. Mask row under color row. |
| `mapsky.bmp` | Parallax sky | Loaded as 512 × 1536 (4 × 384). World n uses strip `n-1`. Loops horizontally when `xBg <= -width`. |
| `home.bmp` | Menus / interstitials | Loaded as 512 × 1920 (5 × 384). See [input-and-ui.md](input-and-ui.md). |

Transparent blits are the EasyX two-pass pattern:

```cpp
putimage(x, y, w, h, &img, srcX, srcYMask, SRCAND);
putimage(x, y, w, h, &img, srcX, srcYColor, SRCPAINT);
```

Walk-cycle index for the hero:

```
rolePos = -x0 + x
iframe = 2  if rolePos/10 % 2 == 0 && rolePos/10 % 4 != 0
iframe = 1  if rolePos/10 % 4 == 0
```

Enemies advance `enemy_iframe += TIME*5` and wrap at 3 (frames 1 and 2).

## Audio (MCI)

`#pragma comment(lib, "Winmm.lib")` in `main.cpp` and `role.cpp`. Aliases are opened with `mciSendString("open res\\….mp3 alias …")`.

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | 背景音乐.mp3 | loop during play |
| `music_win` | 胜利.mp3 | stage clear |
| `music_passedAll` | 通关.mp3 | after world 3, following 胜利 |
| `music_end` | 游戏结束.mp3 | life == 0 |
| `music_died` | 死亡1.mp3 | death flag |
| `music_jump` | 跳.mp3 | jump impulse |
| `music_coin` | 金币.mp3 | coin |
| `music_tread` | 踩敌人.mp3 | stomp |
| `music_getWeapon` | 吃到武器.mp3 | food |
| `music_bullet` | 子弹.mp3 | shot |
| `music_boom` | 子弹撞墙.mp3 | bullet vs tile / range |
| `music_boom2` | 子弹打到敌人.mp3 | bullet vs enemy |

Restart / death reconstruct `Role`, which `open`s the SFX aliases again. Background aliases live in `main` and are `play … from 0` or `stop` / `play repeat`.

`close all` is after the infinite loop and never runs.

## Icon

`MaoLiAo.ico` is `IDI_ICON1` in `MaoLiAo.rc` (Chinese Simplified, code page 936).

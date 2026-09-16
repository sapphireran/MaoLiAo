# 08 — Audio

All music and stings go through WinMM `mciSendString`. There is no volume, no mixer, and no streaming abstraction. Each cue is an alias opened from `res\*.mp3`.

## Aliases opened in `main`

| Alias | File | When |
| --- | --- | --- |
| `music_bg` | `res\背景音乐.mp3` | Loop during play (`play ... repeat`) |
| `music_win` | `res\胜利.mp3` | Stage clear |
| `music_passedAll` | `res\通关.mp3` | After the win sting on world 3 |
| `music_end` | `res\游戏结束.mp3` | Life count hit zero |

`close all` is after the infinite loop and therefore never reached. Process exit releases the devices.

## Aliases opened in `Role::Role`

| Alias | File | Trigger |
| --- | --- | --- |
| `music_died` | `res\死亡1.mp3` | Pit or enemy touch |
| `music_jump` | `res\跳.mp3` | Every accepted jump |
| `music_coin` | `res\金币.mp3` | Coin pickup |
| `music_tread` | `res\踩敌人.mp3` | Stomp |
| `music_getWeapon` | `res\吃到武器.mp3` | Mushroom |
| `music_bullet` | `res\子弹.mp3` | Shot fired |
| `music_boom` | `res\子弹撞墙.mp3` | Bullet vs tile / max distance |
| `music_boom2` | `res\子弹打到敌人.mp3` | Bullet vs enemy |

Plays use `from 0` so a second coin during the first sting restarts the sample.

## Sequencing with the loop

Death and clear **stop** `music_bg` before the long `Timer::Sleep` / `show*` calls, then `play music_bg from 0` after the new `Role`/`Scene` exist. Restart from pause rewinds BGM without stopping it first (`play music_bg from 0`). Home from pause stops BGM, runs the menu in silence, then starts it again.

World 3 all-clear is the only place two music aliases are chained: 6.5 s of `music_win`, then `music_passedAll` overlapping `showPassedAll`’s 7.8 s card.

## Missing `res` in git

The checked-in tree does not include `res\`. Bitmaps, the icon (`MaoLiAo.ico` is listed in the vcxproj), and MP3s are local course artifacts. The game will fail `loadimage` / `mciSendString open` without them. Portable examples do not need the folder.

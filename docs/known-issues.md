# Known issues (as of the 2020 V2.0 sources)

These are observations from reading the tree, not a promise to fix them. The headless examples copy the **intended** formulas and the **literal** level tables; they do not reproduce every overrun.

## Include / macro hygiene

- `define.h` uses `#pragma once` **and** an include guard whose names do not match: `#ifndef MYDEFINE` then `#define MYDIFINE`. The pragma is what actually prevents a double include.
- `#define LIFE 5;` and `#define F TIME*0.3;` include a semicolon in the replacement list.
- `inertia.cpp` starts with `#pragma once` in a `.cpp` file (harmless, odd).
- `role.cpp` includes `inertia.h` twice.

## Array overruns in world builders

Several `while (i <= sizeof(arr)/sizeof(arr[0]))` loops run one past the last initializer:

- `Scene::createCoin` (all three worlds)
- `Scene::createFood` (all three worlds)
- `Role::createEnemy` (all three worlds)
- `Scene::createMap` world 3 (`i <= sizeof(m)/sizeof(m[0])`)

World 1–2 `createMap` uses `i < MAP_NUMBER` (30) instead. That silently drops trailing records:

- World 1 authors **32** tiles; water at x 71 and 75 never loads.
- World 2 authors **36** tiles; the copy stops after the cloud staircase, so the **goal sign** at `(111, 1)`, the grass tuft at `(42, 8)`, and all four trees never load. The stage is still clearable because `isEnding` is a distance check (104 tiles), not a sprite.

World 2’s comment `//???存在问题` sits on the copy loop.

## Collision and world 3

Horizontal blocking calls `hitMap(..., world=1)` even on world 3, so wall slides do not apply the “touch = death” rule. The lethal test is only in the `hitMap` that receives the real `world` (vertical probes and the in-loop body check).

World 3 food at `{10, 10}` is a 10×10 pixel box near the top-left, not a tile coordinate.

## Input latch

`Control::getKey` only refreshes bits when `_kbhit()` is true. The last command remains active after key-up until another keyboard event. This feels like sticky movement and can fire an extra jump or shot.

`CMD_DOWN` and the empty `if (KEY & CMD_ESC)` / `if (KEY & CMD_DOWN)` branches in `Role::action` do nothing.

## Save / UI

- Save is a single integer. A missing file is not handled before `fscanf_s`; a first-run “读档” click can crash or throw a runtime error if `gameRecord.dat` is absent from the cwd.
- Pause item 3 is labeled “退出游戏” but implements `VIR_HOME` (title), matching the comment “主菜单”.
- `showDied` has no `life == 0` or `life == 5` branch.
- Title “返回” from intro/guide sets `_HOME = false` and recurses into `gameStart()`, which is easy to stack if you bounce the menus.

## Audio / lifetime

`main` never leaves the `while (true)` loop, so `mciSendString("close all")` and `closegraph()` are dead. Alt-F4 is the real exit (plus the title **退出** `exit(0)` path, which skips MCI cleanup too).

`Role` re-`open`s the same aliases every construct.

## Timer

`Timer::Sleep` is a wait-until-deadline helper. If a frame overruns, it snaps `m_oldclk` forward and does not catch up. Physics still uses a constant `TIME`, so wall-clock 100 Hz is best-effort only.

## Charset

Debug Win32 is `MultiByte`. Release configs are `Unicode`. Chinese string literals and `res\\….mp3` names are not `_T()`-wrapped everywhere. Build Debug Win32 unless you migrate the UI strings.

## Example / docs contract

If you change a literal in `scene.cpp` or `role.cpp`, update `examples/maoliao_sim/worlds.py` and the counts in `examples/tests/test_all.py`. The tests fail when the extract drifts.

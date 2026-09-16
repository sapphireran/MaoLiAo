# Examples lab

Python ports of the 2020 EasyX systems in `MaoLiAo/`. Nothing here links
against `graphics.h`; the point is to poke the numbers on a Linux box.

```
examples/
  run_lab.py              CLI (python3 -m examples.run_lab)
  data/                   World 1–2 tile/actor tables copied from scene.cpp
  maoliao_lab/
    constants.py          define.h
    inertia.py            Inertia::move
    jump.py               jump arc
    friction.py           a1 = k G u
    aabb.py               Role::isHit / hitMap
    camera.py             XRIGHT pin + parallax
    commands.py           CMD_* bits + sticky _kbhit cache
    savefile.py           gameRecord.dat
    tiles.py              id catalog + u
    worlds.py             MAP_NUMBER=30 cap
    world3.py             pipe-column generator
    sprites.py            sheet coordinates, MCI names
    ascii_map.py          text dump
    loop.py               toy main-loop tick
  tests/                  unittest, no extra deps
```

## Run

From the repo root:

```bash
python3 -m unittest discover -s examples/tests -v
python3 -m examples.run_lab constants
python3 -m examples.run_lab jump-table --every 10
python3 -m examples.run_lab ascii-map --world 1
python3 -m examples.run_lab ascii-map --world 2 --solids-only
python3 -m examples.run_lab summary --world 2
python3 -m examples.run_lab friction
python3 -m examples.run_lab tick --keys D --frames 80
python3 -m examples.run_lab tick --keys W --world 3 --frames 40
python3 -m examples.run_lab camera --frames 30
python3 -m examples.run_lab save
python3 -m examples.run_lab sprites
```

World 2's `summary` should show `goal_present: false` and six dropped
records — that is `MAP_NUMBER` truncating `createMap`, not a lab bug.
See [docs/quirks.md](../docs/quirks.md).

## What is deliberately not ported

- EasyX `putimage` / mouse menus / MP3 playback
- The off-by-one `i <= sizeof(arr)` over-reads (the lab copies the *intended*
  arrays, then applies the 30-slot cap)
- CRT `rand()` pipe rolls (world 3 uses `LAB_PIPE_HEIGHTS` unless you pass
  your own)

C++ remains the authority if a comment and a function disagree.

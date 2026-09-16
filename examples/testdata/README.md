# Reference dumps

`world1_ascii.txt` and `world2_ascii.txt` are the raster produced by
`04_level_preview` with `MAP_NUMBER = 30` (the same cap as `scene.h`).
`04` fails the run if those files drift. `jump_apex.txt` records the
standing-jump numbers from `01`. Regenerate after changing
`common/map_data.cpp`:

```bash
cd examples
make
./build/04_level_preview | sed -n '/ASCII/,/^finish/p'
```

`constants.txt` is the numeric sheet from `define.h` so a diff on that
file is obvious when macros move.

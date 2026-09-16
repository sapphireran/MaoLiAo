# Build (personal Windows game)

The playable target is `MaoLiAo/MaoLiAo.vcxproj` inside `MaoLiAo.sln`.

| | |
| --- | --- |
| Toolset | v142 (Visual Studio 2019) |
| Windows SDK | 10.0 |
| Configurations | Debug/Release × Win32/x64 |
| Charset | **MultiByte** on Debug, **Unicode** on Release |
| Subsystem | Console |
| Extra lib | `Winmm.lib` (pragma) |
| Graphics | EasyX `graphics.h` |

Debug MultiByte matches the narrow `outtextxy` / `loadimage("res\\...")` strings. A Release
Unicode build is the configuration most likely to regress the 2019 “EasyX 字符集识别失败” bug
fixed in commit `a33467f`. Prefer Debug while iterating.

Resources: `MaoLiAo.rc` embeds `MaoLiAo.ico` as `IDI_ICON1`. Bitmaps and mp3s are **not** compiled
in; they are loaded at runtime from `res\`.

## Portable examples (this Linux environment)

The EasyX game cannot link here. The reconstruction programs under `examples/` are C++17 and only
need `g++`:

```bash
cd examples
make          # build every program into examples/build/
make test     # build and run
make clean
```

`examples/run_all.sh` is the same as `make test` and prints a per-binary summary.

No network, no EasyX, no company SDK. The examples only encode numbers that already live in this
personal repository.

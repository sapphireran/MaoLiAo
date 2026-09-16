# 11 — Build

## Original Windows target

- Visual Studio 2019 (toolset `v142` in `MaoLiAo.vcxproj`)
- Windows SDK 10.0
- [EasyX](https://easyx.cn/) providing `graphics.h` / `EasyXa.lib` (or the Unicode `EasyXw.lib`)
- WinMM (`#pragma comment(lib, "Winmm.lib")` in `main.cpp` and `role.cpp`)

Configurations: Debug/Release × Win32/x64. Debug uses **MultiByte**. Release uses **Unicode**. The source is written in the MultiByte / `_T` mix that Debug expects. If a Release Unicode build explodes on string literals, use Debug Win32 first.

Character set in the home/pause UI is Simplified Chinese (`MaoLiAo.rc` `LANG_CHINESE`, code page 936).

### Run directory

`loadimage("res\\...")` and `fopen("gameRecord.dat")` are relative paths. Set the VS debugger working directory to the folder that contains `res\` and, if you want a default load, `gameRecord.dat`. Shipping a zip: `.exe` + `res\` + optional `gameRecord.dat`.

### Icon

`MaoLiAo.rc` references `MaoLiAo.ico` as `IDI_ICON1`. The file is listed in the vcxproj but is not required for the portable examples.

## What will not build here

This Cloud Agent workspace is Linux. There is no MSVC, no EasyX, and no Win32 GDI. Do not expect `MaoLiAo.sln` to compile in this environment.

## Portable examples (Linux / macOS / any g++)

```text
examples/
  common/maoliao_core.{h,cpp}   extracted rules
  01_inertia/ ...
  08_score_rules/
  Makefile
```

```bash
make -C examples
make -C examples test
```

The examples re-implement:

- `Inertia::move`
- jump launch velocity
- four-corner hit
- command bit packing
- save-file accept / reject
- camera pin + parallax step
- `isEnding` thresholds
- world-3 pipe slots
- score deltas

They do **not** re-implement EasyX blits or MCI.

C++ standard: **C++17**. Compiler flags in the Makefile: `-std=c++17 -Wall -Wextra -Wpedantic`.

## Regenerating after you change `define.h`

The core header `examples/common/maoliao_core.h` copies the numeric values, not `#include "../MaoLiAo/define.h"`, because `define.h` pulls no-ops that are fine but also uses `max` / `random` macros that wreck `<algorithm>`. If you retune `G` or `V_MAX`, update both files and the constants table in [12-constants-reference.md](12-constants-reference.md).

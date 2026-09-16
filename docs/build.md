# Build (Windows game)

The game is a Win32 console + EasyX window. The portable examples do not use this path; see [../examples/README.md](../examples/README.md).

## Toolchain

| Piece | Notes |
| --- | --- |
| Visual Studio | Solution format 16 (VS 2019). VS 2022 opens it. Need the C++ desktop workload. |
| Platform toolset | `v142` in `MaoLiAo.vcxproj` |
| Windows SDK | `10.0` |
| EasyX | Install per [easyx.cn/setup](https://easyx.cn/setup). That copies `graphics.h` into the VC include tree. |
| Winmm | `#pragma comment(lib, "Winmm.lib")` in `main.cpp` and `role.cpp` for `mciSendString` |

Character set:

| Config | `CharacterSet` |
| --- | --- |
| Debug Win32 / Debug x64 | MultiByte |
| Release Win32 / Release x64 | Unicode |

Chinese HUD strings are narrow `char` / `"黑体"`. **Debug** matches that. Release Unicode can mis-draw or fail to compile `outtextxy` overloads. Use Debug unless you convert the UI to `TCHAR`.

## Open and run

1. Clone this repo on Windows.
2. Install EasyX for your VS year. The installer lists detected VC versions; click 安装.
3. Place art and music in `MaoLiAo/res/` ([resources.md](resources.md)).
4. Open `MaoLiAo.sln`.
5. Configuration: `Debug`, platform `x86` (Win32) or `x64`.
6. Working directory: the `MaoLiAo` folder (default for the vcxproj when you F5 from that project). Relative paths are `res\\home.bmp`, `gameRecord.dat`, etc.
7. F5. Window title should read `猫里奥 V2.0`, size 512×384.

If `graphics.h` is missing: EasyX is not installed for that toolset. Re-run the EasyX installer and pick the same VS version the solution uses.

If the window is empty or `loadimage` fails: `res/` is missing or the cwd is the solution directory instead of `MaoLiAo/`. In Debugging → Working Directory set `$(ProjectDir)`.

If music is silent: MCI needs the `res\\*.mp3` files. Missing files usually do not crash; aliases just fail to play.

## Manual EasyX

If the installer cannot see VS, follow [docs.easyx.cn setup](https://docs.easyx.cn/zh-cn/setup): copy EasyX `include` + `lib` into the matching VC folder. This project only includes `<graphics.h>`; it does not vendor EasyX.

## What CI cannot do here

This Cloud Agent host is Linux. The `.sln` is not built in this environment. `examples/make test` is the check that does run: it rebuilds the kinematics, AABB, map dump, and save parser from the same constants as `define.h`.

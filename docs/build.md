# Build (Windows / EasyX)

The game is a Visual Studio 2019 Win32 console project (`CharacterSet` =
MultiByte on Debug). It will not link on this Linux checkout: there is no
`graphics.h`.

## Toolchain

- Visual Studio 2019, platform toolset **v142**
- Windows 10 SDK (`WindowsTargetPlatformVersion` 10.0)
- [EasyX](https://easyx.cn/) headers + lib on the include/lib path
- `Winmm.lib` (already `#pragma comment`'d from `main.cpp` and `role.cpp`)

Open `MaoLiAo.sln`. Prefer **Debug | Win32**. Release | Win32 flips the
character set to Unicode, which disagrees with the narrow `"黑体"` string
literals in `control.cpp`.

## Working directory

`loadimage` and `mciSendString` use paths like `"res\\map.bmp"`. Run with
cwd = `MaoLiAo/` (the folder that contains `res/`), or copy `res/` next to
the built exe. `gameRecord.dat` is also created in cwd (pause menu → 进行存档).

## Resource compile

`MaoLiAo.rc` embeds `MaoLiAo.ico` as `IDI_ICON1` (101). The window title is
set in code (`SetWindowText(..., "猫里奥 V2.0")`), not from the rc file.

## What you can do on Linux

Run the Python lab and its tests. That is the supported path in this
environment:

```bash
python3 -m unittest discover -s examples/tests -v
python3 -m examples.run_lab ascii-map --world 1
```

Do not expect `cl.exe`, EasyX, or the MP3 board to exist here.

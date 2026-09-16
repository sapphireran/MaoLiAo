# Building the original game on Windows

The portable programs under `examples/` do not replace this. They only
replay math. The windowed game still needs EasyX and a Microsoft
compiler.

## Toolchain

- Visual Studio 2019 (toolset `v142` in `MaoLiAo.vcxproj`) or newer
  with a toolset you retarget locally.
- EasyX (graphics.h). The 2020 project expected the installer that
  drops `graphics.h` into the VC include path and `EasyXa.lib` /
  `EasyXw.lib` into the lib path.
- Windows SDK 10 (`WindowsTargetPlatformVersion` is `10.0`).

Open `MaoLiAo.sln`. The single project is `MaoLiAo\MaoLiAo.vcxproj`.

## Configurations

| Config | Platform | `CharacterSet` in the vcxproj | Use |
| --- | --- | --- | --- |
| Debug | Win32 | MultiByte | preferred |
| Debug | x64 | MultiByte | preferred |
| Release | Win32 | Unicode | likely broken Chinese text |
| Release | x64 | Unicode | same |

`control.cpp` passes narrow string literals to `outtextxy` / `drawtext`
and `"黑体"` / `"Gill Sans"` / `"Cooper"` / `"Goudy Stout"` face names.
MultiByte + a Chinese system ACP (code page 936) matches how the 2020
author compiled. Unicode builds will mis-encode those literals unless
you add `_T` / `TEXT` everywhere.

`#pragma comment(lib, "Winmm.lib")` is already in `main.cpp` and
`role.cpp`. You do not add WinMM by hand.

## Resources

`MaoLiAo.rc` compiles `MaoLiAo.ico` as `IDI_ICON1`. Language block is
`LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED` with `code_page(936)`.

`gameRecord.dat` is a runtime file, not a compiled resource. The copy
in the repo is a sample save (`3`).

## Debugging

1. Set the debugger working directory to `$(ProjectDir)` so `res\`
   resolves.
2. First paint path: `Control` ctor loads `res\home.bmp` at 512×1920.
   If that blit is a black screen, cwd or EasyX install is wrong.
3. If you hit a crash on 读档, the save file is missing or unreadable.
   The 2020 load path does not check `fopen_s`.

## What you cannot do in this Linux checkout

There is no `graphics.h` here and no Win32. Do not expect `make` in
`MaoLiAo/` to exist. Use `examples/Makefile` to exercise the extracted
logic instead.

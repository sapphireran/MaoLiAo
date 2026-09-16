# 可移植示例

游戏本体要 EasyX，这里把公式和关卡表抽成头文件 [`include/maoliao_model.h`](include/maoliao_model.h)，每个 `src/*_demo.cpp` 自己带 `main`，用 g++ 就能编。

```bash
cd examples
make test
```

产物在 `build/`（已 gitignore）。`make` 只编译，`make test` 编译并按顺序跑完。

| 程序 | 对着源码的哪一块 |
| --- | --- |
| `inertia_demo` | `Inertia::move`、起跳 `sqrt(2gH)`、10 帧自由落体 |
| `aabb_demo` | `Role::isHit` 四顶点、踩地 `y+1`、管道 2× 格 |
| `command_demo` | `CMD_*` / `VIR_*` 位掩码 |
| `save_demo` | `gameRecord.dat` 单整数，非法档返回 false |
| `friction_demo` | 三档 `u = (V_MAX / T) / G`，按 id 分世界 |
| `world_dump` | 第一、二关 ASCII，标出 `MAP_NUMBER` 丢掉的块 |
| `pipe_gen` | 第三关 7 根柱的高度公式和 4 格缝 |
| `loop_trace` | `main.cpp` 生命 / 过关 / 暂停虚拟键 |
| `run_all` | 一页常数摘要 |

`testdata/gameRecord.sample.dat` 和仓库里 `MaoLiAo/gameRecord.dat` 一样，内容是 `1`。

改关卡表时先改 `maoliao_model.h` 里的 `kWorld1Tiles` 等数组，跑 `make test`，再考虑改 `scene.cpp`。循环一律 `< count`，不复制作业里的 `<=` 越界。

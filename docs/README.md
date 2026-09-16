# 猫里奥文档目录

这些笔记对着当前仓库源码写成，方便以后改关卡或把物理单独拿去试验。示例代码在 [`../examples`](../examples/README.md)，不依赖 EasyX。

| 文档 | 讲什么 |
| --- | --- |
| [architecture.md](architecture.md) | 五个类怎么分工，一帧里谁先谁后 |
| [physics.md](physics.md) | 跳跃初速、惯性位移、地面摩擦、速度上限 |
| [collision.md](collision.md) | 四顶点 AABB、管道双倍尺寸、第三关碰砖即死 |
| [input-and-loop.md](input-and-loop.md) | 按键位掩码、暂停虚拟键、生命与过关状态机、存档 |
| [gameplay.md](gameplay.md) | 三关手感、计分、武器蘑菇 |
| [level-design.md](level-design.md) | 地图块字段、tile id、三关表、`MAP_NUMBER` 截断 |
| [assets.md](assets.md) | `res\` 路径、精灵图行、mci 别名 |
| [study-notes.md](study-notes.md) | 头文件宏写错、循环越界、MessageBox 参数对调等 |
| [world-maps.md](world-maps.md) | `world_dump` 打出来的第一、二关 ASCII |

读源码建议顺序：`define.h` → `inertia.cpp` → `main.cpp` → `role.cpp` 的 `action` → `scene.cpp` 的 `createMap`。

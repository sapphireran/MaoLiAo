# 猫里奥 MaoLiAo

个人练习项目：用 C++ 和 [EasyX](https://easyx.cn/) 写的横版过关游戏。主角叫猫里奥，一共三关。前两关是平台跳跃，第三关是随机管道间隙的跳跃关（仿 Flappy Bird）。

2020 年研究与开发实践课程作业，后来整理成这个仓库。窗口标题是 **猫里奥 V2.0**。

```
512 x 384 窗口
  ├─ Control   菜单、暂停、读档、HUD
  ├─ Role      主角、敌人、子弹、碰撞
  ├─ Scene     地图块、金币、蘑菇、视差背景
  ├─ Inertia   s = vt + ½at²
  └─ Timer     QueryPerformanceCounter 定帧
```

## 操作

| 键 | 作用 |
| --- | --- |
| `A` / `D` | 左右移动 |
| `W` 或 `K` | 跳跃（第三关可连跳） |
| `J` | 射击（先捡到蘑菇） |
| `Esc` | 暂停：返回 / 重开 / 回主菜单 / 存档 |

主菜单还有 **介绍**、**指导**、**读档**、**退出**。

## 计分和生命

- 初始 5 条命，掉出屏幕或撞到敌人（非踩踏）扣一条。
- 金币 +10，踩死或射死敌人 +5。
- 过关判定：世界坐标走到关卡终点后自动向右跑出屏幕。
- 存档只写当前关卡号到 `MaoLiAo/gameRecord.dat`。

## 在 Windows 上编译

需要 Visual Studio 2019+（工具集 v142）和 EasyX。打开 `MaoLiAo.sln`，选 Debug|x86 或 Debug|x64，工作目录要对准 `MaoLiAo/`，这样 `res\` 下的 bmp / mp3 才能被找到。

资源文件（`res\role.bmp`、`res\map.bmp`、背景音乐等）不在这个仓库里，本机或旧备份里要自己放回 `MaoLiAo/res/`。详见 [docs/assets.md](docs/assets.md)。

## 文档和可运行示例

游戏本体绑在 EasyX / Win32 上，Linux 云环境编不了窗口程序。所以把公式、碰撞、按键掩码、关卡表、存档格式抽成可移植示例，用 g++ 就能跑：

| 路径 | 内容 |
| --- | --- |
| [docs/](docs/README.md) | 架构、物理、碰撞、关卡、循环、资源、课堂笔记 |
| [examples/](examples/README.md) | 独立 C++ 示例和 `make test` |

```bash
cd examples && make test
```

## 源码地图

```
MaoLiAo.sln
MaoLiAo/
  main.cpp          主循环：输入 → 行动 → 死亡/过关 → 双缓冲绘制
  define.h          窗口、重力、速度、按键位
  control.cpp/.h    菜单和暂停 UI
  role.cpp/.h       主角、敌人、子弹
  scene.cpp/.h      三关地图和金币
  inertia.cpp/.h    匀加速位移
  timer.h           高精度 Sleep
  gameRecord.dat    单整数关卡存档
```

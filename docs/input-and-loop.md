# 输入、主循环、存档

## 按键位

`define.h` 里每个动作占一位，`GetCommand` 用 `|=` 叠起来：

| 宏 | 值 | 键 |
| --- | ---: | --- |
| `CMD_LEFT` | 1 | A |
| `CMD_RIGHT` | 2 | D |
| `CMD_UP` | 4 | W 或 K |
| `CMD_DOWN` | 8 | S（读了，`action` 里空分支） |
| `CMD_SHOOT` | 16 | J |
| `CMD_ESC` | 32 | Esc |
| `VIR_RETURN` | 64 | 暂停：返回游戏 |
| `VIR_RESTART` | 128 | 暂停：重新开始 |
| `VIR_HOME` | 256 | 暂停：退出到主菜单 |

`getKey` 在没有新键时仍返回上一帧的 `key`。松手后 `_kbhit()` 为假，位会一直留着，直到下一次有键盘消息。暂停菜单用鼠标改 `key` 为虚拟值。

示例：`examples/src/command_demo.cpp` 把掩码拆回动作名。

## 主循环状态机

用生命和关卡号就能复述 `main.cpp`：

```text
life = 5, world = 1
进入 gameStart（挡在循环外，点「开始」或合法读档才返回）

每帧:
  key = getKey()
  if key == RESTART: 重建当前 world 的 Role/Scene，BGM 从头
  if key == HOME:    life=5, world=1, 再进 gameStart
  Role.action(key)
  Scene.action(role)

  if died:
      停 BGM，最后一帧定格 3.5s
      life -= 1
      if life == 0:
          游戏结束曲 + showGameOver
          life=5, world=1, gameStart, 重建
      else:
          showDied(life)   // 画剩余命数的猫头
          重建当前 world
  if passed:
      停 BGM
      if world == 3:
          胜利曲 + 通关曲 + showPassedAll
          life=5, world=1, 重建, gameStart
      else:
          胜利曲，world += 1, showPassed(world), 重建

  画 Scene、Role、分数、关卡
  Sleep(10ms)
```

`showPassed` 传入的是 **已经加一后的 world**，字幕上的 LEVEL 是下一关编号。

示例：`examples/src/loop_trace.cpp` 用纯数据推这条状态机。

## 存档格式

暂停选「进行存档」，或主菜单「读档」：

```text
文件: MaoLiAo/gameRecord.dat
内容: 一个十进制整数，当前 world（1、2 或 3）
```

读档时若整数 `<= 0` 或 `>= 4` 弹 MessageBox（源码里标题和正文参数对调了，见 study-notes）。成功则 `world = flag` 并离开菜单，**不恢复生命和分数**。仓库里现有的 `gameRecord.dat` 是 `1`。

示例：`examples/src/save_demo.cpp` 读写同样的单整数文件。

## HUD

`showScore` / `showLevel` 每帧用 Cooper 字体画在左上和右上。死亡等待的 3.5 秒里也会画一次，所以能看见死前的分数。

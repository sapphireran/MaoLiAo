# 课堂笔记：对着源码记下来的坑

2020 年交作业时能跑就行。下面这些是后来重读时标的，改游戏请先对一下行号。文档和 `examples/` 按「作者原意」建模，不把未定义行为当成特性。

## `define.h` 头卫写错

```cpp
#ifndef MYDEFINE
#define MYDIFINE    // 不是 MYDEFINE
```

多次包含仍会再定义一遍宏。现在每个 cpp 只含一次，所以没爆。应改成 `#define MYDEFINE`。

## 带分号的宏

```cpp
#define F TIME*0.3;
#define LIFE 5;
```

`int life = LIFE;` 能编过（多一个空语句）。`scenery_iframe += F` 也能编过。不能写 `LIFE + 1` 这种表达式。示例代码用字面量 5 和 `0.003`。

## `createEnemy` / `createCoin` / `createFood` 多走一格

```cpp
while (i <= sizeof(arr) / sizeof(arr[0]))
```

`<=` 会读 `arr[count]`，也就是数组后面的栈垃圾，再写进 `myEnemy[i]` / `coins[i]`。第一关 10 只敌人会多写第 11 个槽。多数时候垃圾是 0，看起来像「没多一只怪」。

第三关 `createMap` 同样 `i <= sizeof(m)/sizeof(m[0])`，还会写到 `map[30]` 以外——`map` 只有 30 格，这是明确的缓冲区溢出。

## `MAP_NUMBER = 30` 截断关卡

第一关列表 32 块，第二关 36 块。循环 `i < MAP_NUMBER` 丢掉末尾景色：第一关两格水，第二关草、旗、树。关卡设计表和运行时不一致。

## 暂停菜单文案和返回值

绿色按钮写「退出游戏」，hover 英文是 `The menu`，点击发 `VIR_HOME`（回主菜单），并不是结束进程。主菜单「退出」才 `exit(0)`。

存档按钮的注释仍写「如果选择主菜单」。

## 读档 MessageBox

```cpp
MessageBox(GetForegroundWindow(), "提醒", "存档缺失，请检查存档", 1);
```

Win32 签名是 `(hwnd, text, caption, type)`，所以窗口标题变成「存档缺失…」，正文才是「提醒」。

## `gameStart` 递归

介绍/指导页点返回会 `gameStart()` 再进一层，而不是设标志回到菜单循环。栈不深，但每次重新 `putimage` 整张背景。

## `GetCommand` 和粘滞按键

`getKey` 只在 `_kbhit()` 为真时更新 `key`。控制台键盘缓冲和 `GetAsyncKeyState` 混用，松键后旧掩码可能保持到下一次按键。暂停用鼠标写虚拟键，回来后这一帧 `key` 不再含方向位。

## 子弹函数名

`bullteFlying`、`LEHGTH_INTERVAL_BULLET`：历史拼写，搜索时按错字母会找不到。

## `inertia.cpp` 顶上的 `#pragma once`

`.cpp` 里写 `#pragma once` 没有意义，也不伤事。

## `Timer` 定义在头文件

静态成员在 `timer.h` 里定义。第二个 cpp 再 include 就会重复定义。目前只有 `main.cpp` 用。

## 第三关随机没有 `srand`

`random(a,b)` 是 `rand()%(b-a)+a`。没见到 `srand`，管道布局每次启动都一样（CRT 默认种子），只有进程重启且运行库改种子时才会变。注释写「自动随机生成」，实际常是固定序列。

## 水平挤墙用 `world = 1`

`hitMap(myHero.x, myHero.y, myScene, 1)` 在第三关挤墙时不会触发即死。脚底检测仍传真实 `world`。

## 分数特效坐标

金币是格子坐标，蘑菇是像素。`setScorePos` 之后 `show` 一律按 `* WIDTH` 画闪光。蘑菇拾取闪光会画飞到屏幕外或错位。

## 不改游戏代码的原因

这份仓库这次只加 `docs/` 和 `examples/`。上面每条都可以单独开修复提交；示例里的循环都用 `< count`，避免把作业里的越界复制进去。

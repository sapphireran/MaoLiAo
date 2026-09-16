# 物理

运动公式集中在 `Inertia::move` 和 `Role::action`。屏幕 y 向下为正，跳跃初速因此是负数。

## 时间步

`TIME` 是 0.01 秒。主循环末尾 `Timer::Sleep((int)(TIME * 1000))`，目标 100 FPS。位移先按「现实米」算，再乘像素比例：

```
像素位移 = Inertia::move(v, TIME, a) * UNREAL_HEIGHT / REAL_HEIGHT
```

- `REAL_HEIGHT = 3.5`（注释里写的「感觉跳了 3.5 米」）
- `UNREAL_HEIGHT = 3 * 32 + 5 = 101` 像素

比例约为 `101 / 3.5 ≈ 28.857` 像素/米。

## 匀加速位移

`inertia.cpp`：

```text
X = v * t + 0.5 * a * t * t
v = v + a * t
return X
```

`v` 是引用，调用后速度已经加上 `a*t`。垂直和水平都走同一条公式。

示例：`examples/src/inertia_demo.cpp` 用同一实现对若干 `(v, t, a)` 做断言。

## 跳跃

地面上按 `W`/`K`，或第三关任意时刻按跳：

```text
isFly = true
vY    = -sqrt(2 * G * REAL_HEIGHT)
```

`G = 30`。初速大小是 `sqrt(210) ≈ 14.491`。若不碰顶，上升到最高点的「现实」高度正好是 3.5，对应约 101 像素。落地后把 `y` 对齐到格子中心：

```text
y = (y + HEIGHT/2) / HEIGHT * HEIGHT
```

下落且 `y > YSIZE`（384）判定坠亡。

第三关 `CMD_UP` 不检查 `isFly`，所以可以空中再跳。这是 Flappy 手感的来源，不是二段跳计数器。

## 水平加速度和摩擦

按左：`a -= A_ROLE`（`A_ROLE = 20`）。按右或已经 `ending`：`a += A_ROLE`。过关走廊会强制一直加速向右。

速度和加速度反向时，地面给一个摩擦加速度 `a1`：

```text
a1 = k * G * map->u
```

`k` 带方向，源码里用 `Xabs / vX` 再加减 3，用来加强刹车。腾空时 `map == NULL`，`a1 = 0`，空中不减速。

`u` 按砖块 id 在 `createMap` 里算好：

| 地面手感 | 宏 | 公式 | 典型 id |
| --- | --- | --- | --- |
| 摩擦大 | `T1 = 0.5` | `(V_MAX / T1) / G` | 第一关管道砖 id 6；第三关多数实体砖 |
| 中等 | `T2 = 1.2` | `(V_MAX / T2) / G` | 草、雪、云 (1, 3, 4, 5, 2) |
| 摩擦小 | `T3 = 1.5` | `(V_MAX / T3) / G` | 默认（装饰碰撞等） |

`V_MAX = 8`。算完这一步如果 `|vX| > V_MAX` 就按比例压回去。

示例：`examples/src/friction_demo.cpp`。

## 相机和视差

主角屏幕 x 夹在 `XLEFT = 0` 和 `XRIGHT = 6 * 32 = 192` 之间。再往右走时不增加 `x`，而是减少 `x0`，整张地图往左退。`Scene::action` 里：

```text
xMap = hero.x0
bgStep = |vX| * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG
```

`K_MAP_BG = 5`，天空比地形慢五倍。`xBg` 减到一张图宽以外就回 0，两张图循环。

世界坐标里主角位置是 `-x0 + x`。`isEnding` 用这个距离和关卡长度比。

## 敌人步进

敌人每半帧（`enemy_iframe * 100` 为偶数）沿 `turn` 走 `ENEMY_STEP = 1` 像素。脚下一格没有实体砖，或身子撞墙，就 `turn *= -1`。它们不用惯性，也不受 `V_MAX` 限制。

## 子弹

`LEHGTH_INTERVAL_BULLET = 4` 像素/帧（宏名拼写如此）。按住 `J` 时要等 `TIME_INTERVAL_BULLET = 0.2` 秒才能再射。飞出窗口、碰到 `id` 1–10 的砖、打到敌人、或屏幕 x 超过 `MAX_DISTANCE = 480` 都会清掉并播爆炸帧。

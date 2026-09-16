# 碰撞

所有相交测试都是「主体四个顶点是否落进目标矩形」。顶点比精灵缩进 1 像素，避免贴边就误判。

## 四顶点

对屏幕坐标 `(x, y)` 的 32×32 精灵：

```text
xmap = (int)hero.x0          // 相机原点，地图空间用
r0 = (-xmap + x + 1,           y + 1)
r1 = (-xmap + x + WIDTH - 1,   y + 1)
r2 = (-xmap + x + 1,           y + HEIGHT - 1)
r3 = (-xmap + x + WIDTH - 1,   y + HEIGHT - 1)
```

目标矩形用左上 `m0` 和右下 `m1`。`isHit` 只要有一个 `ri` 满足：

```text
m0.x <= ri.x <= m1.x && m0.y <= ri.y <= m1.y
```

就返回 true。这不是完整的分离轴定理：主体很大而目标是细条时，四角都在条外、条却穿过身体中间，会漏检。砖块和金币都按格子摆，实际关卡里很少出现那种细条。

示例：`examples/src/aabb_demo.cpp`。

## 地形 `hitMap`

只遍历 `id` 在 1…10 且 `id > 0` 的槽，上限 `MAP_NUMBER = 30`。

普通砖：

```text
m0 = (tile.x * 32, tile.y * 32)
m1 = m0 + (xAmount * 32, yAmount * 32)
```

管道口 `id` 8 和 10 按 **2× 格子** 算宽高，和 `Scene::show` 里 `putimage(..., 2*WIDTH, 2*HEIGHT)` 对齐。

返回第一个碰到的 `Map*`。水平顶墙时调用方用 `world = 1` 做检测，避免第三关「碰砖即死」在挤墙修正里误触发。

## 第三关即死

`hitMap` 里：

```text
if (world == 3 && map.id != 2 && hero.isShoot == false)
    hero.died = true
```

id 2 是云朵，吃了蘑菇（`isShoot`）后管道也不即死。落地用 `hitMap(x, y+1)`，飞的时候头顶脚底都走同一条函数。

## 金币和蘑菇

金币存在格子坐标里，矩形就是一格 32×32。吃到后坐标清零，`Scene::setScorePos` 记一格播「+分」帧。

蘑菇 `food` 用像素坐标。碰撞盒比精灵略大：

```text
宽 = 3*WIDTH/2 + 4 = 52
高 = 4*HEIGHT/5 = 25
```

吃到以后 `isShoot = true`，才能按 `J`。

## 敌人

`turn != 0` 的槽才检测。矩形就是敌人脚点起的 32×32。

- 主角 `vY > 0`（正在下落）踩到：敌人清槽、+5 分、爆炸。
- 之后再测一次，若仍重叠且 `vY <= 0`：主角死亡。

顺序在 `action` 后半段，所以同一帧里先处理踩踏再处理误撞。

## 子弹打地形

子弹也走 `hitMap`，但 **写死 `world = 1`**，所以第三关子弹撞管道不会把主角判死。命中点按格子对齐后再 `setBomb`。

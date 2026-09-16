# 资源文件

程序用相对路径 `res\\...` 加载，工作目录必须是 `MaoLiAo/`（Visual Studio 调试时把 Working Directory 设成 `$(ProjectDir)`）。这个 git 仓库没有提交 `res/`，下面按源码列出缺什么。

## 位图

| 路径 | 谁加载 | 用法 |
| --- | --- | --- |
| `res\home.bmp` | Control | 主菜单、过关、死亡、结束。按 `XSIZE × 5*YSIZE` 拉伸，用负 y 选其中一屏 |
| `res\role.bmp` | Role, Control::showDied | 走、转向、死亡。掩码行 + 彩色行，EasyX `SRCAND`/`SRCPAINT` |
| `res\ani.bmp` | Role, Scene | 敌人、爆炸、子弹、金币、拾取闪光、蘑菇，按行切片 |
| `res\mapsky.bmp` | Scene | 三关天空竖着拼，高约 `4*YSIZE`，宽拉成 512 |
| `res\map.bmp` | Scene | 地形砖，每行一个 id |
| `res\scenery.bmp` | Scene | 草、旗、水、树的两帧动画 |

透明图约定：同一主题上下两行，上行（或偏移后的一行）是彩色，下行是掩码。`putimage` 先 `SRCAND` 再 `SRCPAINT`。

`role.bmp` 行大致是：

- 行 0：向右走两帧 + 死亡 + 向左走两帧的彩色
- 行 1：对应掩码

`Control::showDied` 取 `(2*WIDTH, 0)` 那一格猫头当剩余命图标。

## 声音

`mciSendString("open res\\....mp3 alias ...")`，链接 `Winmm.lib`。

| 文件 | 别名 | 时机 |
| --- | --- | --- |
| `背景音乐.mp3` | `music_bg` | 进关循环播放 |
| `胜利.mp3` | `music_win` | 过关 |
| `通关.mp3` | `music_passedAll` | 打完第三关 |
| `游戏结束.mp3` | `music_end` | 五条命用完 |
| `死亡1.mp3` | `music_died` | 撞敌 / 坠落 |
| `跳.mp3` | `music_jump` | 起跳 |
| `金币.mp3` | `music_coin` | 吃金币 |
| `踩敌人.mp3` | `music_tread` | 踩死 |
| `吃到武器.mp3` | `music_getWeapon` | 蘑菇 |
| `子弹.mp3` | `music_bullet` | 开火 |
| `子弹撞墙.mp3` | `music_boom` | 子弹碰砖或到射程 |
| `子弹打到敌人.mp3` | `music_boom2` | 子弹命中敌人 |

别名在 `Role` 构造里打开，对象被赋值替换时旧别名不会 `close`，多次死会重复 `open` 同一别名。

## 其它

| 路径 | 说明 |
| --- | --- |
| `MaoLiAo.ico` / `MaoLiAo.rc` | 窗口图标，`IDI_ICON1` |
| `gameRecord.dat` | 存档，一个整数 |

图标和 rc 在工程里；`res\` 美术和 mp3 需要自行放回。

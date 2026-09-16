# 13 — Player guide (personal)

How I actually play the 2020 binary, written so a future me does not have to re-derive it from `control.cpp`.

## First launch

1. Build Debug Win32 with EasyX. Put `res\` next to the exe.
2. Click **开始** unless you already have a `gameRecord.dat` with `1`, `2`, or `3`.
3. Do not click **读档** on a missing file — the MessageBox title will look like an error because the caption/text arguments are swapped.

## Movement that matters

- **A / D** — walk. There is inertia. Tapping D does not teleport a tile; you accelerate toward `V_MAX` and slide when you let go.
- **W or K** — jump. In worlds 1–2 this only works on the ground. In world 3 you can flutter: hold jump and you keep re-launching.
- **J** — shoot, but only after the mushroom. There is one mushroom per world. World 3’s mushroom is a 52×25 sliver at pixel `(10, 10)` — ignore it unless you are experimenting.
- **Esc** — pause. Click **返回游戏**, then tap a movement key. Esc can “stick” because `getKey` only refreshes on `_kbhit()`.

## World 1

Stay on the grass. The first pit is around column 15 (water scenery, not a floor). The mid stretch looks snowy (`id 5`) but plays like normal friction. Columns 67–77 are the nasty bit: sticky pipe floor, a one-tile step with a coin stack, then a gap. Enemies reverse at ledges; you can bait them off.

Coins: cloud row, high cloud, the one-tile elevator, then the zigzag into the flag.

## World 2

Do not treat the snow walkway as continuous — it is islands. The real route is the cloud staircase on the left (row 3–9) and the brick stairs around columns 67–75. The mushroom sits on the lonely high cloud at column 39. Ending is **ten tiles later** than world 1 (`104` vs `94`).

## World 3

This is the Flappy homage. Pipes are lethal. Clouds are not. The hole is four tiles tall and its row is random every process (no `srand`). Hold jump, thread seven gaps, land on the long cloud at column 80, walk into the flag. Enemies still patrol in the gaps; a stomp is possible but greed is how you eat a pipe.

If you picked up a mushroom in a previous world: you did not. Death rebuilds `Role`, and a clear rebuilds `Role`. Immunity never carries.

## Scoring for its own sake

Coins are twice an enemy. A “clean” world 1 coin route is 19 coins = 190 plus whatever stomps you take. Nothing is saved except the world index, so a high score is a screenshot.

## Pause menu truth table

| What it says | What it does |
| --- | --- |
| 返回游戏 | Resume. `key` becomes `VIR_RETURN`. |
| 重新开始 | New `Role`+`Scene` for **this** world. Score gone. Lives kept. |
| 退出游戏 | Home screen, `life = 5`, `world = 1`. Not `exit(0)`. |
| 进行存档 | Writes `world` to `gameRecord.dat`, then resumes. |

Home **退出** is the real process exit.

## If the window is a white box

`res\` is missing. The repo does not ship the 2020 art. Restore the bitmap/MP3 pack from the personal backup, not from this git tree.

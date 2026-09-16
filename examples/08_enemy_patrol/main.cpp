#include "check.hpp"
#include "constants.hpp"
#include "enemy.hpp"
#include "map_data.hpp"

#include <cstdio>
#include <vector>

// One goomba-style walker on world 1's opening grass [0,9] x 15 tiles.
// It should bounce between the left wall (tile 0) and the right edge of
// that platform (tile 15), reversing at the ledge like Role::action.

int main() {
    std::printf("=== 08 enemy patrol ===\n");

    std::size_t n = 0;
    const mla::MapRec* w1 = mla::world1Map(n);
    const std::size_t loaded = mla::loadedCount(n);

    mla::Patrol e;
    e.x = 3 * mla::kWidth;  // first world1 enemy
    e.y = 8 * mla::kHeight;
    e.turn = 1;

    std::printf("start x=%d y=%d turn=%+d  on grass [0,15) tiles\n", e.x, e.y,
                e.turn);

    int reversals = 0;
    int lastTurn = e.turn;
    int minX = e.x;
    int maxX = e.x;
    const int steps = 800;
    for (int i = 0; i < steps; ++i) {
        mla::stepPatrol(e, w1, loaded, mla::kEnemyStep);
        if (e.turn != lastTurn) {
            ++reversals;
            lastTurn = e.turn;
            std::printf("  reverse #%d at x=%d  now turn=%+d  step=%d\n", reversals,
                        e.x, e.turn, i);
        }
        if (e.x < minX) {
            minX = e.x;
        }
        if (e.x > maxX) {
            maxX = e.x;
        }
    }

    std::printf("after %d steps: x=%d turn=%+d reversals=%d range=[%d,%d]\n",
                steps, e.x, e.turn, reversals, minX, maxX);

    // A second walker on the isolated cloud [10,6] 4 tiles wide.
    mla::Patrol cloud;
    cloud.x = 11 * mla::kWidth;
    cloud.y = 5 * mla::kHeight;
    cloud.turn = 1;
    int cloudRev = 0;
    int ct = cloud.turn;
    for (int i = 0; i < 400; ++i) {
        mla::stepPatrol(cloud, w1, loaded, mla::kEnemyStep);
        if (cloud.turn != ct) {
            ++cloudRev;
            ct = cloud.turn;
        }
    }
    std::printf("cloud walker reversals=%d range would stay on 4-tile platform\n",
                cloudRev);

    mla::Checks c;
    c.expect(reversals >= 2, "opening-grass walker turns at least twice");
    c.expect(minX >= -mla::kWidth && maxX <= 16 * mla::kWidth,
             "stays near the 15-tile grass (ledge / wall)");
    c.expect(cloudRev >= 2, "cloud walker also patrols");
    c.expect(e.turn == 1 || e.turn == -1, "turn is always ±1");

    const std::vector<mla::Rect> boxes = mla::solidBoxes(w1, loaded);
    c.expect(!boxes.empty(), "world1 exposes solid AABBs");
    return c.finish("08_enemy_patrol");
}

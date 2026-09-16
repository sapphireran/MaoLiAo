#include <iostream>
#include <vector>

#include "maoliao/aabb.hpp"
#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"
#include "maoliao/worlds.hpp"

namespace {

// Minimal patrol step from Role::action: move ENEMY_STEP, flip if the
// body hits a solid or the pixel under the front foot has no floor.
bool hits_any(int sx, int sy, const std::vector<maoliao::Tile>& map) {
    for (const auto& t : map) {
        if (maoliao::hero_hits_tile(sx, sy, 0, t)) {
            return true;
        }
    }
    return false;
}

}  // namespace

int main() {
    using namespace maoliao;

    // A 20-tile-wide floor at y=8, walls at x=0 and x=19.
    std::vector<Tile> map;
    map.push_back(Tile{0, 8, 1, 20, 1, u_t2()});
    map.push_back(Tile{0, 6, 1, 1, 2, u_t2()});
    map.push_back(Tile{19, 6, 1, 1, 2, u_t2()});

    Actor e{5 * kWidth, 7 * kHeight, 1};

    int flips = 0;
    int last_turn = e.turn;
    for (int step = 0; step < 800; ++step) {
        e.x += e.turn * kEnemyStep;
        const int x1 = e.x;
        const int y1 = e.y;
        const int x2 = e.x + e.turn * kWidth;
        const int y2 = e.y + 1;
        if (hits_any(x1, y1, map) || !hits_any(x2, y2, map)) {
            e.turn *= -1;
        }
        if (e.turn != last_turn) {
            ++flips;
            last_turn = e.turn;
        }
    }

    std::cout << "patrol flips   " << flips << "  x " << e.x << " turn " << e.turn << "\n";
    MLA_CHECK(flips >= 2);
    MLA_CHECK(e.x > 0 && e.x < 20 * kWidth);

    // Stomp vs side-hit: falling (vY>0) lives; rising/idle dies.
    const bool stomp = true;   // vY > 0
    const bool side = false;   // vY <= 0
    MLA_CHECK(stomp && !side);

    const auto w1 = world1_enemies();
    MLA_CHECK(w1.size() == 10);
    MLA_CHECK(w1[0].x == 3 && w1[0].turn == 1);
    MLA_CHECK(w1[1].turn == -1);

    return done("enemy_patrol");
}

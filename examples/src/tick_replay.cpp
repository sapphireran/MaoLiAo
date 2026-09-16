#include <iomanip>
#include <iostream>
#include <vector>

#include "maoliao/aabb.hpp"
#include "maoliao/camera.hpp"
#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"
#include "maoliao/kinematics.hpp"
#include "maoliao/worlds.hpp"

// A short headless replay: stand on world-1 grass, jump, hold right.
// This is not the EasyX binary — it only chains the same integrators.

struct Hero {
    maoliao::Camera cam;
    double yy = maoliao::kSpawnY;
    double vY = 0;
    bool fly = true;
    int score = 0;
};

int main() {
    using namespace maoliao;

    const auto floor = stored_map(world1_authored());
    Hero h;
    h.cam.x = kX0;
    h.cam.xx = kX0;
    h.yy = kSpawnY;

    auto on_floor = [&](int x, int y) -> const Tile* {
        for (const Tile& t : floor) {
            if (hero_hits_tile(x, y + 1, static_cast<int>(h.cam.x0), t)) {
                return &t;
            }
        }
        return nullptr;
    };

    // Drop until the grass at y=9 (pixel 288) catches us.
    int land = -1;
    for (int i = 0; i < 400; ++i) {
        if (h.fly) {
            air_step(h.yy, h.vY);
            h.cam.x = h.cam.x;  // screen x unchanged while falling
            if (const Tile* t = on_floor(h.cam.x, static_cast<int>(h.yy))) {
                if (h.vY > 0) {
                    h.fly = false;
                    h.vY = 0;
                    const int y = static_cast<int>(h.yy);
                    h.yy = (y + kHeight / 2) / kHeight * kHeight;
                    land = i;
                    (void)t;
                    break;
                }
            }
        }
    }
    std::cout << "land frame     " << land << "  y " << h.yy << "\n";
    MLA_CHECK(land >= 0);
    MLA_CHECK(static_cast<int>(h.yy) % kHeight == 0);

    // Jump + hold right for 120 frames. Collect any coin we overlap.
    h.fly = true;
    h.vY = jump_speed();
    auto coins = world1_coins();
    int collected = 0;
    for (int i = 0; i < 120; ++i) {
        if (h.fly) {
            air_step(h.yy, h.vY);
            if (const Tile* t = on_floor(h.cam.x, static_cast<int>(h.yy))) {
                if (h.vY > 0) {
                    h.fly = false;
                    h.vY = 0;
                    const int y = static_cast<int>(h.yy);
                    h.yy = (y + kHeight / 2) / kHeight * kHeight;
                }
                (void)t;
            }
            if (h.yy > kYSize) {
                break;
            }
        } else if (!on_floor(h.cam.x, static_cast<int>(h.yy))) {
            h.fly = true;
        }

        const Tile* feet = on_floor(h.cam.x, static_cast<int>(h.yy));
        const double u = feet ? feet->u : 0;
        const double dx = walk_step(h.cam.vX, kARole, u, feet != nullptr);
        apply_rail(h.cam, dx);

        const Vertex4 corners =
            hero_corners(h.cam.x, static_cast<int>(h.yy), static_cast<int>(h.cam.x0));
        for (Actor& c : coins) {
            if (c.x == 0 && c.y == 0) {
                continue;
            }
            if (is_hit(corners, coin_box(c.x, c.y))) {
                h.score += kScoreCoin;
                ++collected;
                c.x = 0;
                c.y = 0;
            }
        }
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "after jump     x " << h.cam.x << "  x0 " << h.cam.x0
              << "  y " << h.yy << "  score " << h.score
              << "  coins " << collected << "\n";
    MLA_CHECK(h.cam.x >= kX0);
    MLA_CHECK(h.yy < kYSize);
    MLA_CHECK(h.score == collected * kScoreCoin);

    return done("tick_replay");
}

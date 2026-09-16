#pragma once

#include "camera.hpp"
#include "collision.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "friction.hpp"
#include "inertia.hpp"
#include "levels.hpp"

#include <cmath>
#include <vector>

namespace maoliao {

struct HeroState {
    int x = kSpawnX;
    int y = kSpawnYActual;
    double xx = kSpawnX;
    double yy = kSpawnYActual;
    double x0 = 0.0;
    double vx = 0.0;
    double vy = 0.0;
    int turn = 1;
    bool fly = true;
    bool shoot = false;
    bool died = false;
    bool ending = false;
    bool passed = false;
    int world = 1;
};

struct TickInfo {
    double walk_accel = 0.0;
    double friction_accel = 0.0;
    double pixel_dx = 0.0;
    bool jumped = false;
    bool landed = false;
};

inline void launch_jump(HeroState& hero) {
    hero.fly = true;
    hero.vy = jump_launch_speed();
}

inline TickInfo step_hero(HeroState& hero, int key, const std::vector<Tile>& map) {
    TickInfo info;

    const bool want_up = has_cmd(key, kCmdUp);
    if ((want_up && !hero.fly && !hero.ending) || (want_up && hero.world == 3)) {
        launch_jump(hero);
        info.jumped = true;
    }

    const Tile* tiles = map.empty() ? nullptr : map.data();
    const std::size_t n = map.size();

    auto probe = [&](int sx, int sy) -> const Tile* {
        if (tiles == nullptr) {
            return nullptr;
        }
        const auto hit = hit_map(sx, sy, static_cast<int>(hero.x0), tiles, n,
                                 hero.world, hero.world == 3 ? &hero.died : nullptr);
        if (!hit) {
            return nullptr;
        }
        return &map[hit->index];
    };

    const Tile* ground = nullptr;
    if (hero.fly) {
        const double dy = integrate_tick(hero.vy, kGravity);
        hero.yy += dy * kMetersToPixels;
        hero.y = static_cast<int>(hero.yy);
        ground = probe(hero.x, hero.y + 1);
        if (ground != nullptr) {
            if (hero.vy > 0.0) {
                hero.fly = false;
                info.landed = true;
            }
            hero.vy = 0.0;
            hero.y = (hero.y + kTileHeight / 2) / kTileHeight * kTileHeight;
            hero.yy = hero.y;
        }
        if (hero.vy > 0.0 && hero.y > kScreenHeight) {
            hero.died = true;
        }
    } else {
        ground = probe(hero.x, hero.y + 1);
        if (ground == nullptr) {
            hero.fly = true;
        }
    }

    double a = 0.0;
    if (has_cmd(key, kCmdLeft) && !hero.ending) {
        a -= kWalkAccel;
        hero.turn = -1;
    }
    if (has_cmd(key, kCmdRight) || hero.ending) {
        a += kWalkAccel;
        hero.turn = 1;
    }
    info.walk_accel = a;

    const double old_vx = hero.vx;
    const double xabs = std::fabs(old_vx);
    double a1 = 0.0;
    if (old_vx * a <= 0.0 && old_vx != 0.0) {
        if (ground != nullptr) {
            a1 = friction_accel(old_vx, ground->u, false);
        }
    }
    info.friction_accel = a1;

    const double h = integrate_tick(hero.vx, a + a1) * kMetersToPixels;
    if (old_vx * hero.vx < 0.0) {
        hero.vx = 0.0;
    }
    hero.xx += h;
    hero.x = static_cast<int>(hero.xx);
    info.pixel_dx = h;

    if (is_ending(hero.world, world_x(hero.x, hero.x0))) {
        hero.ending = true;
    }
    if (is_passed(hero.x)) {
        hero.passed = true;
    }

    if (tiles != nullptr) {
        bool ignore_death = false;
        const auto wall = hit_map(hero.x, hero.y, static_cast<int>(hero.x0), tiles,
                                  n, 1, &ignore_death);
        if (wall) {
            if (hero.x > kRightLimit) {
                hero.x = kRightLimit;
            } else {
                hero.x = static_cast<int>(hero.xx - h);
            }
            hero.xx = hero.x;
            hero.vx = 0.0;
        }
    }

    if (xabs > kMaxSpeed) {
        hero.vx = hero.vx / xabs * kMaxSpeed;
    }

    pin_hero(hero.x, hero.xx, hero.vx, hero.x0, hero.ending);
    return info;
}

inline std::vector<Tile> flat_floor(int width_tiles = 40, int row = 9, int id = 1,
                                    int world = 1) {
    return {make_tile(0, row, id, width_tiles, 1, world)};
}

} // namespace maoliao

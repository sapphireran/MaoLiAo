#include "maoliao_model.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace maoliao;

struct Case {
    const char* name;
    int hx, hy;
    double x0;
    Box box;
    bool expect;
};

static int fail(const std::string& msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

int main() {
    const Box grass = tile_box(0, 9, 15, 1, 1); // [0, 480) x [288, 320)
    std::cout << "grass platform box [" << grass.x0 << "," << grass.x1 << "] x ["
              << grass.y0 << "," << grass.y1 << "]\n";

    const Box pipe = tile_box(36, 7, 1, 1, 10); // 64x64
    if (pipe.x1 - pipe.x0 != 64 || pipe.y1 - pipe.y0 != 64) return fail("pipe mouth is 64x64");

    const Box shaft = tile_box(36, 0, 1, 4, 7); // 32-wide despite 64-wide art
    if (shaft.x1 - shaft.x0 != 32) return fail("id 7 hit box stays 32 px wide");

    std::vector<Case> cases = {
        // Role probes (x, y+1) for ground. Inset bottom then sits on y=288.
        {"feet on grass (y+1 probe)", 64, 256 + 1, 0.0, grass, true},
        {"sprite origin only, no probe", 64, 256, 0.0, grass, false},
        {"air above grass", 64, 200, 0.0, grass, false},
        {"past the 15-tile cap", 500, 256 + 1, 0.0, grass, false},
        {"corner exactly on tl after inset", 0, 288, 0.0, grass, true},
        {"pipe mouth overlap", 36 * 32 + 8, 7 * 32 + 8, 0.0, pipe, true},
        {"beside pipe mouth", 36 * 32 - 40, 7 * 32, 0.0, pipe, false},
    };

    int failed = 0;
    for (const auto& c : cases) {
        const bool hit = hero_hits_box(c.hx, c.hy, c.x0, c.box);
        std::cout << (hit == c.expect ? "  ok  " : " FAIL ") << c.name << " -> " << hit << "\n";
        if (hit != c.expect) ++failed;
    }

    // Scenery ids do not enter hitMap.
    if (collides_as_tile(11) || collides_as_tile(12) || collides_as_tile(13) || collides_as_tile(14)) {
        return fail("ids 11-14 must not collide");
    }
    if (!collides_as_tile(1) || !collides_as_tile(10) || collides_as_tile(0)) {
        return fail("ids 1-10 collide, 0 does not");
    }

    // World-3 death table.
    if (!world3_contact_kills(7, false)) return fail("pipe shaft kills unarmed hero");
    if (world3_contact_kills(2, false)) return fail("clouds are safe");
    if (world3_contact_kills(7, true)) return fail("armed hero survives solids");
    if (world3_contact_kills(12, false)) return fail("victory sign is scenery");

    // Camera-shifted hero: x0 = -200, screen x = 192 → world left = 200+192 = 392.
    Point c[4];
    hero_corners_world(192, 100, -200.0, c);
    if (c[0].x != 200 + 192 + 1) return fail("world x uses -x0 + screen + 1");

    if (failed) return fail("fixture table");
    std::cout << "aabb_collision: ok\n";
    return 0;
}

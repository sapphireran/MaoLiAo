#include "maoliao_model.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

static bool gap_open(const std::vector<MapRec>& pipes, int cluster, int probe_row) {
    // A 32x32 hero at tile column x=10+10*cluster, row probe_row, no camera.
    const int x = (cluster * 10 + 10) * WIDTH;
    const int y = probe_row * HEIGHT;
    for (const auto& r : pipes) {
        if (r.x != cluster * 10 + 10) continue;
        if (!collides_as_tile(r.id)) continue;
        if (hero_hits_box(x, y, 0.0, tile_box(r.x, r.y, r.x_amount, r.y_amount, r.id))) {
            return false;
        }
    }
    return true;
}

int main() {
    // Game: height[i] = rand() % 6 + 1  → 1..6
    const int heights[7] = {1, 2, 3, 4, 5, 6, 3};
    const auto pipes = world3_map_from_heights(heights);
    if (pipes.size() != 30) return fail("7*4 + 2 = 30");

    for (int i = 0; i < 7; ++i) {
        const int x = i * 10 + 10;
        const int h = heights[i];
        const auto& top_mouth = pipes[static_cast<size_t>(i * 4 + 0)];
        const auto& top_shaft = pipes[static_cast<size_t>(i * 4 + 1)];
        const auto& bot_mouth = pipes[static_cast<size_t>(i * 4 + 2)];
        const auto& bot_shaft = pipes[static_cast<size_t>(i * 4 + 3)];
        std::cout << "cluster " << i << " x=" << x << " h=" << h
                  << " top_mouth.y=" << top_mouth.y << " bot_mouth.y=" << bot_mouth.y << "\n";
        if (top_mouth.id != 8 || bot_mouth.id != 10) return fail("mouth ids");
        if (top_mouth.x != x || bot_shaft.x != x) return fail("cluster x");
        if (top_mouth.y != h - 2) return fail("top mouth row");
        if (bot_mouth.y != 4 + h) return fail("bottom mouth row");
        if (top_shaft.y_amount != std::max(0, h - 2)) return fail("top shaft length");
        if (bot_shaft.y_amount != std::max(0, 6 - h)) return fail("bottom shaft length");
    }

    // Gap between the mouths should stay open at row ~ h+1 for a mid height.
    if (!gap_open(pipes, 2, heights[2] + 1)) return fail("expected a gap at h+1 for cluster 2");
    if (gap_open(pipes, 2, 0)) return fail("row 0 should hit the top shaft / mouth");

    // Cloud runway is the only safe solid if unarmed.
    const auto& runway = pipes[28];
    if (runway.id != 2 || runway.x_amount != 25) return fail("cloud runway");
    if (world3_contact_kills(runway.id, false)) return fail("runway must not kill");
    if (!world3_contact_kills(8, false)) return fail("mouths kill");

    // Flap: every UP press resets vY, so two jumps in air share the same takeoff speed.
    double v = jump_speed();
    integrate(v, TIME, G);
    integrate(v, TIME, G);
    const double mid_air = v;
    v = jump_speed();
    if (std::fabs(v - jump_speed()) > 1e-12) return fail("flap replaces vY");
    if (mid_air <= jump_speed()) return fail("gravity should have increased vY before the flap");
    std::cout << "flap resets vY from " << mid_air << " to " << v << "\n";

    std::cout << "world3_pipes: ok\n";
    return 0;
}

#include "maoliao_model.hpp"

#include <cmath>
#include <iostream>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

int main() {
    // Hold D from spawn until we are well past the rail.
    auto run = simulate_hold_right(400);
    const auto& last = run.back();
    std::cout << "after 400 ticks: screen_x=" << last.screen_x << " x0=" << last.x0
              << " v=" << last.v_x << " distance=" << last.distance << "\n";

    if (last.screen_x != XRIGHT) return fail("hero should pin to the 192 px rail");
    if (last.x0 >= 0.0) return fail("origin should slide left (negative x0)");
    if (last.distance <= XRIGHT) return fail("world distance should exceed screen x");
    if (std::fabs(last.v_x - V_MAX) > 0.05) return fail("should be at V_MAX");

    // Find the first tick that hits the rail.
    int rail_tick = -1;
    for (const auto& s : run) {
        if (s.screen_x == XRIGHT && s.x0 < 0.0) {
            rail_tick = s.tick;
            break;
        }
    }
    std::cout << "first rail+scroll tick = " << rail_tick << "\n";
    if (rail_tick < 0) return fail("never reached the rail");

    auto cam = apply_rail(0.0, 250.0, V_MAX, false);
    if (cam.screen_x != XRIGHT) return fail("rail clamps 250 -> 192");
    if (std::fabs(cam.x0 - (0.0 - (250 - 192))) > 1e-9) return fail("x0 -= overflow");
    if (cam.sky_dx >= 0.0) return fail("sky moves left (negative) when scrolling right");

    const double expect_sky = -(V_MAX * TIME * UNREAL_HEIGHT / REAL_HEIGHT / K_MAP_BG);
    if (std::fabs(cam.sky_dx - expect_sky) > 1e-12) return fail("parallax 1/5");
    std::cout << "sky dx at V_MAX on rail = " << cam.sky_dx << " (expect " << expect_sky << ")\n";

    auto end_cam = apply_rail(-3000.0, 200.0, V_MAX, true);
    if (end_cam.screen_x != 200) return fail("ending ignores the rail");
    if (end_cam.x0 != -3000.0) return fail("ending does not keep sliding x0");

    if (world_distance(192, -2816.0) != 3008) return fail("world 1 finish distance identity");
    std::cout << "distance at x=192, x0=-2816 is " << world_distance(192, -2816.0)
              << " (world 1 line)\n";

    std::cout << "camera_scroll: ok\n";
    return 0;
}

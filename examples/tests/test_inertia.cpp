#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    CHECK(kScreenWidth == 512);
    CHECK(kScreenHeight == 384);
    CHECK(kTileWidth == 32);
    CHECK(kPixelJumpHeight == 101);
    CHECK(kTick == 0.01);

    const double v0 = jump_launch_speed();
    check_near(v0, -std::sqrt(210.0), 1e-12, "launch");
    check_near(apex_time(v0), std::sqrt(210.0) / 30.0, 1e-12, "apex time");
    check_near(real_height(v0, apex_time(v0)), -kRealJumpMeters, 1e-12, "real apex");
    check_near(pixel_height(v0, apex_time(v0)), -kPixelJumpHeight, 1e-12,
               "pixel apex");

    double v = 0.0;
    const double dx = integrate(v, 0.01, 20.0);
    check_near(dx, 0.001, 1e-12, "x = 1/2 a t^2");
    check_near(v, 0.2, 1e-12, "v = a t");

    double vy = v0;
    double pixel_y = 0.0;
    double min_y = 0.0;
    for (int i = 1; i <= 80; ++i) {
        sample_jump_tick(i, vy, pixel_y);
        if (pixel_y < min_y) {
            min_y = pixel_y;
        }
    }
    check_near(min_y, -kPixelJumpHeight, 0.2, "tick apex near 101 px");
    return report("test_inertia");
}

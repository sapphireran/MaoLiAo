// Camera pin + sky parallax from Role::action and Scene::action.

#include "maoliao_core.hpp"

#include <cmath>
#include <cstdio>

using namespace maoliao;

static int fail = 0;

static void expect_near(const char* name, double got, double want, double eps) {
    if (std::fabs(got - want) > eps) {
        std::printf("FAIL %s: got %.6f want %.6f\n", name, got, want);
        ++fail;
    } else {
        std::printf("ok   %s: %.6f\n", name, got);
    }
}

static void expect_eq(const char* name, int got, int want) {
    if (got != want) {
        std::printf("FAIL %s: got %d want %d\n", name, got, want);
        ++fail;
    } else {
        std::printf("ok   %s: %d\n", name, got);
    }
}

static void expect_true(const char* name, bool cond) {
    if (!cond) {
        std::printf("FAIL %s\n", name);
        ++fail;
    } else {
        std::printf("ok   %s\n", name);
    }
}

struct Hero {
    double xx = kX0;
    int x = kX0;
    double x0 = 0.0;
    double vx = 0.0;
    bool ending = false;
    bool passed = false;
};

static void apply_camera(Hero& h) {
    if (h.x < kXLeft) {
        h.x = kXLeft;
        h.xx = h.x;
        h.vx = 0.0;
    } else if (h.x > kXRight && !h.ending) {
        h.x0 -= (h.x - kXRight);
        h.x = kXRight;
        h.xx = h.x;
    }
    if (h.x > kXSize) {
        h.passed = true;
    }
}

int main() {
    expect_eq("XRIGHT", kXRight, 192);
    expect_eq("XLEFT", kXLeft, 0);

    Hero h;
    h.xx = 200;
    h.x = 200;
    h.vx = kVMax;
    apply_camera(h);
    expect_eq("pinned_x", h.x, 192);
    expect_near("x0_after_first_overshoot", h.x0, -8.0, 1e-12);

    const int world_x = -(int)h.x0 + h.x;
    expect_eq("world_x", world_x, 200);

    // Keep walking right at V_MAX for 100 frames while pinned.
    double x_bg = 0.0;
    int x_map = (int)h.x0;
    for (int i = 0; i < 100; ++i) {
        const int xmap_before = x_map;
        const double step_m = inertia_move(h.vx, kTime, 0.0);
        h.vx = kVMax; // clamp
        h.xx += step_m * kPxPerMeter;
        h.x = (int)h.xx;
        apply_camera(h);
        x_map = (int)h.x0;
        const double xabs = h.vx < 0 ? -h.vx : h.vx;
        const double bg_step = xabs * kTime * kPxPerMeter / kMapBg;
        if (h.x == kXRight && h.vx > 0 && xmap_before != x_map) {
            x_bg -= bg_step;
        }
    }
    std::printf("after 100 pinned frames: x0=%.4f xMap=%d xBg=%.4f world=%d\n", h.x0,
                x_map, x_bg, -(int)h.x0 + h.x);

    // Sky should have moved 1/5 of the tile camera motion this burst.
    // Each frame we move V_MAX * TIME * scale pixels in world space.
    const double world_px_per_frame = kVMax * kTime * kPxPerMeter;
    expect_near("world_px_per_frame", world_px_per_frame, 8.0 * 0.01 * (101.0 / 3.5),
                1e-9);
    expect_near("sky_ratio", std::fabs(x_bg) / std::fabs(h.x0 + 8.0), 1.0 / kMapBg,
                0.05);

    expect_true("ending_w1_3008_false", !is_ending(1, 94 * kWidth));
    expect_eq("ending_w1_3009", (int)is_ending(1, 94 * kWidth + 1), 1);
    expect_eq("ending_w2_3328", (int)is_ending(2, 104 * kWidth), 0);
    expect_eq("ending_w2_3329", (int)is_ending(2, 104 * kWidth + 1), 1);

    // Ending auto-walk: once past the pin with ending=true, x can exceed 192
    // and then 512.
    h.ending = true;
    h.x = 500;
    h.xx = 500;
    apply_camera(h);
    expect_eq("ending_no_pin", h.x, 500);
    h.x = 513;
    apply_camera(h);
    expect_eq("passed", (int)h.passed, 1);

    if (fail) {
        std::printf("\n%d check(s) failed\n", fail);
        return 1;
    }
    std::printf("\nall camera checks passed\n");
    return 0;
}

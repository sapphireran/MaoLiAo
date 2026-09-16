#include "portable/maoliao_math.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

int gFailures = 0;

void expectNear(const char* name, double got, double want, double eps) {
    if (std::fabs(got - want) > eps) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << " = " << got << "\n";
    }
}

void expectTrue(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

} // namespace

int main() {
    using namespace mla;

    expectNear("pixel_scale", kPixelScale, 101.0 / 3.5, 1e-12);
    expectNear("unreal_height", kUnrealHeight, 101.0, 1e-12);

    const double v0 = jumpSpeed();
    expectNear("jump_v0", v0, -std::sqrt(2.0 * 30.0 * 3.5), 1e-9);

    double v = v0;
    const double dMeters = move(v, kTime, kGravity);
    const double dPixels = metersToPixels(dMeters);
    expectNear("tick0_meters", dMeters, v0 * kTime + 0.5 * kGravity * kTime * kTime, 1e-12);
    expectNear("tick0_v", v, v0 + kGravity * kTime, 1e-12);
    expectNear("tick0_pixels", dPixels, dMeters * kPixelScale, 1e-12);
    expectTrue("first_tick_goes_up", dPixels < 0.0);

    // Hang time to v = 0: t = -v0 / G.
    const double hang = -v0 / kGravity;
    expectNear("hang_seconds", hang, 0.483, 0.002);

    double vy = v0;
    double yPx = 0.0;
    int ticks = 0;
    while (vy < 0.0 && ticks < 200) {
        yPx += metersToPixels(move(vy, kTime, kGravity));
        ++ticks;
    }
    expectTrue("apex_ticks_around_48", ticks >= 47 && ticks <= 49);
    expectNear("apex_pixels", yPx, -kUnrealHeight, 2.0);

    expectNear("u_grass_w1", frictionU(1, 1), (kVMax / kT2) / kGravity, 1e-12);
    expectNear("u_ice_w1", frictionU(6, 1), (kVMax / kT1) / kGravity, 1e-12);
    expectNear("u_pipe_w1", frictionU(10, 1), (kVMax / kT3) / kGravity, 1e-12);
    expectNear("u_grass_w3", frictionU(1, 3), (kVMax / kT1) / kGravity, 1e-12);
    expectNear("u_cloud_w3", frictionU(2, 3), (kVMax / kT2) / kGravity, 1e-12);

    expectTrue("ending_w1_93_false", !isEnding(1, 93 * kWidth));
    expectTrue("ending_w1_95_true", isEnding(1, 95 * kWidth));
    expectTrue("ending_w2_needs_104", !isEnding(2, 104 * kWidth) && isEnding(2, 104 * kWidth + 1));

    // Horizontal step at v = V_MAX, a = 0: one tick in pixels.
    double vx = kVMax;
    const double hMeters = move(vx, kTime, 0.0);
    const double hPixels = metersToPixels(hMeters);
    expectNear("vmax_hold", vx, kVMax, 1e-12);
    expectNear("vmax_pixels_per_tick", hPixels, kVMax * kTime * kPixelScale, 1e-12);

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "physics_kinematics: all checks passed\n";
    return 0;
}

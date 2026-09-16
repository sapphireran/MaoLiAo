#include "portable/maoliao_math.hpp"

#include <cmath>
#include <iostream>

// Horizontal brake from Role::action when vX and key-accel have opposite
// signs (or the key is released). Documents how tile id changes stopping
// distance. Air (no floor) sets a1 = 0.

namespace {

int gFailures = 0;

void expect(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

int ticksToStop(double vX, double u, bool onFloor) {
    // Released keys: a = 0, a1 = k * G * u with k = -2 when moving right
    // (Xabs/vX - 3 == -2).
    const double kSign = (vX > 0.0) ? -2.0 : 2.0;
    int ticks = 0;
    while (std::fabs(vX) > 1e-6 && ticks < 5000) {
        const double a1 = onFloor ? kSign * mla::kGravity * u : 0.0;
        const double tmp = vX;
        mla::move(vX, mla::kTime, a1);
        if (tmp * vX < 0.0) {
            vX = 0.0;
        }
        ++ticks;
    }
    return ticks;
}

} // namespace

int main() {
    using namespace mla;

    const double uGrass = frictionU(1, 1);
    const double uSlick = frictionU(6, 1); // id 6, high mu, stops sooner
    const double uSlip = frictionU(10, 1);

    const int grass = ticksToStop(kVMax, uGrass, true);
    const int slick = ticksToStop(kVMax, uSlick, true);
    const int slip = ticksToStop(kVMax, uSlip, true);
    const int air = ticksToStop(kVMax, uGrass, false);

    std::cout << "ticks_to_stop from vX=V_MAX, key released:\n";
    std::cout << "  grass(id1)=" << grass << "\n";
    std::cout << "  slick(id6)=" << slick << "\n";
    std::cout << "  default(id10)=" << slip << "\n";
    std::cout << "  air(no floor)=" << air << " (capped at 5000)\n";

    expect("slick_stops_before_grass", slick < grass);
    expect("grass_stops_before_default", grass < slip);
    expect("air_does_not_stop", air >= 5000);

    // Speed cap: integrating A_ROLE from rest should not stay above V_MAX
    // if the game clamp is applied each tick.
    double v = 0.0;
    for (int i = 0; i < 200; ++i) {
        move(v, kTime, kAccelRole);
        const double abs = v < 0.0 ? -v : v;
        if (abs > kVMax) {
            v = v / abs * kVMax;
        }
    }
    expect("cap_holds", std::fabs(std::fabs(v) - kVMax) < 1e-9);

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "friction_slide: all checks passed\n";
    return 0;
}

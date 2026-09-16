// Twin of Scene::createMap friction switch and a coast-to-stop estimate.

#include "check.h"
#include "define_portable.h"
#include "inertia_portable.h"
#include "map_portable.h"

#include <cmath>
#include <iostream>
#include <vector>

using namespace maoliao;
using maoliao::test::expect;
using maoliao::test::expectNear;

namespace {

double coastPixels(double u) {
    // Game: a1 = k * G * u with k = -2 when vX > 0.
    double v = kVMax;
    double xPx = 0.0;
    for (int i = 0; i < 5000; ++i) {
        const double k = -2.0;
        const double a1 = k * kG * u;
        const double before = v;
        const double metres = inertiaMove(v, kTime, a1);
        if (before * v < 0.0) {
            v = 0.0;
            break;
        }
        xPx += metres * kPixelScale;
        if (std::fabs(v) < 1e-9) {
            break;
        }
    }
    return xPx;
}

}  // namespace

int main() {
    std::cout << "05_level_friction — tile u and coast distance\n";

    const double uIce = frictionU(kT1);
    const double uMid = frictionU(kT2);
    const double uSlip = frictionU(kT3);

    expectNear(uIce, (8.0 / 0.5) / 30.0, 1e-12, "T1 ice");
    expectNear(uMid, (8.0 / 1.2) / 30.0, 1e-12, "T2 medium");
    expectNear(uSlip, (8.0 / 1.5) / 30.0, 1e-12, "T3 slip");

    expectNear(frictionForIdOverworld(1), uMid, 1e-12, "grass");
    expectNear(frictionForIdOverworld(5), uMid, 1e-12, "snow top");
    expectNear(frictionForIdOverworld(6), uIce, 1e-12, "world-1 ice strip");
    expectNear(frictionForIdOverworld(10), uSlip, 1e-12, "pipe default");
    expectNear(frictionForIdOverworld(2), uMid, 1e-12, "cloud overworld");

    expectNear(frictionForIdWorld3(6), uIce, 1e-12, "world3 ice group");
    expectNear(frictionForIdWorld3(2), uMid, 1e-12, "world3 cloud");
    expectNear(frictionForIdWorld3(8), uSlip, 1e-12, "world3 pipe");

    // Mini world-1 strip: grass, ice, pipe — same ids as the start of createMap.
    std::vector<MapCell> w1 = {
        {0, 9, 1, 15, 1, 0.0},
        {67, 9, 6, 4, 1, 0.0},
        {36, 7, 10, 1, 1, 0.0},
    };
    expect(assignFriction(w1.data(), static_cast<int>(w1.size()), 1) == 3, "assigned 3");
    expectNear(w1[0].u, uMid, 1e-12, "assigned grass");
    expectNear(w1[1].u, uIce, 1e-12, "assigned ice");
    expectNear(w1[2].u, uSlip, 1e-12, "assigned pipe");

    const double coastIce = coastPixels(uIce);
    const double coastMid = coastPixels(uMid);
    const double coastSlip = coastPixels(uSlip);

    std::cout << "  u ice/mid/slip = " << uIce << " / " << uMid << " / " << uSlip << "\n";
    std::cout << "  coast px from V_MAX: ice=" << coastIce << " mid=" << coastMid
              << " slip=" << coastSlip << "\n";

    expect(coastIce < coastMid && coastMid < coastSlip, "more u → shorter coast");
    expect(coastIce > 0.0 && coastSlip < 400.0, "coast stays on a short runway");

    expect(isEnding(1, 94 * kWidth) == false, "equal to 94 tiles is not ending");
    expect(isEnding(1, 94 * kWidth + 1), "one pixel past 94 tiles");
    expect(isEnding(2, 104 * kWidth + 1), "world 2 uses 104");
    expect(isEnding(3, 94 * kWidth + 1), "world 3 uses 94");
    expect(!isEnding(2, 94 * kWidth + 1), "world 2 still running at 94");

    // Safer loop length vs the game's `i <= sizeof/sizeof` extra step.
    const MapCell authored[] = {{0, 9, 1, 15, 1, 0.0}, {18, 8, 1, 9, 1, 0.0}};
    const int safe = static_cast<int>(sizeof(authored) / sizeof(authored[0]));
    const int unsafe = safe + 1;
    std::cout << "  authored cells=" << safe << " game-style <= loop would run " << unsafe
              << " times\n";
    expect(safe == 2, "explicit length");
    expect(unsafe == 3, "documents the extra iteration");

    return test::summary("05_level_friction");
}

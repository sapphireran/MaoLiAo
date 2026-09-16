// Hero screen-x is pinned to [0, 192]. Surplus motion becomes -x0 (scroll).
// Sky only eases when the pin is active and vX > 0.

#include "maoliao_core.h"

#include <cmath>
#include <iostream>

using namespace maoliao;

static int gFailures = 0;

static void expectEq(const char* name, int got, int want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << " = " << got << "\n";
    }
}

static void expectNear(const char* name, double got, double want, double eps) {
    if (std::fabs(got - want) > eps) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo camera / parallax ===\n";

    Hero h;
    pinCamera(h, 64);
    expectEq("spawn stays", h.x, 64);
    expectNear("x0 still 0", h.x0, 0.0, 1e-12);

    pinCamera(h, -10);
    expectEq("left wall", h.x, 0);
    expectNear("left kills vX", h.vX, 0.0, 1e-12);

    h.vX = 4.0;
    pinCamera(h, 192 + 40);
    expectEq("pinned at XRIGHT", h.x, kXRight);
    expectNear("world origin slid", h.x0, -40.0, 1e-12);

    // Ending releases the pin so the cat can walk off XSIZE.
    h.ending = true;
    h.x0 = -40.0;
    pinCamera(h, 400);
    expectEq("ending free x", h.x, 400);

    const double step = parallaxStep(8.0);
    expectNear("bg step at Vmax", step, 8.0 * kTime * pixelScale() / 5.0, 1e-12);

    // ASCII: 16 columns, hero column = x / 32, camera window starts at -x0.
    Hero demo;
    demo.x = kXRight;
    demo.x0 = -96;  // 3 tiles of scroll
    std::cout << "window world-x = " << static_cast<int>(-demo.x0) << " .. "
              << static_cast<int>(-demo.x0) + kXSize << "\n";
    std::cout << "hero screen-x  = " << demo.x << " (tile " << demo.x / kWidth << ")\n";

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all camera checks passed\n";
    return 0;
}

#include "aabb.hpp"
#include "constants.hpp"
#include "kinematics.hpp"

#include <cmath>
#include <iostream>

namespace {

int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

bool near(double a, double b, double eps) { return std::fabs(a - b) <= eps; }

}  // namespace

int main() {
    using namespace maoliao;

    std::cout << "MaoLiAo camera_parallax\n";

    // Hero glued to the right rail, running at V_MAX. Each tick the overflow
    // goes into x0; Scene then eases xBg by |vX|*TIME*scale/K_MAP_BG.
    double vX = kVMax;
    int x = kXRight;
    int x0 = 0;
    double xBg = 0.0;
    double xMap = 0.0;

    const int ticks = 50;
    for (int i = 0; i < ticks; ++i) {
        HorizStep step = stepHorizontal(vX, kARole, frictionU(1, false), true);
        vX = step.vX;
        const int oldX0 = x0;
        const int rawX = x + static_cast<int>(step.deltaPixels);
        x = clampCameraX(rawX, x0, false);
        const int xMapNow = x0;
        if (x == kXRight && vX > 0.0 && oldX0 != x0) {
            const double xabs = (vX < 0.0) ? -vX : vX;
            xBg -= xabs * kTime * pixelScale() / kMapBg;
        }
        xMap = static_cast<double>(xMapNow);
    }

    std::cout << "after " << ticks << " ticks at the right rail:\n";
    std::cout << "  screen_x=" << x << " (rail " << kXRight << ")\n";
    std::cout << "  x0=" << x0 << "  xMap=" << xMap << "  xBg=" << xBg << "\n";
    std::cout << "  vX=" << vX << "\n";

    if (x != kXRight) {
        return fail("hero should stay on the rail");
    }
    if (x0 >= 0) {
        return fail("camera origin should scroll negative");
    }
    if (xBg >= 0.0) {
        return fail("background should drift left");
    }

    // Parallax is 1/K_MAP_BG of the map scroll in the original formula, which
    // uses velocity, not Δx0. Map scroll is the integral of pixel velocity.
    const double mapPixels = -x0;
    const double bgPixels = -xBg;
    const double ratio = bgPixels / mapPixels;
    std::cout << "  |xBg|/|x0|=" << ratio << " (authored 1/" << kMapBg << "="
              << (1.0 / kMapBg) << ")\n";
    if (ratio < 0.15 || ratio > 0.25) {
        return fail("parallax ratio not near 1/5");
    }

    // Ending path: rail released, x can exceed XSIZE and mark passed.
    int endX0 = x0;
    int endX = kXRight;
    endX = clampCameraX(endX + 400, endX0, true);
    if (endX < kXSize) {
        return fail("ending walk should be able to leave the window");
    }
    if (endX0 != x0) {
        return fail("ending should not keep eating x0");
    }

    // Sky band pick: yBg = -(world-1)*YSIZE
    for (int world = 1; world <= 3; ++world) {
        const int yBg = -(world - 1) * kYSize;
        std::cout << "  world " << world << " sky yBg=" << yBg << "\n";
        if (yBg != -(world - 1) * 384) {
            return fail("sky band");
        }
    }

    if (!near(pixelScale(), 101.0 / 3.5, 1e-12)) {
        return fail("pixel scale");
    }

    std::cout << "OK\n";
    return 0;
}

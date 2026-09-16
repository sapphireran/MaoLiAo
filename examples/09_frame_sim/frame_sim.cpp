// A few Role::action-shaped ticks using the portable core.
// Not a full port: no enemies, no EasyX. Three phases: fall, walk-to-pin, jump.

#include "maoliao_core.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace maoliao;

struct Tick {
    int n = 0;
    int x = 0;
    int y = 0;
    double x0 = 0;
    double vX = 0;
    double vY = 0;
    bool fly = false;
};

static bool onGrass(const Hero& h, const MapRun& grass) {
    Vec2i corners[4];
    heroWorldCorners(h.x, h.y + 1, static_cast<int>(h.x0), corners);
    const Rect r = tileWorldRect(grass);
    const Vec2i box[2] = {{r.left, r.top}, {r.right, r.bottom}};
    return cornersHit(corners, box);
}

static void stepVertical(Hero& h, const MapRun& grass, bool jumpHeld) {
    if (jumpHeld && !h.isFly) {
        h.isFly = true;
        h.vY = jumpVelocity();
    }
    if (h.isFly) {
        const double meters = integrate(h.vY, kTime, kGravity);
        h.yy += meters * pixelScale();
        h.y = static_cast<int>(h.yy);
        if (onGrass(h, grass) && h.vY > 0.0) {
            h.isFly = false;
            h.vY = 0.0;
            h.y = (h.y + kHeight / 2) / kHeight * kHeight;
            h.yy = h.y;
        }
        if (h.y > kYSize) {
            h.died = true;
        }
    } else if (!onGrass(h, grass)) {
        h.isFly = true;
    }
}

static void stepHorizontal(Hero& h, bool right) {
    const double a = right ? kARole : 0.0;
    const double meters = integrate(h.vX, kTime, a);
    if (std::fabs(h.vX) > kVMax) {
        h.vX = (h.vX / std::fabs(h.vX)) * kVMax;
    }
    pinCamera(h, h.xx + meters * pixelScale());
}

int main() {
    std::cout << "=== MaoLiAo multi-frame sim ===\n";
    MapRun grass{0, 9, 1, 80, 1, frictionU(1, 1)};
    Hero h;
    h.x = kX0;
    h.xx = kX0;
    h.y = 64;
    h.yy = 64;
    h.isFly = true;

    std::vector<Tick> log;
    auto record = [&](int n) {
        log.push_back({n, h.x, h.y, h.x0, h.vX, h.vY, h.isFly});
    };

    int n = 0;
    auto tick = [&](bool right, bool jump) {
        stepVertical(h, grass, jump);
        stepHorizontal(h, right);
        ++n;
    };

    // Phase 1: drop onto row 9. World-1 jump is ignored while airborne.
    record(n);
    tick(false, true);
    while (h.isFly && n < 200) {
        tick(false, false);
    }
    const int landedAt = n;
    record(n);

    // Phase 2: hold right until the camera pin engages.
    while (h.x < kXRight && n < 400) {
        tick(true, false);
    }
    const int pinnedAt = n;
    record(n);

    // Phase 3: jump once, then wait to land again while still running.
    tick(true, true);
    const int jumpedAt = n;
    record(n);
    while (h.isFly && n < 600) {
        tick(true, false);
    }
    record(n);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "n\tx\ty\tx0\tvX\tvY\tfly\n";
    for (const auto& t : log) {
        std::cout << t.n << '\t' << t.x << '\t' << t.y << '\t' << t.x0 << '\t'
                  << t.vX << '\t' << t.vY << '\t' << t.fly << '\n';
    }
    std::cout << "landed_at=" << landedAt << " pinned_at=" << pinnedAt
              << " jumped_at=" << jumpedAt << " end_n=" << n << "\n";

    int failures = 0;
    if (h.died) {
        std::cerr << "FAIL sim died in a pit\n";
        ++failures;
    }
    if (landedAt == 0 || log[1].y != 8 * kHeight || log[1].fly) {
        std::cerr << "FAIL first landing should snap to y=256 on ground\n";
        ++failures;
    }
    if (h.x != kXRight) {
        std::cerr << "FAIL expected camera pin at 192, x=" << h.x << "\n";
        ++failures;
    }
    if (h.x0 >= 0.0) {
        std::cerr << "FAIL expected world to scroll left, x0=" << h.x0 << "\n";
        ++failures;
    }
    if (h.isFly || h.y != 8 * kHeight) {
        std::cerr << "FAIL expected to be grounded on row 8 at the end\n";
        ++failures;
    }
    if (jumpedAt <= pinnedAt) {
        std::cerr << "FAIL jump should happen after the pin\n";
        ++failures;
    }

    if (failures != 0) {
        return 1;
    }
    std::cout << "frame sim settled: y=" << h.y << " x=" << h.x << " x0=" << h.x0 << "\n";
    std::cout << "all frame-sim checks passed\n";
    return 0;
}

#include "portable/maoliao_math.hpp"

#include <iostream>

// Replays the screen-vs-world split from Role::action:
//   * hero.x is clamped to [XLEFT, XRIGHT] = [0, 192] during play
//   * overflow past XRIGHT is subtracted from hero.x0 (the camera)
//   * world X used by isEnding is -x0 + x

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

void expectEq(const char* name, int got, int want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << " = " << got << "\n";
    }
}

struct HeroRail {
    int x = mla::kX0;
    double x0 = 0.0;
    bool ending = false;
    bool passed = false;

    int worldX() const { return -static_cast<int>(x0) + x; }

    void applyScreenX(int newX) {
        x = newX;
        if (x > mla::kXSize) {
            passed = true;
        }
        if (mla::isEnding(1, worldX())) {
            ending = true;
        }
        if (x < mla::kXLeft) {
            x = mla::kXLeft;
        } else if (x > mla::kXRight && !ending) {
            x0 -= (x - mla::kXRight);
            x = mla::kXRight;
        }
    }
};

} // namespace

int main() {
    using namespace mla;

    expectEq("xright", kXRight, 192);
    expectEq("spawn_x", kX0, 64);

    HeroRail h;
    h.applyScreenX(kX0 + 20);
    expectEq("walk_inside_rail_x", h.x, 84);
    expect("camera_still_zero", h.x0 == 0.0);
    expectEq("world_x_inside_rail", h.worldX(), 84);

    h.applyScreenX(300);
    expectEq("clamped_to_rail", h.x, kXRight);
    expectEq("camera_took_overflow", static_cast<int>(h.x0), -(300 - kXRight));
    expectEq("world_x_after_scroll", h.worldX(), 300);

    // Walk far enough to trip world-1 ending (94 * 32 = 3008).
    h.x0 = 0;
    h.x = kXRight;
    h.applyScreenX(kXRight); // no extra overflow this call
    // Simulate camera already at world 3010.
    h.x0 = -(3010 - kXRight);
    h.x = kXRight;
    h.ending = false;
    h.applyScreenX(kXRight);
    expect("ending_latched", h.ending);
    expect("not_passed_until_offscreen", !h.passed);

    // Once ending, the rail no longer eats overflow — hero can walk off 512.
    h.applyScreenX(kXSize + 8);
    expect("passed_when_x_gt_512", h.passed);
    expectEq("ending_keeps_screen_x", h.x, kXSize + 8);

    // Parallax step from Scene::action (only used when on the rail and vX > 0).
    const double vX = kVMax;
    const double bgStep = vX * kTime * kPixelScale / kMapBg;
    expect("parallax_slower_than_map", bgStep < metersToPixels(vX * kTime));
    std::cout << "bgStep_at_vmax=" << bgStep << " px/tick\n";

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "camera_rail: all checks passed\n";
    return 0;
}

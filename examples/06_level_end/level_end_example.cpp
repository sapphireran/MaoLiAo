// Scene::isEnding thresholds and the two-phase clear (ending → passed).

#include "maoliao_core.h"

#include <iostream>

using namespace maoliao;

static int gFailures = 0;

static void expect(const char* name, bool cond) {
    if (!cond) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo level-end distances ===\n";

    expect("w1 94 tiles", endingDistance(1) == 94 * kWidth);
    expect("w2 104 tiles", endingDistance(2) == 104 * kWidth);
    expect("w3 94 tiles", endingDistance(3) == 94 * kWidth);
    expect("unknown world", endingDistance(9) == 0);

    expect("w1 just before", !isEnding(1, 94 * kWidth));
    expect("w1 just after", isEnding(1, 94 * kWidth + 1));
    expect("w2 longer than w1", isEnding(1, 100 * kWidth) && !isEnding(2, 100 * kWidth));

    // Once ending, Role forces CMD_RIGHT until screen x > 512.
    Hero h;
    h.x = kXRight;
    h.x0 = -(94 * kWidth - kXRight);
    const int worldX = -static_cast<int>(h.x0) + h.x;
    expect("world x at pin equals ending", worldX == 94 * kWidth);
    expect("not yet ending (strict >)", !isEnding(1, worldX));

    h.x0 -= 1;  // one more pixel of scroll
    expect("now ending", isEnding(1, -static_cast<int>(h.x0) + h.x));

    h.ending = true;
    h.x = kXSize + 1;
    h.passed = h.x > kXSize;
    expect("passed when x > XSIZE", h.passed);

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all level-end checks passed\n";
    return 0;
}

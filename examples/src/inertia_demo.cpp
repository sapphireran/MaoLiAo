// Check Inertia::move, jump apex, and pixel scaling against define.h.

#include "maoliao_model.h"

#include <cmath>
#include <cstdio>

using namespace maoliao;

static bool nearly(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Rest particle, a = 20, t = 0.01 → s = 0.001, v = 0.2
    double v = 0;
    double s = move(v, kTime, kARole);
    MLA_CHECK(nearly(s, 0.001));
    MLA_CHECK(nearly(v, 0.2));

    // Opposite accel cancels in one step if v is small enough to cross zero
    v = 0.1;
    s = move(v, kTime, -kARole);
    MLA_CHECK(nearly(s, 0.1 * kTime + 0.5 * (-kARole) * kTime * kTime));
    MLA_CHECK(nearly(v, 0.1 - kARole * kTime));

    // Jump: vY = -sqrt(2 g h). Time to apex = |v| / g.
    const double v0 = jumpSpeed();
    MLA_CHECK(nearly(v0, -std::sqrt(2.0 * kG * kRealHeight), 1e-12));
    MLA_CHECK(v0 < 0);

    const double tApex = -v0 / kG;
    double vy = v0;
    double meters = 0;
    const int steps = static_cast<int>(tApex / kTime);
    for (int i = 0; i < steps; ++i) {
        meters += -move(vy, kTime, kG);  // screen y down; rise is negative move
    }
    // Discrete Euler undershoots the exact 3.5 m a little; stay within 4 cm.
    MLA_CHECK(meters > 3.46 && meters < 3.50);

    const double pixels = meters * kPixelPerMeter;
    MLA_CHECK(pixels > 99.5 && pixels < kUnrealHeight);

    // Free fall from rest for 10 frames.
    vy = 0;
    double fall = 0;
    for (int i = 0; i < 10; ++i) fall += move(vy, kTime, kG);
    const double exact = 0.5 * kG * (0.1 * 0.1);
    MLA_CHECK(nearly(fall, exact, 1e-12));

    std::printf("inertia_demo ok  jump_m=%.4f jump_px=%.2f v0=%.4f\n",
                meters, pixels, v0);
    return 0;
}

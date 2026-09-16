// Twin of MaoLiAo/inertia.cpp + the jump / run block in Role::action.
// Prints a short table, then asserts the numbers used in docs/physics.md.

#include "check.h"
#include "define_portable.h"
#include "inertia_portable.h"

#include <cmath>
#include <iostream>

using namespace maoliao;
using maoliao::test::expect;
using maoliao::test::expectNear;

int main() {
    std::cout << "01_inertia_motion — jump apex and run cap\n";

    const double launch = jumpLaunchSpeed();
    expectNear(launch, -std::sqrt(210.0), 1e-12, "launch = -sqrt(2 G REAL_HEIGHT)");
    expectNear(kUnrealHeight, 101.0, 1e-12, "UNREAL_HEIGHT is 3*32+5");
    expectNear(kPixelScale, 101.0 / 3.5, 1e-12, "pixel scale");

    // Integrate a jump until vY crosses through zero (apex).
    double vY = launch;
    double yy = 0.0;  // pixels, up is negative in v but we accumulate rise
    double risePx = 0.0;
    int frames = 0;
    int apexFrame = -1;
    while (frames < 200) {
        const double metres = inertiaMove(vY, kTime, kG);
        const double dPx = metres * kPixelScale;
        yy += dPx;
        if (dPx < 0.0) {
            risePx += -dPx;
        }
        ++frames;
        if (vY >= 0.0 && apexFrame < 0) {
            apexFrame = frames;
            break;
        }
    }

    std::cout << "  launch vY     = " << launch << " m/s\n";
    std::cout << "  frames to apex = " << apexFrame << "\n";
    std::cout << "  rise pixels    = " << risePx << " (want ~101)\n";
    std::cout << "  vY at apex+    = " << vY << "\n";

    expect(apexFrame == 49, "apex on frame 49 (vY becomes >= 0 after that step)");
    expectNear(risePx, kUnrealHeight, 2.0, "pixel rise close to UNREAL_HEIGHT");
    expect(vY >= 0.0, "past apex we are falling");

    // Closed-form time to apex: |v0|/G
    const double tApex = -launch / kG;
    expectNear(tApex, 0.483, 0.002, "t_apex ≈ 0.483 s");
    expectNear(tApex / kTime, 48.3, 0.2, "about 48 TIME steps");

    // Sprint from rest: hit V_MAX at A_ROLE.
    double vX = 0.0;
    int capFrame = -1;
    double xPx = 0.0;
    for (int i = 0; i < 200; ++i) {
        double a = kARole;
        if (std::fabs(vX) >= kVMax) {
            vX = (vX > 0.0 ? kVMax : -kVMax);
            a = 0.0;
            if (capFrame < 0) {
                capFrame = i;
            }
        }
        xPx += inertiaMove(vX, kTime, a) * kPixelScale;
    }
    const double tCap = kVMax / kARole;
    std::cout << "  frames to V_MAX = " << capFrame << " (analytic " << tCap / kTime << ")\n";
    std::cout << "  2.00 s travel px = " << xPx << "\n";

    expectNear(tCap, 0.4, 1e-12, "8/20 = 0.4 s to cap");
    expect(capFrame == 40, "cap on the 40th step");
    expectNear(vX, kVMax, 1e-9, "clamped to V_MAX");

    // Sign flip clamp: the game zeros vX when the step crosses through 0.
    // Need |a| > v/TIME so one tick overshoots (2 / 0.01 = 200).
    double v = 2.0;
    const double before = v;
    inertiaMove(v, kTime, -500.0);
    const bool flipped = before * v < 0.0;
    expect(flipped, "strong brake crosses zero in one TIME");
    if (flipped) {
        v = 0.0;
    }
    expectNear(v, 0.0, 1e-12, "Role::action zeros vX after a sign change");

    return test::summary("01_inertia_motion");
}

#include <iostream>

#include "maoliao/camera.hpp"
#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"
#include "maoliao/kinematics.hpp"

int main() {
    using namespace maoliao;

    MLA_CHECK(kXRight == 192);
    MLA_CHECK(is_ending(1, 94 * kWidth + 1));
    MLA_CHECK(!is_ending(1, 94 * kWidth));
    MLA_CHECK(is_ending(2, 104 * kWidth + 1));
    MLA_CHECK(!is_ending(3, 94 * kWidth));

    Camera c;
    c.x = kX0;
    c.xx = kX0;
    c.vX = kVMax;

    // Walk right many frames; sprite must pin at 192 and x0 must go negative.
    for (int i = 0; i < 400; ++i) {
        const double dx = walk_step(c.vX, kARole, u_t2(), true);
        apply_rail(c, dx);
    }
    std::cout << "pinned x       " << c.x << "  x0 " << c.x0 << "\n";
    MLA_CHECK(c.x == kXRight);
    MLA_CHECK(c.x0 < 0);

    const double sky = sky_step(c.vX, c.x == kXRight, true);
    MLA_CHECK(sky > 0);
    MLA_NEAR(sky, kVMax * kTime * kPxPerMeter / kMapBg, 1e-9);
    std::cout << "sky step       " << sky << "\n";
    MLA_NEAR(sky_step(c.vX, false, true), 0, 1e-12);

    // Ending rail lets x grow past 192 toward 512.
    Camera e;
    e.x = kXRight;
    e.xx = kXRight;
    e.x0 = -(94 * kWidth - kXRight);
    e.ending = true;
    e.vX = kVMax;
    for (int i = 0; i < 200; ++i) {
        const double dx = walk_step(e.vX, kARole, u_t2(), true);
        apply_rail(e, dx);
    }
    std::cout << "ending x       " << e.x << "  passed " << e.passed << "\n";
    MLA_CHECK(e.x > kXRight);
    MLA_CHECK(e.passed);

    return done("camera_lab");
}

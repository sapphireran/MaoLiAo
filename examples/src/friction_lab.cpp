#include <cmath>
#include <iostream>

#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"
#include "maoliao/kinematics.hpp"

int main() {
    using namespace maoliao;

    MLA_NEAR(u_t1(), (8.0 / 0.5) / 30.0, 1e-12);
    MLA_NEAR(u_t2(), (8.0 / 1.2) / 30.0, 1e-12);
    MLA_NEAR(u_t3(), (8.0 / 1.5) / 30.0, 1e-12);

    MLA_CHECK(friction_k(3.0) == -2);
    MLA_CHECK(friction_k(-3.0) == 2);

    // Coasting right on grass (T2) with no walk input: a_walk = 0, so
    // v*a <= 0 and friction fires. |k|=2 doubles textbook μg.
    double v = kVMax;
    int frames = 0;
    while (v > 1e-9 && frames < 2000) {
        walk_step(v, 0.0, u_t2(), true);
        ++frames;
    }
    std::cout << "coast T2 from V_MAX -> 0 in " << frames << " frames\n";
    MLA_CHECK(frames > 10 && frames < 80);

    // T1 (id 6 slab) should stop sooner than T2.
    double v1 = kVMax;
    double v2 = kVMax;
    int f1 = 0, f2 = 0;
    while (v1 > 1e-9 && f1 < 2000) {
        walk_step(v1, 0.0, u_t1(), true);
        ++f1;
    }
    while (v2 > 1e-9 && f2 < 2000) {
        walk_step(v2, 0.0, u_t2(), true);
        ++f2;
    }
    std::cout << "coast T1 frames " << f1 << "  T2 frames " << f2 << "\n";
    MLA_CHECK(f1 < f2);

    // Airborne: on_floor=false → no friction, speed holds.
    double air = 4.0;
    walk_step(air, 0.0, u_t1(), false);
    MLA_NEAR(air, 4.0, 1e-12);

    // Hold right on grass: should saturate at V_MAX.
    double run = 0;
    for (int i = 0; i < 200; ++i) {
        walk_step(run, kARole, u_t2(), true);
    }
    MLA_NEAR(std::fabs(run), kVMax, 1e-9);
    std::cout << "held-right vX  " << run << "\n";

    return done("friction_lab");
}

#include "maoliao_model.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

int main() {
    std::cout << std::setprecision(10);
    std::cout << "constants\n";
    std::cout << "  TIME=" << TIME << " G=" << G << " V_MAX=" << V_MAX << " A_ROLE=" << A_ROLE << "\n";
    std::cout << "  REAL_HEIGHT=" << REAL_HEIGHT << " UNREAL_HEIGHT=" << UNREAL_HEIGHT
              << " scale=" << (UNREAL_HEIGHT / REAL_HEIGHT) << "\n";
    std::cout << "  u(T1)=" << friction_u(T1) << " u(T2)=" << friction_u(T2)
              << " u(T3)=" << friction_u(T3) << "\n";

    if (std::fabs(friction_u(T1) - (V_MAX / T1) / G) > 1e-12) return fail("u(T1)");
    if (std::fabs(friction_u(T2) - (8.0 / 1.2) / 30.0) > 1e-12) return fail("u(T2)");
    if (std::fabs(tile_u_world12(6) - friction_u(T1)) > 1e-12) return fail("pipe uses T1");
    if (std::fabs(tile_u_world12(1) - friction_u(T2)) > 1e-12) return fail("grass uses T2");
    if (std::fabs(tile_u_world3(5) - friction_u(T1)) > 1e-12) return fail("world3 snow uses T1");
    if (std::fabs(tile_u_world3(2) - friction_u(T2)) > 1e-12) return fail("world3 cloud stays T2");

    if (std::fabs(friction_k(4.0) + 2.0) > 1e-12) return fail("k(+v) should be -2");
    if (std::fabs(friction_k(-4.0) - 2.0) > 1e-12) return fail("k(-v) should be +2");

    // Coasting right on grass: input a=0, so friction applies.
    double v = 8.0;
    const double u = friction_u(T2);
    const double a1 = friction_k(v) * G * u; // -2 * 30 * 0.2222 = -13.333
    const double step = integrate(v, TIME, a1);
    std::cout << "coast from V_MAX on T2: a1=" << a1 << " dx=" << step << " v'=" << v << "\n";
    if (a1 >= 0.0) return fail("rightward coast friction must be negative");
    if (v >= 8.0) return fail("speed should drop");

    // One run tick from rest with D held, no friction (v*a > 0 after start: v==0 so no a1).
    v = 0.0;
    const double go = integrate(v, TIME, A_ROLE);
    std::cout << "first run tick: dx=" << go << " v=" << v << " px=" << metres_to_pixels(go) << "\n";
    const double expect_v = A_ROLE * TIME;
    if (std::fabs(v - expect_v) > 1e-12) return fail("v = a t from rest");
    if (std::fabs(go - 0.5 * A_ROLE * TIME * TIME) > 1e-12) return fail("x = 1/2 a t^2 from rest");

    // Pixel step at cap: 8 * 0.01 * 101/3.5 ≈ 2.31 px per tick if v already V_MAX and a nets 0.
    const double px_at_cap = metres_to_pixels(V_MAX * TIME);
    std::cout << "pixels per tick at V_MAX (a=0): " << px_at_cap << "\n";
    if (px_at_cap < 2.2 || px_at_cap > 2.4) return fail("cap step should be ~2.3 px");

    std::cout << "inertia_kinematics: ok\n";
    return 0;
}

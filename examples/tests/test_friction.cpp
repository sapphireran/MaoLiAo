#include "check.hpp"
#include "maoliao.hpp"

int main() {
    using namespace maoliao;

    check_near(u_for_surface(Surface::High), (8.0 / 0.5) / 30.0, 1e-12, "T1");
    check_near(u_for_surface(Surface::Mid), (8.0 / 1.2) / 30.0, 1e-12, "T2");
    check_near(u_for_surface(Surface::Low), (8.0 / 1.5) / 30.0, 1e-12, "T3");

    CHECK(surface_for_tile(1, 1) == Surface::Mid);
    CHECK(surface_for_tile(6, 1) == Surface::High);
    CHECK(surface_for_tile(11, 1) == Surface::Low);
    CHECK(surface_for_tile(1, 3) == Surface::High);
    CHECK(surface_for_tile(2, 3) == Surface::Mid);
    CHECK(surface_for_tile(7, 3) == Surface::Low);

    CHECK(friction_direction_k(-4.0) == 2);
    CHECK(friction_direction_k(4.0) == -2);
    CHECK(friction_direction_k(0.0) == 0);

    const double u = u_for_surface(Surface::Mid);
    check_near(friction_accel(4.0, u, false), -2.0 * 30.0 * u, 1e-12, "brake R");
    check_near(friction_accel(-4.0, u, false), 2.0 * 30.0 * u, 1e-12, "brake L");
    check_near(friction_accel(4.0, u, true), 0.0, 1e-12, "air");
    return report("test_friction");
}

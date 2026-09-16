#include <cmath>
#include <iomanip>
#include <iostream>

#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"
#include "maoliao/kinematics.hpp"

int main() {
    using namespace maoliao;

    const double v0 = jump_speed();
    MLA_NEAR(v0, -std::sqrt(210.0), 1e-12);
    MLA_NEAR(kPxPerMeter, 101.0 / 3.5, 1e-12);
    MLA_NEAR(kUnrealHeight, 101.0, 1e-12);

    const JumpApex apex = jump_apex();
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "jump v0        " << v0 << " m/s\n";
    std::cout << "px per meter   " << kPxPerMeter << "\n";
    std::cout << "discrete apex  " << apex.meters << " m / "
              << apex.pixels << " px  in " << apex.frames << " frames\n";

    // Continuous energy says 3.5 m / 101 px. The 10 ms step undershoots.
    MLA_CHECK(apex.meters < kRealHeight);
    MLA_CHECK(apex.meters > 3.499);
    MLA_NEAR(apex.pixels, meters_to_px(apex.meters), 1e-9);
    MLA_CHECK(apex.frames > 40 && apex.frames < 60);

    // First frame from spawn y = 64, matching Role::air path.
    double vY = v0;
    double yy = static_cast<double>(kSpawnY);
    air_step(yy, vY);
    std::cout << "first air yy   " << yy << "  vY " << vY << "\n";
    MLA_CHECK(yy < kSpawnY);
    MLA_CHECK(vY > v0);  // gravity already applied

    return done("jump_lab");
}

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
    const double v0 = jump_speed();
    const double expect = -std::sqrt(210.0);
    if (std::fabs(v0 - expect) > 1e-12) return fail("takeoff speed is -sqrt(2 G REAL_HEIGHT)");

    auto samples = simulate_jump();
    const auto& top = apex(samples);

    std::cout << "jump takeoff vY = " << std::setprecision(12) << v0 << " m/s\n";
    std::cout << "tick  vY            metres        pixels\n";
    for (const auto& s : samples) {
        if (s.tick <= 5 || s.tick == top.tick || s.tick + 1 == static_cast<int>(samples.size())) {
            std::cout << std::setw(4) << s.tick << "  " << std::setw(13) << s.v << "  "
                      << std::setw(12) << s.metres << "  " << std::setw(12) << s.pixels << "\n";
        } else if (s.tick == 6) {
            std::cout << " ...\n";
        }
    }

    std::cout << "apex tick=" << top.tick << " metres=" << top.metres
              << " pixels=" << top.pixels << "\n";

    // Closed form: peak at t = -v0/G, height = -REAL_HEIGHT (up is negative metres).
    if (top.tick < 40 || top.tick > 55) return fail("apex should be near t = sqrt(210)/30 ~ 48 ticks");
    if (std::fabs(top.metres + REAL_HEIGHT) > 0.02) return fail("apex should be ~ -3.5 m");
    if (std::fabs(top.pixels + UNREAL_HEIGHT) > 0.6) return fail("apex should be ~ -101 px");

    // First tick matches the worked table in docs/physics.md.
    const double first = samples[1].metres;
    const double expect_first = v0 * TIME + 0.5 * G * TIME * TIME;
    if (std::fabs(first - expect_first) > 1e-12) return fail("first-tick integrator mismatch");

    std::cout << "jump_trajectory: ok\n";
    return 0;
}

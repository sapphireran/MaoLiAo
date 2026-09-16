#include "maoliao.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    const double v0 = jump_launch_speed();
    const double t_apex = apex_time(v0);
    const double real_apex = real_height(v0, t_apex);
    const double pixel_apex = pixel_height(v0, t_apex);

    std::cout << "Cat Mario jump (Role::action + Inertia::move)\n";
    std::cout << "  v0            = " << v0 << " m/s\n";
    std::cout << "  apex time     = " << t_apex << " s\n";
    std::cout << "  real apex     = " << real_apex << " m\n";
    std::cout << "  pixel apex    = " << pixel_apex << " px\n";
    std::cout << "  authored      = " << -kRealJumpMeters << " m / "
              << -kPixelJumpHeight << " px\n\n";

    double vy = v0;
    double pixel_y = 0.0;
    std::cout << std::setw(6) << "tick" << std::setw(10) << "t" << std::setw(12)
              << "vy" << std::setw(12) << "real_y" << std::setw(12) << "px_y"
              << '\n';

    for (int i = 1; i <= 60; ++i) {
        const auto s = sample_jump_tick(i, vy, pixel_y);
        if (i <= 8 || i == 48 || i == 49 || i == 60) {
            std::cout << std::setw(6) << s.tick << std::setw(10) << std::fixed
                      << std::setprecision(3) << s.time << std::setw(12)
                      << std::setprecision(4) << s.vy << std::setw(12)
                      << s.real_y << std::setw(12) << s.pixel_y << '\n';
        }
    }

    const bool ok = std::fabs(real_apex + kRealJumpMeters) < 1e-9 &&
                    std::fabs(pixel_apex + kPixelJumpHeight) < 1e-9;
    std::cout << '\n' << (ok ? "apex matches define.h\n" : "apex mismatch\n");
    return ok ? 0 : 1;
}

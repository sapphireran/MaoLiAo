#include "constants.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>

// Matches Inertia::move in MaoLiAo/inertia.cpp.
static double move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

int main() {
    const double scale = pixelsPerMetre();
    double vy = -std::sqrt(2.0 * kG * kRealHeight);
    double y_m = 0.0;
    double y_px = 0.0;

    std::cout << std::setprecision(10) << std::fixed;
    std::cout << "vY0            " << vy << "\n";
    std::cout << "G              " << kG << "\n";
    std::cout << "TIME           " << kTime << "\n";
    std::cout << "pixels/metre   " << scale << "\n";
    std::cout << "u T1/T2/T3     " << frictionU(kT1) << "  " << frictionU(kT2) << "  "
              << frictionU(kT3) << "\n\n";
    std::cout << "tick  t_s        vY            y_metres       y_pixels\n";

    std::cout << std::setw(4) << 0 << "  " << std::setw(8) << 0.0 << "  " << std::setw(12) << vy
              << "  " << std::setw(12) << y_m << "  " << std::setw(12) << y_px << "\n";

    constexpr int kTicks = 40;
    for (int tick = 1; tick <= kTicks; ++tick) {
        const double disp = move(vy, kTime, kG);
        y_m += disp;
        y_px += disp * scale;
        std::cout << std::setw(4) << tick << "  " << std::setw(8) << tick * kTime << "  "
                  << std::setw(12) << vy << "  " << std::setw(12) << y_m << "  " << std::setw(12)
                  << y_px << "\n";
    }
    return 0;
}

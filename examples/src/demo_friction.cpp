#include "maoliao.hpp"

#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    std::cout << "Surface mu = (V_MAX / T) / G\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  high T1=" << kFrictionTimeHigh
              << "  u=" << u_for_surface(Surface::High) << '\n';
    std::cout << "  mid  T2=" << kFrictionTimeMid
              << "  u=" << u_for_surface(Surface::Mid) << '\n';
    std::cout << "  low  T3=" << kFrictionTimeLow
              << "  u=" << u_for_surface(Surface::Low) << '\n';

    std::cout << "\nTile id -> surface (world 1 vs world 3)\n";
    std::cout << std::setw(6) << "id" << std::setw(16) << "name" << std::setw(10)
              << "w1" << std::setw(10) << "w3" << std::setw(12) << "u_w1"
              << std::setw(12) << "u_w3" << '\n';
    for (int id = 1; id <= 14; ++id) {
        std::cout << std::setw(6) << id << std::setw(16) << tile_name(id)
                  << std::setw(10)
                  << (surface_for_tile(id, 1) == Surface::High
                          ? "high"
                          : surface_for_tile(id, 1) == Surface::Mid ? "mid"
                                                                   : "low")
                  << std::setw(10)
                  << (surface_for_tile(id, 3) == Surface::High
                          ? "high"
                          : surface_for_tile(id, 3) == Surface::Mid ? "mid"
                                                                   : "low")
                  << std::setw(12) << friction_for_tile(id, 1) << std::setw(12)
                  << friction_for_tile(id, 3) << '\n';
    }

    const double u = u_for_surface(Surface::Mid);
    std::cout << "\nBrake accel on grass, |vX|=4: left "
              << friction_accel(-4.0, u, false) << "  right "
              << friction_accel(4.0, u, false) << "  air "
              << friction_accel(4.0, u, true) << '\n';
    return 0;
}

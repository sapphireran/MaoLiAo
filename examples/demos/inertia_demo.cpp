#include "maoliao/constants.h"
#include "maoliao/inertia.h"

#include <cmath>
#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;

    std::cout << "Inertia::move reconstruction\n";
    std::cout << "dt=" << kTime << "  G=" << kG << "  scale=" << kPxPerMetre
              << " px/m\n\n";

    double v = 0;
    const double a = kARole;
    std::cout << "From rest, a=" << a << " (run accel)\n";
    std::cout << "step  v_model     dx_m       dx_px\n";
    for (int i = 1; i <= 12; ++i) {
        const double dx = inertia_move(v, kTime, a);
        std::cout << std::setw(4) << i << ' ' << std::fixed << std::setprecision(5)
                  << std::setw(10) << v << ' ' << std::setw(10) << dx << ' '
                  << std::setw(10) << dx * kPxPerMetre << '\n';
    }

    const int steps_to_vmax = static_cast<int>(std::ceil(kVMax / (kARole * kTime)));
    std::cout << "\nSteps to reach V_MAX at A_ROLE: " << steps_to_vmax
              << " (t=" << steps_to_vmax * kTime << "s)\n";

    double v2 = kVMax;
    const double ice_u = friction_u(kT1);
    const double a1 = -2.0 * kG * ice_u;
    int n = 0;
    while (v2 > 0 && n < 1000) {
        const double tmp = v2;
        inertia_move(v2, kTime, a1);
        if (tmp * v2 < 0) {
            v2 = 0;
        }
        ++n;
    }
    std::cout << "Ice slide-to-stop from V_MAX: " << n << " steps (" << n * kTime
              << "s), |a1|=" << -a1 << "\n";
    return 0;
}

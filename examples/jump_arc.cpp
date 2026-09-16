#include "portable/maoliao_math.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

// Prints the first 50 ticks of a world-1 jump (no ceiling). Useful when
// retuning G or REAL_HEIGHT: the apex should stay near -101 px.

int main() {
    using namespace mla;

    double vY = jumpSpeed();
    double y = 0.0;
    double minY = 0.0;
    int apexTick = 0;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "tick\tvy\tdy_px\ty_px\n";

    for (int tick = 0; tick < 50; ++tick) {
        const double meters = move(vY, kTime, kGravity);
        const double dy = metersToPixels(meters);
        y += dy;
        if (y < minY) {
            minY = y;
            apexTick = tick;
        }
        std::cout << tick << '\t' << vY << '\t' << dy << '\t' << y << '\n';
    }

    std::cout << "apex_tick=" << apexTick << " min_y=" << minY
              << " expected~=" << -kUnrealHeight << "\n";

    if (std::fabs(minY + kUnrealHeight) > 2.5) {
        std::cerr << "FAIL apex drifted from UNREAL_HEIGHT\n";
        return 1;
    }
    std::cout << "PASS jump_arc apex\n";
    return 0;
}

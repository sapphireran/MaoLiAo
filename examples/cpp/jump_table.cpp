#include "maoliao_kit.hpp"

#include <iomanip>
#include <iostream>

int main() {
    using namespace maoliao;
    const auto samples = jump_profile();
    const auto top = apex(samples);
    std::cout << std::fixed;
    std::cout << "launch vY     " << std::setprecision(9) << launch_vy() << "\n";
    std::cout << "pixel scale   " << pixel_scale() << "\n";
    std::cout << "apex          " << std::setprecision(4) << top.rise_px
              << " px at frame " << top.frame << "\n";
    std::cout << "hang frames   " << samples.size() << "\n";
    std::cout << "UNREAL_HEIGHT " << UNREAL_HEIGHT << "\n";
    return 0;
}

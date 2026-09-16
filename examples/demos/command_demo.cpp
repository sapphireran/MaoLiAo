#include "maoliao/commands.h"
#include "maoliao/constants.h"

#include <iostream>

int main() {
    using namespace maoliao;

    const int run_jump = pack_commands(false, true, true, false, false, false);
    const int pause = pack_commands(false, false, false, false, false, true);
    const int all = pack_commands(true, true, true, true, true, true);

    std::cout << "D+W  mask=" << run_jump << " " << describe_commands(run_jump) << "\n";
    std::cout << "Esc  mask=" << pause << " " << describe_commands(pause) << "\n";
    std::cout << "all  mask=" << all << " " << describe_commands(all) << "\n";
    std::cout << "none " << describe_commands(0) << "\n";

    std::cout << "\nSticky getKey model (mask updates only on console hit):\n";
    int mask = pack_commands(false, true, false, false, false, false);
    std::cout << "  hold D, console hit -> " << describe_commands(mask) << "\n";
    mask = sticky_update(mask, pack_commands(false, true, false, false, false, false),
                         false);
    std::cout << "  still holding D, no console event -> " << describe_commands(mask)
              << " (stuck)\n";
    mask = sticky_update(mask, 0, false);
    std::cout << "  released D physically, still no console event -> "
              << describe_commands(mask) << " (still RIGHT)\n";
    mask = sticky_update(mask, 0, true);
    std::cout << "  any console hit with keys up -> " << describe_commands(mask)
              << "\n";

    std::cout << "\nPause virtual keys: RETURN=" << kVirReturn
              << " RESTART=" << kVirRestart << " HOME=" << kVirHome << "\n";
    return 0;
}

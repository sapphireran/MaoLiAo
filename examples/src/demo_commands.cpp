#include "maoliao.hpp"

#include <iostream>

int main() {
    using namespace maoliao;

    const int walk_jump_shoot = command_mask({Key::A, Key::D, Key::W, Key::J});
    std::cout << "A|D|W|J mask = " << walk_jump_shoot << " (expect "
              << (kCmdLeft | kCmdRight | kCmdUp | kCmdShoot) << ")\n";
    std::cout << "decoded:";
    for (const auto& label : decode_mask(walk_jump_shoot)) {
        std::cout << ' ' << label;
    }
    std::cout << '\n';

    const int pause = kVirRestart;
    std::cout << "VIR_RESTART virtual-only: " << is_virtual_only(pause) << '\n';
    std::cout << "W and K share CMD_UP: " << (bit_for_key(Key::W) == bit_for_key(Key::K))
              << '\n';
    std::cout << "A+D cancels in Role::action because both ±A_ROLE apply\n";
    return 0;
}

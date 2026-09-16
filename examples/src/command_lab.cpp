#include <iostream>

#include "maoliao/check.hpp"
#include "maoliao/constants.hpp"
#include "maoliao/input.hpp"

int main() {
    using namespace maoliao;

    MLA_CHECK(kCmdLeft == 1);
    MLA_CHECK(kCmdRight == 2);
    MLA_CHECK(kCmdUp == 4);
    MLA_CHECK(kCmdDown == 8);
    MLA_CHECK(kCmdShoot == 16);
    MLA_CHECK(kCmdEsc == 32);
    MLA_CHECK(kVirReturn == 64);
    MLA_CHECK(kVirRestart == 128);
    MLA_CHECK(kVirHome == 256);

    const int walk_jump = compose({kCmdRight, kCmdUp});
    MLA_CHECK(walk_jump == (2 | 4));
    const auto names = decode_bits(walk_jump);
    MLA_CHECK(names.size() == 2);
    MLA_CHECK(names[0] == "RIGHT");
    MLA_CHECK(names[1] == "UP");

    const int pause_home = compose({kCmdEsc, kVirHome});
    MLA_CHECK(pause_home == (32 | 256));
    MLA_CHECK(decode_bits(kVirRestart)[0] == "RESTART");

    // Distinct bits — no overlap in the homework mask.
    int seen = 0;
    int n = 0;
    const KeyName* t = key_table(n);
    for (int i = 0; i < n; ++i) {
        MLA_CHECK((seen & t[i].bit) == 0);
        seen |= t[i].bit;
    }

    std::cout << "RIGHT|UP mask  " << walk_jump << "\n";
    std::cout << "bitfield used  " << seen << "\n";
    return done("command_lab");
}

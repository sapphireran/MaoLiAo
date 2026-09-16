#include "portable/commands.hpp"

#include <iostream>

namespace {

int gFailures = 0;

void expectEq(const char* name, int got, int want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << " = " << got << "\n";
    }
}

void expect(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

} // namespace

int main() {
    using namespace mla;

    expectEq("idle", commandFromChars(""), 0);
    expectEq("left", commandFromChars("A"), kCmdLeft);
    expectEq("right", commandFromChars("D"), kCmdRight);
    expectEq("jump_w", commandFromChars("W"), kCmdUp);
    expectEq("jump_k", commandFromChars("K"), kCmdUp);
    expectEq("shoot", commandFromChars("J"), kCmdShoot);
    expectEq("esc", commandFromChars("Q"), kCmdEsc);

    const int runJump = commandFromChars("DW");
    expectEq("right_jump_mask", runJump, kCmdRight | kCmdUp);
    expect("chord_right", wantsRight(runJump));
    expect("chord_jump", wantsJump(runJump));
    expect("chord_not_left", !wantsLeft(runJump));

    const int full = commandFromChars("ADWKSJ");
    expectEq("all_gameplay_bits", full,
             kCmdLeft | kCmdRight | kCmdUp | kCmdDown | kCmdShoot);

    // Virtual pause results are not key bits; they sit in the same int.
    expect("vir_distinct", kVirReturn != kCmdEsc);
    expect("vir_restart_bit", kVirRestart == 128);
    expect("vir_home_bit", kVirHome == 256);
    expect("restart_not_a_move", !wantsRight(kVirRestart) && !wantsJump(kVirRestart));

    // Role::action uses (KEY & CMD_UP) && !isFly || world==3.
    const bool groundedJump = wantsJump(commandFromChars("W")) && true && false == false;
    expect("grounded_jump_allowed", groundedJump);

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "command_bits: all checks passed\n";
    return 0;
}

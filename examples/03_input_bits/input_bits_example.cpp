// GetCommand bit packing. Combinations matter because Role::action
// tests each mask independently (A+D together nets out accel).

#include "maoliao_core.h"

#include <iostream>

using namespace maoliao;

static int gFailures = 0;

static void expectEq(const char* name, int got, int want) {
    if (got != want) {
        std::cerr << "FAIL " << name << " got " << got << " want " << want << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << " = " << got << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo command bits ===\n";

    expectEq("idle", packCommand(0, 0, 0, 0, 0, 0), 0);
    expectEq("left", packCommand(1, 0, 0, 0, 0, 0), kCmdLeft);
    expectEq("right", packCommand(0, 1, 0, 0, 0, 0), kCmdRight);
    expectEq("jump W/K", packCommand(0, 0, 1, 0, 0, 0), kCmdUp);
    expectEq("shoot", packCommand(0, 0, 0, 0, 1, 0), kCmdShoot);
    expectEq("esc", packCommand(0, 0, 0, 0, 0, 1), kCmdEsc);

    const int runJump = packCommand(0, 1, 1, 0, 0, 0);
    expectEq("D+W", runJump, kCmdRight | kCmdUp);
    if (!hasCommand(runJump, kCmdRight) || !hasCommand(runJump, kCmdUp)) {
        std::cerr << "FAIL mask tests on D+W\n";
        ++gFailures;
    }

    const int everything = packCommand(1, 1, 1, 1, 1, 1);
    expectEq("all keys", everything, 1 + 2 + 4 + 8 + 16 + 32);

    // Virtual keys replace, they do not or, in Control::pauseClick.
    expectEq("VIR_RETURN", kVirReturn, 64);
    expectEq("VIR_RESTART", kVirRestart, 128);
    expectEq("VIR_HOME", kVirHome, 256);

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all input-bit checks passed\n";
    return 0;
}

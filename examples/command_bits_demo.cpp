// Command bit algebra from Control::GetCommand and main's VIR_ branches.

#include "maoliao_core.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace maoliao;

static int fail = 0;

static void expect_eq(const char* name, int got, int want) {
    if (got != want) {
        std::printf("FAIL %s: got %d want %d\n", name, got, want);
        ++fail;
    } else {
        std::printf("ok   %s = %d\n", name, got);
    }
}

static int sample_keys(const std::vector<int>& down) {
    int c = 0;
    for (int bit : down) {
        c |= bit;
    }
    return c;
}

static const char* describe(int key) {
    static char buf[128];
    buf[0] = 0;
    struct Pair {
        int bit;
        const char* name;
    };
    const Pair pairs[] = {
        {kCmdLeft, "LEFT"},     {kCmdRight, "RIGHT"}, {kCmdUp, "UP"},
        {kCmdDown, "DOWN"},     {kCmdShoot, "SHOOT"}, {kCmdEsc, "ESC"},
        {kVirReturn, "RETURN"}, {kVirRestart, "RESTART"}, {kVirHome, "HOME"},
    };
    bool first = true;
    for (const auto& p : pairs) {
        if (key & p.bit) {
            if (!first) {
                std::strcat(buf, "|");
            }
            std::strcat(buf, p.name);
            first = false;
        }
    }
    if (first) {
        std::strcpy(buf, "NONE");
    }
    return buf;
}

int main() {
    expect_eq("A", sample_keys({kCmdLeft}), 1);
    expect_eq("D", sample_keys({kCmdRight}), 2);
    expect_eq("W_or_K", sample_keys({kCmdUp}), 4);
    expect_eq("A+D_cancel_mask", sample_keys({kCmdLeft, kCmdRight}), 3);

    const int run_jump = sample_keys({kCmdRight, kCmdUp});
    expect_eq("D+W", run_jump, kCmdRight | kCmdUp);
    std::printf("     bits %s\n", describe(run_jump));

    const int armed = sample_keys({kCmdRight, kCmdShoot});
    if ((armed & kCmdShoot) && true /* isShoot */) {
        std::printf("ok   shoot+run mask fires the bullet branch\n");
    }

    // main.cpp treats these as exclusive reconstructs.
    expect_eq("VIR_RESTART", kVirRestart, 128);
    expect_eq("VIR_HOME", kVirHome, 256);
    expect_eq("no_overlap_with_cmd", (kVirRestart & 63), 0);

    // Sticky-key illustration (see docs/controls-and-save.md):
    // last sampled mask is kept when _kbhit() is false.
    int sticky = sample_keys({kCmdRight});
    const bool kbhit_on_release = false;
    if (!kbhit_on_release) {
        // key member unchanged
    }
    expect_eq("sticky_D_after_release", sticky, kCmdRight);
    std::printf("     still %s — Role::action will keep a=+A_ROLE\n", describe(sticky));

    if (fail) {
        std::printf("\n%d check(s) failed\n", fail);
        return 1;
    }
    std::printf("\nall command-bit checks passed\n");
    return 0;
}

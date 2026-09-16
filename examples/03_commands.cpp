// 03_commands — bit-packed input, same masks as define.h / Control.
//
// GetCommand ORs GetAsyncKeyState bits. The portable packer does the
// same so combinations used in Role::action can be unit-tested.

#include "maoliao_core.hpp"

#include <cstdio>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        ++g_failed;
    }
}

void expect_eq(int got, int want, const char* name)
{
    const bool ok = got == want;
    std::printf("  [%s] %s  got=%d want=%d\n", ok ? "PASS" : "FAIL", name, got,
                want);
    if (!ok) {
        ++g_failed;
    }
}

}  // namespace

int main()
{
    std::printf("03_commands\n");

    expect_eq(maoliao::kCmdLeft, 1, "CMD_LEFT");
    expect_eq(maoliao::kCmdRight, 2, "CMD_RIGHT");
    expect_eq(maoliao::kCmdUp, 4, "CMD_UP");
    expect_eq(maoliao::kCmdDown, 8, "CMD_DOWN");
    expect_eq(maoliao::kCmdShoot, 16, "CMD_SHOOT");
    expect_eq(maoliao::kCmdEsc, 32, "CMD_ESC");
    expect_eq(maoliao::kVirReturn, 64, "VIR_RETURN");
    expect_eq(maoliao::kVirRestart, 128, "VIR_RESTART");
    expect_eq(maoliao::kVirHome, 256, "VIR_HOME");

    const int walk_jump = maoliao::pack_command(true, false, true, false, false,
                                                false);
    expect_eq(walk_jump, maoliao::kCmdLeft | maoliao::kCmdUp, "A+W == 5");
    expect(maoliao::has_command(walk_jump, maoliao::kCmdLeft), "has left");
    expect(maoliao::has_command(walk_jump, maoliao::kCmdUp), "has up");
    expect(!maoliao::has_command(walk_jump, maoliao::kCmdRight), "no right");
    expect(!maoliao::has_command(walk_jump, maoliao::kCmdShoot), "no shoot");

    const int fire = maoliao::pack_command(false, true, false, false, true,
                                           false);
    expect_eq(fire, maoliao::kCmdRight | maoliao::kCmdShoot, "D+J == 18");

    const int pause = maoliao::pack_command(false, false, false, false, false,
                                            true);
    expect_eq(pause, maoliao::kCmdEsc, "Esc == 32");

    // Role::action uses (KEY & CMD_UP) style tests; leftover virtual
    // bits must not look like movement.
    const int restart = maoliao::kVirRestart;
    expect(!maoliao::has_command(restart, maoliao::kCmdLeft),
           "VIR_RESTART is not left");
    expect(!maoliao::has_command(restart, maoliao::kCmdUp),
           "VIR_RESTART is not jump");

    // Combined pause+hold should still reveal Esc.
    const int held = maoliao::kCmdRight | maoliao::kCmdEsc;
    expect(maoliao::has_command(held, maoliao::kCmdEsc), "Esc visible in combo");
    expect(maoliao::has_command(held, maoliao::kCmdRight), "right still set");

    if (g_failed != 0) {
        std::printf("03_commands: %d failed\n", g_failed);
        return 1;
    }
    std::printf("03_commands: all passed\n");
    return 0;
}

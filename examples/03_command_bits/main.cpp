// Twin of define.h command macros and Control::GetCommand / pauseClick.

#include "check.h"
#include "command_portable.h"

#include <iostream>

using namespace maoliao;
using maoliao::test::expect;

int main() {
    std::cout << "03_command_bits — OR masks and pause replacement\n";

    expect(kCmdLeft == 1, "CMD_LEFT");
    expect(kCmdRight == 2, "CMD_RIGHT");
    expect(kCmdUp == 4, "CMD_UP");
    expect(kCmdDown == 8, "CMD_DOWN");
    expect(kCmdShoot == 16, "CMD_SHOOT");
    expect(kCmdEsc == 32, "CMD_ESC");
    expect(kVirReturn == 64, "VIR_RETURN");
    expect(kVirRestart == 128, "VIR_RESTART");
    expect(kVirHome == 256, "VIR_HOME");

    KeyState idle;
    expect(packCommand(idle) == 0, "no keys");

    KeyState runJumpShoot;
    runJumpShoot.right = true;
    runJumpShoot.up = true;
    runJumpShoot.shoot = true;
    const int combo = packCommand(runJumpShoot);
    expect(combo == (kCmdRight | kCmdUp | kCmdShoot), "D + W + J = 2|4|16");
    expect(combo == 22, "numeric 22");
    expect(hasBit(combo, kCmdRight) && hasBit(combo, kCmdUp), "bits survive OR");
    expect(!hasBit(combo, kCmdLeft), "A was up");

    KeyState both;
    both.left = true;
    both.right = true;
    expect(packCommand(both) == (kCmdLeft | kCmdRight), "A+D both latched");
    // Role::action then does direction.x -= 1 and += 1 → 0 wish.

    KeyState pause;
    pause.right = true;
    pause.esc = true;
    const int beforePause = packCommand(pause);
    expect(hasBit(beforePause, kCmdEsc), "Esc bit set");
    expect(applyPause(beforePause, PauseChoice::Return) == kVirReturn,
           "返回游戏 overwrites, does not OR");
    expect(applyPause(beforePause, PauseChoice::Restart) == kVirRestart, "重新开始");
    expect(applyPause(beforePause, PauseChoice::Home) == kVirHome, "退出游戏 → VIR_HOME");
    expect(applyPause(beforePause, PauseChoice::Save) == kVirReturn,
           "存档 writes the file then VIR_RETURN");
    expect(!hasBit(applyPause(beforePause, PauseChoice::Home), kCmdRight),
           "latched D is discarded after pause");

    std::cout << "  sample mask D+W+J = " << combo << "\n";
    std::cout << "  pause Home        = " << kVirHome << "\n";

    return test::summary("03_command_bits");
}

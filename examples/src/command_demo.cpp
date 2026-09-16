// Bitmask overlay used by Control::GetCommand / pauseClick.

#include "maoliao_model.h"

#include <cstdio>
#include <string>

using namespace maoliao;

int main() {
    MLA_CHECK((kCmdLeft | kCmdRight | kCmdUp) == 7);
    MLA_CHECK(kVirHome == 256);

    const int walkJump = kCmdRight | kCmdUp;
    CommandBits c = decodeCommand(walkJump);
    MLA_CHECK(c.right && c.up);
    MLA_CHECK(!c.left && !c.shoot && !c.virHome);

    const int pauseSave = kCmdEsc;  // first frame of Esc
    c = decodeCommand(pauseSave);
    MLA_CHECK(c.esc);

    c = decodeCommand(kVirRestart);
    MLA_CHECK(c.virRestart);
    MLA_CHECK(!c.esc);

    // Holding A+D+J at once, as GetAsyncKeyState allows.
    const int mash = kCmdLeft | kCmdRight | kCmdShoot;
    auto names = commandList(mash);
    MLA_CHECK(names.size() == 3);
    MLA_CHECK(std::string(names[0]) == "CMD_LEFT");
    MLA_CHECK(std::string(names[1]) == "CMD_RIGHT");
    MLA_CHECK(std::string(names[2]) == "CMD_SHOOT");

    std::printf("command_demo ok  mash=");
    for (std::size_t i = 0; i < names.size(); ++i) {
        std::printf("%s%s", names[i], i + 1 == names.size() ? "\n" : "|");
    }
    return 0;
}

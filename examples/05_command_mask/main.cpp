#include "check.hpp"
#include "command.hpp"

#include <cstdio>
#include <string>

static std::string join(const std::vector<std::string>& parts) {
    std::string s;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i) {
            s += '|';
        }
        s += parts[i];
    }
    return s;
}

int main() {
    std::printf("=== 05 command mask ===\n");
    std::printf("bit table (define.h):\n");
    std::printf("  CMD_LEFT=%d RIGHT=%d UP=%d DOWN=%d SHOOT=%d ESC=%d\n",
                mla::kCmdLeft, mla::kCmdRight, mla::kCmdUp, mla::kCmdDown,
                mla::kCmdShoot, mla::kCmdEsc);
    std::printf("  VIR_RETURN=%d RESTART=%d HOME=%d\n", mla::kVirReturn,
                mla::kVirRestart, mla::kVirHome);

    const int runJump = mla::pack(false, true, true, false, false, false);
    const int strafe = mla::pack(true, true, false, false, false, false);
    const int shoot = mla::pack(false, false, false, false, true, false);
    const int pause = mla::kCmdEsc;
    const int restart = mla::kVirRestart;

    std::printf("D+W (run jump) = %d  [%s]\n", runJump,
                join(mla::decode(runJump)).c_str());
    std::printf("A+D (both)     = %d  [%s]\n", strafe,
                join(mla::decode(strafe)).c_str());
    std::printf("J shoot        = %d  [%s]\n", shoot,
                join(mla::decode(shoot)).c_str());
    std::printf("Esc            = %d  [%s]\n", pause,
                join(mla::decode(pause)).c_str());
    std::printf("pause restart  = %d  [%s]\n", restart,
                join(mla::decode(restart)).c_str());

    mla::Checks c;
    c.expect(runJump == (mla::kCmdRight | mla::kCmdUp), "D+W packs to 2|4=6");
    c.expect(mla::has(runJump, mla::kCmdRight) && mla::has(runJump, mla::kCmdUp),
             "decode sees RIGHT and UP");
    c.expect(!mla::has(runJump, mla::kCmdLeft), "LEFT not set");
    c.expect(strafe == 3, "A+D is 1|2 — Role will add both accelerations");
    c.expect(shoot == 16, "J is bit 16");
    c.expect(mla::kVirHome == 256 && mla::kVirRestart == 128, "virtual bits");
    c.expect(join(mla::decode(0)) == "NONE", "empty mask");
    return c.finish("05_command_mask");
}

#include "../common/maoliao_constants.h"

#include <cstdio>
#include <string>

namespace {

int g_failures = 0;

void expect(const char* name, bool cond) {
    if (!cond) {
        std::fprintf(stderr, "FAIL %s\n", name);
        ++g_failures;
    } else {
        std::printf("ok   %s\n", name);
    }
}

std::string decode(int mask) {
    static const struct {
        int bit;
        const char* name;
    } kBits[] = {
        {maoliao::kCmdLeft, "LEFT"},     {maoliao::kCmdRight, "RIGHT"},
        {maoliao::kCmdUp, "UP"},         {maoliao::kCmdDown, "DOWN"},
        {maoliao::kCmdShoot, "SHOOT"},   {maoliao::kCmdEsc, "ESC"},
        {maoliao::kVirReturn, "RETURN"}, {maoliao::kVirRestart, "RESTART"},
        {maoliao::kVirHome, "HOME"},
    };
    std::string out;
    for (const auto& b : kBits) {
        if (mask & b.bit) {
            if (!out.empty()) {
                out += "|";
            }
            out += b.name;
        }
    }
    return out.empty() ? "(none)" : out;
}

}  // namespace

int main() {
    using namespace maoliao;

    std::printf("=== Control bit masks (define.h) ===\n");
    std::printf("GetCommand ORs GetAsyncKeyState bits; pause writes VIR_* \n\n");

    const int walkJump = kCmdRight | kCmdUp;
    std::printf("A+D conflict: LEFT|RIGHT = %d (%s)\n", kCmdLeft | kCmdRight,
                decode(kCmdLeft | kCmdRight).c_str());
    std::printf("D+W typical run: %d (%s)\n", walkJump, decode(walkJump).c_str());
    std::printf("Esc: %d (%s)\n", kCmdEsc, decode(kCmdEsc).c_str());
    std::printf("pause restart: %d (%s)\n", kVirRestart, decode(kVirRestart).c_str());

    expect("unique powers of two",
           kCmdLeft == 1 && kCmdRight == 2 && kCmdUp == 4 && kCmdDown == 8 &&
               kCmdShoot == 16 && kCmdEsc == 32);
    expect("virtual keys continue the chain",
           kVirReturn == 64 && kVirRestart == 128 && kVirHome == 256);
    expect("decode jump-right", decode(walkJump) == "RIGHT|UP");
    expect("decode home", decode(kVirHome) == "HOME");

    // Role::action treats (CMD_UP && world==3) as extra jump even when airborne.
    const int w3Flap = kCmdUp;
    expect("world3 flap is just UP", (w3Flap & kCmdUp) != 0);

    if (g_failures) {
        std::fprintf(stderr, "\n%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("\nall input-bit checks passed\n");
    return 0;
}

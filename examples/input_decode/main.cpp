#include "input_bits.hpp"

#include <iostream>

namespace {

int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

}  // namespace

int main() {
    using namespace maoliao;

    const int ad = decodeWasd(true, true, false, false, false, false);
    if (ad != (kCmdLeft | kCmdRight)) {
        return fail("A+D mask");
    }
    if (describeMask(ad) != "LEFT|RIGHT") {
        std::cerr << "got " << describeMask(ad) << "\n";
        return fail("describe A+D");
    }

    const int jumpFire = decodeWasd(false, false, true, false, true, false);
    if (jumpFire != (kCmdUp | kCmdShoot)) {
        return fail("W+J mask");
    }
    if (!hasBit(jumpFire, kCmdUp) || !hasBit(jumpFire, kCmdShoot)) {
        return fail("hasBit");
    }

    const int idle = decodeWasd(false, false, false, false, false, false);
    if (idle != 0 || describeMask(idle) != "NONE") {
        return fail("idle");
    }

    if (describeMask(kVirHome | kCmdEsc) != "ESC|HOME") {
        std::cerr << "got " << describeMask(kVirHome | kCmdEsc) << "\n";
        return fail("ESC|HOME");
    }

    // Bits match define.h and Control::GetCommand.
    std::cout << "CMD_LEFT=" << kCmdLeft << " CMD_RIGHT=" << kCmdRight
              << " CMD_UP=" << kCmdUp << " CMD_SHOOT=" << kCmdShoot << "\n";
    std::cout << "sample A+J => " << describeMask(decodeWasd(true, false, false, false, true, false))
              << " (" << decodeWasd(true, false, false, false, true, false) << ")\n";
    std::cout << "OK\n";
    return 0;
}

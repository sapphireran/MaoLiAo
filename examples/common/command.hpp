#pragma once

#include "constants.hpp"

#include <string>
#include <vector>

namespace mla {

inline bool has(int mask, int bit) { return (mask & bit) != 0; }

inline int pack(bool left, bool right, bool up, bool down, bool shoot,
                bool esc) {
    int c = 0;
    if (left) c |= kCmdLeft;
    if (right) c |= kCmdRight;
    if (up) c |= kCmdUp;
    if (down) c |= kCmdDown;
    if (shoot) c |= kCmdShoot;
    if (esc) c |= kCmdEsc;
    return c;
}

inline std::vector<std::string> decode(int mask) {
    std::vector<std::string> names;
    if (has(mask, kCmdLeft)) names.emplace_back("LEFT");
    if (has(mask, kCmdRight)) names.emplace_back("RIGHT");
    if (has(mask, kCmdUp)) names.emplace_back("UP");
    if (has(mask, kCmdDown)) names.emplace_back("DOWN");
    if (has(mask, kCmdShoot)) names.emplace_back("SHOOT");
    if (has(mask, kCmdEsc)) names.emplace_back("ESC");
    if (has(mask, kVirReturn)) names.emplace_back("VIR_RETURN");
    if (has(mask, kVirRestart)) names.emplace_back("VIR_RESTART");
    if (has(mask, kVirHome)) names.emplace_back("VIR_HOME");
    if (names.empty()) names.emplace_back("NONE");
    return names;
}

}  // namespace mla

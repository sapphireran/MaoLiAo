#pragma once

#include "constants.hpp"

#include <cstdint>
#include <string>

namespace maoliao {

inline constexpr bool hasBit(int mask, int bit) { return (mask & bit) != 0; }

inline int decodeWasd(bool a, bool d, bool w, bool s, bool j, bool esc) {
    int c = 0;
    if (a) {
        c |= kCmdLeft;
    }
    if (d) {
        c |= kCmdRight;
    }
    if (w) {
        c |= kCmdUp;
    }
    if (s) {
        c |= kCmdDown;
    }
    if (j) {
        c |= kCmdShoot;
    }
    if (esc) {
        c |= kCmdEsc;
    }
    return c;
}

inline std::string describeMask(int mask) {
    std::string out;
    const struct {
        int bit;
        const char* name;
    } bits[] = {
        {kCmdLeft, "LEFT"},       {kCmdRight, "RIGHT"}, {kCmdUp, "UP"},
        {kCmdDown, "DOWN"},       {kCmdShoot, "SHOOT"}, {kCmdEsc, "ESC"},
        {kVirReturn, "RETURN"},   {kVirRestart, "RESTART"}, {kVirHome, "HOME"},
    };
    for (const auto& b : bits) {
        if (mask & b.bit) {
            if (!out.empty()) {
                out += '|';
            }
            out += b.name;
        }
    }
    if (out.empty()) {
        out = "NONE";
    }
    return out;
}

}  // namespace maoliao

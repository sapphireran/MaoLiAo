#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "maoliao/constants.hpp"

namespace maoliao {

struct KeyName {
    int bit;
    const char* name;
};

inline const KeyName* key_table(int& n) {
    static const KeyName k[] = {
        {kCmdLeft, "LEFT"},     {kCmdRight, "RIGHT"}, {kCmdUp, "UP"},
        {kCmdDown, "DOWN"},     {kCmdShoot, "SHOOT"}, {kCmdEsc, "ESC"},
        {kVirReturn, "RETURN"}, {kVirRestart, "RESTART"}, {kVirHome, "HOME"},
    };
    n = static_cast<int>(sizeof(k) / sizeof(k[0]));
    return k;
}

inline std::vector<std::string> decode_bits(int mask) {
    int n = 0;
    const KeyName* t = key_table(n);
    std::vector<std::string> out;
    for (int i = 0; i < n; ++i) {
        if (mask & t[i].bit) {
            out.push_back(t[i].name);
        }
    }
    return out;
}

inline int compose(std::initializer_list<int> bits) {
    int m = 0;
    for (int b : bits) {
        m |= b;
    }
    return m;
}

}  // namespace maoliao

#pragma once

#include "constants.hpp"

#include <initializer_list>
#include <string>
#include <vector>

namespace maoliao {

enum class Key : int {
    A,
    D,
    W,
    K,
    S,
    J,
    Esc,
};

inline int bit_for_key(Key key) {
    switch (key) {
    case Key::A:
        return kCmdLeft;
    case Key::D:
        return kCmdRight;
    case Key::W:
    case Key::K:
        return kCmdUp;
    case Key::S:
        return kCmdDown;
    case Key::J:
        return kCmdShoot;
    case Key::Esc:
        return kCmdEsc;
    }
    return 0;
}

// OR-combine held keys the same way Control::GetCommand does.
inline int command_mask(std::initializer_list<Key> held) {
    int mask = 0;
    for (Key key : held) {
        mask |= bit_for_key(key);
    }
    return mask;
}

inline bool has_cmd(int mask, int bit) { return (mask & bit) != 0; }

inline std::vector<std::string> decode_mask(int mask) {
    std::vector<std::string> labels;
    if (has_cmd(mask, kCmdLeft)) {
        labels.emplace_back("left");
    }
    if (has_cmd(mask, kCmdRight)) {
        labels.emplace_back("right");
    }
    if (has_cmd(mask, kCmdUp)) {
        labels.emplace_back("up");
    }
    if (has_cmd(mask, kCmdDown)) {
        labels.emplace_back("down");
    }
    if (has_cmd(mask, kCmdShoot)) {
        labels.emplace_back("shoot");
    }
    if (has_cmd(mask, kCmdEsc)) {
        labels.emplace_back("esc");
    }
    if (has_cmd(mask, kVirReturn)) {
        labels.emplace_back("vir-return");
    }
    if (has_cmd(mask, kVirRestart)) {
        labels.emplace_back("vir-restart");
    }
    if (has_cmd(mask, kVirHome)) {
        labels.emplace_back("vir-home");
    }
    return labels;
}

inline bool is_virtual_only(int mask) {
    const int gameplay = kCmdLeft | kCmdRight | kCmdUp | kCmdDown | kCmdShoot |
                         kCmdEsc;
    return (mask & gameplay) == 0 &&
           (mask & (kVirReturn | kVirRestart | kVirHome)) != 0;
}

} // namespace maoliao

#pragma once

#include "maoliao_const.hpp"

namespace maoliao {

enum class Key : int {
    A = 1,
    D = 2,
    W = 4,
    S = 8,
    J = 16,
    Esc = 32,
    K = 64,  // extra physical key that also sets CMD_UP
};

// Mirrors Control::GetCommand. K is folded into CMD_UP, not its own bit.
inline int commandFromKeys(int physical) {
    int c = 0;
    if (physical & static_cast<int>(Key::A)) {
        c |= kCmdLeft;
    }
    if (physical & static_cast<int>(Key::D)) {
        c |= kCmdRight;
    }
    if ((physical & static_cast<int>(Key::W)) || (physical & static_cast<int>(Key::K))) {
        c |= kCmdUp;
    }
    if (physical & static_cast<int>(Key::S)) {
        c |= kCmdDown;
    }
    if (physical & static_cast<int>(Key::J)) {
        c |= kCmdShoot;
    }
    if (physical & static_cast<int>(Key::Esc)) {
        c |= kCmdEsc;
    }
    return c;
}

struct HitBox {
    int left;
    int top;
    int right;
    int bottom;
};

inline bool inside(const HitBox& b, int x, int y) {
    return x > b.left && x < b.right && y > b.top && y < b.bottom;
}

// Home / pause columns are centered: [XSIZE/2 - 45, XSIZE/2 + 45] = [211, 301].
inline HitBox menuRow(int indexFromTop, int rowsStartY = kYSize / 3, int rowH = 30) {
    return HitBox{kXSize / 2 - 45, rowsStartY + indexFromTop * rowH, kXSize / 2 + 45,
                  rowsStartY + (indexFromTop + 1) * rowH};
}

inline HitBox backButton() {
    return HitBox{kXSize - 46, kYSize - 26, kXSize - 2, kYSize - 2};
}

}  // namespace maoliao

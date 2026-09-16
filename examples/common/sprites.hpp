#pragma once

#include "maoliao_const.hpp"

namespace maoliao {

// Role::show walk-cycle: rolePos = -x0 + x, then STEP-sized parity.
inline int heroIFrame(int rolePos) {
    const int q = rolePos / kStep;
    if (q % 2 == 0 && q % 4 != 0) {
        return 2;
    }
    if (q % 4 == 0) {
        return 1;
    }
    return 1;  // odd STEP cells keep the previous convention (frame 1 in practice)
}

// Sheet columns: facing right uses frames 0–1, facing left uses 3–4 (3*WIDTH offset).
inline int heroSheetColumn(int iframe, int turn) {
    if (turn >= 0) {
        return iframe - 1;
    }
    return (iframe - 1) + 3;
}

inline double wrapIFrame(double iframe, double step, int limit) {
    iframe += step;
    if (static_cast<int>(iframe) == limit) {
        return 1.0;
    }
    return iframe;
}

inline bool enemyMovesThisFrame(double enemyIFrame) {
    return (static_cast<int>(enemyIFrame * 100) % 2) == 0;
}

}  // namespace maoliao

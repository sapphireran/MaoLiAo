#pragma once

// Twin of Control::GetCommand bit packing and pauseClick replacement.

#include "define_portable.h"

namespace maoliao {

struct KeyState {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool shoot = false;
    bool esc = false;
};

inline int packCommand(const KeyState& k) {
    int c = 0;
    if (k.left) c |= kCmdLeft;
    if (k.right) c |= kCmdRight;
    if (k.up) c |= kCmdUp;
    if (k.down) c |= kCmdDown;
    if (k.shoot) c |= kCmdShoot;
    if (k.esc) c |= kCmdEsc;
    return c;
}

// Pause menu does not OR the virtual key; it overwrites the latched mask.
enum class PauseChoice { Return, Restart, Home, Save };

inline int applyPause(int /*currentMask*/, PauseChoice choice) {
    switch (choice) {
        case PauseChoice::Return:
        case PauseChoice::Save:
            return kVirReturn;
        case PauseChoice::Restart:
            return kVirRestart;
        case PauseChoice::Home:
            return kVirHome;
    }
    return kVirReturn;
}

inline bool hasBit(int mask, int bit) { return (mask & bit) != 0; }

}  // namespace maoliao

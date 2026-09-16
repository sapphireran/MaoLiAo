#pragma once

// Command bit layout from define.h / Control::GetCommand.

namespace mla {

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

inline int commandFromChars(const char* keysHeld) {
    int c = 0;
    if (!keysHeld) {
        return 0;
    }
    for (const char* p = keysHeld; *p; ++p) {
        switch (*p) {
        case 'A':
        case 'a':
            c |= kCmdLeft;
            break;
        case 'D':
        case 'd':
            c |= kCmdRight;
            break;
        case 'W':
        case 'w':
        case 'K':
        case 'k':
            c |= kCmdUp;
            break;
        case 'S':
        case 's':
            c |= kCmdDown;
            break;
        case 'J':
        case 'j':
            c |= kCmdShoot;
            break;
        case 'Q': // stand-in for Esc in portable tests
        case 'q':
            c |= kCmdEsc;
            break;
        default:
            break;
        }
    }
    return c;
}

inline bool wantsLeft(int key) { return (key & kCmdLeft) != 0; }
inline bool wantsRight(int key) { return (key & kCmdRight) != 0; }
inline bool wantsJump(int key) { return (key & kCmdUp) != 0; }
inline bool wantsShoot(int key) { return (key & kCmdShoot) != 0; }

} // namespace mla

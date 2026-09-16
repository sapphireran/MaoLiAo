#include "maoliao/commands.h"

#include "maoliao/constants.h"

#include <sstream>
#include <vector>

namespace maoliao {

int pack_commands(bool left, bool right, bool up, bool down, bool shoot, bool esc) {
    int c = 0;
    if (left) {
        c |= kCmdLeft;
    }
    if (right) {
        c |= kCmdRight;
    }
    if (up) {
        c |= kCmdUp;
    }
    if (down) {
        c |= kCmdDown;
    }
    if (shoot) {
        c |= kCmdShoot;
    }
    if (esc) {
        c |= kCmdEsc;
    }
    return c;
}

void unpack_commands(int mask, bool& left, bool& right, bool& up, bool& down,
                     bool& shoot, bool& esc) {
    left = (mask & kCmdLeft) != 0;
    right = (mask & kCmdRight) != 0;
    up = (mask & kCmdUp) != 0;
    down = (mask & kCmdDown) != 0;
    shoot = (mask & kCmdShoot) != 0;
    esc = (mask & kCmdEsc) != 0;
}

std::string describe_commands(int mask) {
    struct Named {
        int bit;
        const char* name;
    };
    const Named names[] = {
        {kCmdLeft, "LEFT"},       {kCmdRight, "RIGHT"}, {kCmdUp, "UP"},
        {kCmdDown, "DOWN"},       {kCmdShoot, "SHOOT"}, {kCmdEsc, "ESC"},
        {kVirReturn, "RETURN"},   {kVirRestart, "RESTART"},
        {kVirHome, "HOME"},
    };
    std::vector<const char*> on;
    for (const Named& n : names) {
        if (mask & n.bit) {
            on.push_back(n.name);
        }
    }
    if (on.empty()) {
        return "NONE";
    }
    std::ostringstream os;
    for (std::size_t i = 0; i < on.size(); ++i) {
        if (i) {
            os << '|';
        }
        os << on[i];
    }
    return os.str();
}

int sticky_update(int previous, int sampled, bool console_hit) {
    return console_hit ? sampled : previous;
}

}  // namespace maoliao

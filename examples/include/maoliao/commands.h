#pragma once

#include <string>

namespace maoliao {

struct CommandBits {
    int mask = 0;
};

int pack_commands(bool left, bool right, bool up, bool down, bool shoot, bool esc);

void unpack_commands(int mask, bool& left, bool& right, bool& up, bool& down,
                     bool& shoot, bool& esc);

std::string describe_commands(int mask);

// Models the sticky-key behavior of Control::getKey: the mask only refreshes
// when console_hit is true.
int sticky_update(int previous, int sampled, bool console_hit);

}  // namespace maoliao

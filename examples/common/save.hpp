#pragma once

#include <string>

namespace mla {

// gameRecord.dat is a single integer written with fprintf("%d", world).
// Valid worlds in the title-screen loader are 1, 2, and 3.

struct SaveResult {
    bool ok = false;
    int world = 0;
    std::string error;
};

bool writeWorld(const std::string& path, int world, std::string& error);
SaveResult readWorld(const std::string& path);
bool worldInRange(int world);

}  // namespace mla

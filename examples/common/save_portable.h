#pragma once

#include <string>

namespace maoliao {

// Twin of Control pause-save / title 读档.
// Valid worlds are 1, 2, 3 — same test as `flag >= 4 || flag <= 0`.

enum class SaveStatus { Ok, Missing, OutOfRange, IoError };

struct SaveResult {
    SaveStatus status = SaveStatus::Missing;
    int world = 0;
};

SaveStatus writeWorld(const std::string& path, int world);
SaveResult readWorld(const std::string& path);

inline bool worldInRange(int world) { return world >= 1 && world <= 3; }

}  // namespace maoliao

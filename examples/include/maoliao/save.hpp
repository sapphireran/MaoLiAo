#pragma once

#include <fstream>
#include <optional>
#include <string>

namespace maoliao {

inline bool world_in_range(int world) { return world >= 1 && world <= 3; }

// Writes the same single-integer text file Control::pauseClick produces.
inline bool write_world(const std::string& path, int world) {
    if (!world_in_range(world)) {
        return false;
    }
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return false;
    }
    out << world;
    return static_cast<bool>(out);
}

// Reads gameRecord.dat. Missing or unreadable files return nullopt instead
// of following the unchecked fopen_s path in control.cpp.
inline std::optional<int> read_world(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return std::nullopt;
    }
    int world = 0;
    in >> world;
    if (!in || !world_in_range(world)) {
        return std::nullopt;
    }
    return world;
}

} // namespace maoliao

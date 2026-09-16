#pragma once

#include <fstream>
#include <string>

namespace maoliao {

// Same grammar as Control::pauseClick / gameStart: one decimal integer, no newline required.
inline bool writeWorldSave(const std::string& path, int world) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return false;
    }
    out << world;
    return static_cast<bool>(out);
}

inline bool readWorldSave(const std::string& path, int& world) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    int flag = 0;
    if (!(in >> flag)) {
        return false;
    }
    if (flag >= 4 || flag <= 0) {
        return false;
    }
    world = flag;
    return true;
}

}  // namespace maoliao

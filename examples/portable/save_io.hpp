#pragma once

// Portable stand-in for the gameRecord.dat integer.
// Uses standard fopen (the game uses fopen_s on MSVC).

#include <cstdio>
#include <string>

namespace mla {

inline bool writeWorld(const std::string& path, int world) {
    FILE* fp = std::fopen(path.c_str(), "w");
    if (!fp) {
        return false;
    }
    const int n = std::fprintf(fp, "%d", world);
    std::fclose(fp);
    return n > 0;
}

inline bool readWorld(const std::string& path, int& world) {
    FILE* fp = std::fopen(path.c_str(), "r");
    if (!fp) {
        return false;
    }
    int flag = 0;
    const int n = std::fscanf(fp, "%d", &flag);
    std::fclose(fp);
    if (n != 1) {
        return false;
    }
    if (flag < 1 || flag > 3) {
        return false;
    }
    world = flag;
    return true;
}

} // namespace mla

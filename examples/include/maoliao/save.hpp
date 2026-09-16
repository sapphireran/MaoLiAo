#pragma once

#include <cstdio>
#include <string>

namespace maoliao {

inline bool write_record(const std::string& path, int world) {
    FILE* fp = std::fopen(path.c_str(), "w");
    if (!fp) {
        return false;
    }
    std::fprintf(fp, "%d", world);
    std::fclose(fp);
    return true;
}

inline bool read_record(const std::string& path, int& world) {
    FILE* fp = std::fopen(path.c_str(), "r");
    if (!fp) {
        return false;
    }
    const int n = std::fscanf(fp, "%d", &world);
    std::fclose(fp);
    return n == 1;
}

// Control::gameStart rejects <= 0 or >= 4.
inline bool record_ok(int world) {
    return world >= 1 && world <= 3;
}

}  // namespace maoliao

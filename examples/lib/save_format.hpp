#pragma once

#include <cctype>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

namespace maoliao {

// gameRecord.dat is a single integer world index written with fprintf("%d").
struct SaveData {
    int world = 1;
};

inline bool validWorld(int world) { return world >= 1 && world <= 3; }

inline std::string encodeSave(const SaveData& s) { return std::to_string(s.world); }

inline std::optional<SaveData> decodeSave(const std::string& text) {
    std::size_t i = 0;
    while (i < text.size() && std::isspace(static_cast<unsigned char>(text[i]))) {
        ++i;
    }
    if (i >= text.size()) {
        return std::nullopt;
    }
    int sign = 1;
    if (text[i] == '+' || text[i] == '-') {
        sign = (text[i] == '-') ? -1 : 1;
        ++i;
    }
    if (i >= text.size() || !std::isdigit(static_cast<unsigned char>(text[i]))) {
        return std::nullopt;
    }
    int value = 0;
    while (i < text.size() && std::isdigit(static_cast<unsigned char>(text[i]))) {
        value = value * 10 + (text[i] - '0');
        ++i;
    }
    SaveData s;
    s.world = sign * value;
    return s;
}

inline bool writeSaveFile(const std::string& path, const SaveData& s) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }
    out << encodeSave(s);
    return static_cast<bool>(out);
}

inline std::optional<SaveData> readSaveFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return std::nullopt;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return decodeSave(ss.str());
}

}  // namespace maoliao

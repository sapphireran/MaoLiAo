#include "save_format.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace maoliao {

SaveResult parseSaveText(const std::string& text) {
    SaveResult r{SaveStatus::Unreadable, 0};
    std::size_t i = 0;
    while (i < text.size() && std::isspace(static_cast<unsigned char>(text[i]))) {
        ++i;
    }
    if (i >= text.size()) {
        return r;
    }
    bool neg = false;
    if (text[i] == '+' || text[i] == '-') {
        neg = text[i] == '-';
        ++i;
    }
    if (i >= text.size() || !std::isdigit(static_cast<unsigned char>(text[i]))) {
        return r;
    }
    long value = 0;
    while (i < text.size() && std::isdigit(static_cast<unsigned char>(text[i]))) {
        value = value * 10 + (text[i] - '0');
        ++i;
    }
    if (neg) {
        value = -value;
    }
    if (value < 1 || value > 3) {
        r.status = SaveStatus::OutOfRange;
        r.world = static_cast<int>(value);
        return r;
    }
    r.status = SaveStatus::Ok;
    r.world = static_cast<int>(value);
    return r;
}

SaveResult parseSaveFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return SaveResult{SaveStatus::MissingFile, 0};
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return parseSaveText(ss.str());
}

std::string formatSave(int world) {
    return std::to_string(world);
}

}  // namespace maoliao

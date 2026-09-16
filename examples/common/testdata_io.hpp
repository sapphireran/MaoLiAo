#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace mla {

inline bool writeLines(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream out(path.c_str(), std::ios::trunc);
    if (!out) {
        return false;
    }
    for (std::size_t i = 0; i < lines.size(); ++i) {
        out << lines[i] << '\n';
    }
    return static_cast<bool>(out);
}

inline bool readLines(const std::string& path, std::vector<std::string>& lines) {
    std::ifstream in(path.c_str());
    if (!in) {
        return false;
    }
    lines.clear();
    std::string row;
    while (std::getline(in, row)) {
        if (!row.empty() && row[row.size() - 1] == '\r') {
            row.erase(row.size() - 1);
        }
        lines.push_back(row);
    }
    return true;
}

inline bool sameLines(const std::vector<std::string>& a,
                      const std::vector<std::string>& b) {
    return a == b;
}

inline std::string firstExisting(const std::vector<std::string>& candidates) {
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        std::ifstream in(candidates[i].c_str());
        if (in) {
            return candidates[i];
        }
    }
    return "";
}

}  // namespace mla

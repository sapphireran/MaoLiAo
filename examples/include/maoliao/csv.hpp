#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "maoliao/aabb.hpp"
#include "maoliao/worlds.hpp"

namespace maoliao {

inline std::vector<std::vector<int>> read_int_csv(const std::string& path) {
    std::ifstream in(path);
    std::vector<std::vector<int>> rows;
    std::string line;
    bool header = true;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        if (header) {
            header = false;
            continue;
        }
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        if (!row.empty()) {
            rows.push_back(row);
        }
    }
    return rows;
}

inline std::vector<Tile> tiles_from_csv(const std::string& path) {
    std::vector<Tile> out;
    for (const auto& r : read_int_csv(path)) {
        if (r.size() < 5) {
            continue;
        }
        out.push_back(Tile{r[0], r[1], r[2], r[3], r[4], 0});
    }
    return out;
}

inline std::vector<Actor> actors_from_csv(const std::string& path) {
    std::vector<Actor> out;
    for (const auto& r : read_int_csv(path)) {
        Actor a;
        if (r.size() >= 2) {
            a.x = r[0];
            a.y = r[1];
        }
        if (r.size() >= 3) {
            a.turn = r[2];
        }
        out.push_back(a);
    }
    return out;
}

}  // namespace maoliao

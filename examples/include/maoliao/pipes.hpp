#pragma once

#include "random.hpp"
#include "tiles.hpp"

#include <vector>

namespace maoliao {

struct PipeColumn {
    int x = 0;
    int height = 0;
    Tile top_mouth{};
    Tile top_shaft{};
    Tile bottom_mouth{};
    Tile bottom_shaft{};

    int gap_top() const { return height; }
    int gap_bottom() const { return 4 + height; }
    int gap_tiles() const { return gap_bottom() - gap_top(); }
};

inline PipeColumn make_pipe_column(int x, int height) {
    PipeColumn col;
    col.x = x;
    col.height = height;
    const int top_h = height > 2 ? height - 2 : 0;
    const int bottom_h = height < 6 ? 6 - height : 0;
    col.top_mouth = make_tile(x, height - 2, 8, 1, 1, 3);
    col.top_shaft = make_tile(x, 0, 7, 1, top_h, 3);
    col.bottom_mouth = make_tile(x, 4 + height, 10, 1, 1, 3);
    col.bottom_shaft = make_tile(x, height + 6, 7, 1, bottom_h, 3);
    return col;
}

// Scene::createMap world 3: x[i] = i*10+10, height = random(1,7), first 7 columns.
inline std::vector<PipeColumn> generate_gauntlet(Engine& engine, int columns = 7) {
    std::vector<PipeColumn> out;
    out.reserve(static_cast<std::size_t>(columns));
    for (int i = 0; i < columns; ++i) {
        const int x = i * 10 + 10;
        const int height = engine.range(1, 7);
        out.push_back(make_pipe_column(x, height));
    }
    return out;
}

inline std::vector<Tile> flatten_gauntlet(const std::vector<PipeColumn>& columns) {
    std::vector<Tile> tiles;
    tiles.reserve(columns.size() * 4 + 2);
    for (const auto& col : columns) {
        tiles.push_back(col.top_mouth);
        tiles.push_back(col.top_shaft);
        tiles.push_back(col.bottom_mouth);
        tiles.push_back(col.bottom_shaft);
    }
    tiles.push_back(make_tile(80, 6, 2, 25, 1, 3));
    tiles.push_back(make_tile(101, 4, 12, 1, 1, 3));
    return tiles;
}

inline bool column_sane(const PipeColumn& col) {
    if (col.height < 1 || col.height > 6) {
        return false;
    }
    if (col.gap_tiles() != 4) {
        return false;
    }
    if (col.bottom_mouth.y <= col.top_mouth.y) {
        return false;
    }
    return col.x >= 10 && (col.x - 10) % 10 == 0;
}

} // namespace maoliao

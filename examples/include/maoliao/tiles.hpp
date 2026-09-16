#pragma once

#include "constants.hpp"
#include "friction.hpp"

#include <string_view>

namespace maoliao {

enum class TileId : int {
    None = 0,
    Grass = 1,
    Cloud = 2,
    GrassFill = 3,
    SnowFill = 4,
    SnowCap = 5,
    PipeBody = 6,
    PipeShaft = 7,
    PipeMouthDown = 8,
    PipeMouthUpUnused = 9,
    PipeMouthUp = 10,
    Tuft = 11,
    Flag = 12,
    Water = 13,
    Tree = 14,
};

struct Tile {
    int x = 0;
    int y = 0;
    int id = 0;
    int x_amount = 0;
    int y_amount = 0;
    double u = 0.0;
};

inline bool is_solid(int id) {
    return id > 0 && id < 11;
}

inline bool is_pipe_mouth(int id) {
    return id == static_cast<int>(TileId::PipeMouthDown) ||
           id == static_cast<int>(TileId::PipeMouthUp);
}

inline int cell_width_px(int id) {
    return is_pipe_mouth(id) ? 2 * kTileWidth : kTileWidth;
}

inline int cell_height_px(int id) {
    return is_pipe_mouth(id) ? 2 * kTileHeight : kTileHeight;
}

inline std::string_view tile_name(int id) {
    switch (id) {
    case 1: return "grass";
    case 2: return "cloud";
    case 3: return "grass-fill";
    case 4: return "snow-fill";
    case 5: return "snow-cap";
    case 6: return "pipe-body";
    case 7: return "pipe-shaft";
    case 8: return "pipe-mouth-down";
    case 9: return "pipe-mouth-up-unused";
    case 10: return "pipe-mouth-up";
    case 11: return "tuft";
    case 12: return "flag";
    case 13: return "water";
    case 14: return "tree";
    default: return "unknown";
    }
}

inline Tile make_tile(int x, int y, int id, int x_amount, int y_amount, int world) {
    Tile t;
    t.x = x;
    t.y = y;
    t.id = id;
    t.x_amount = x_amount;
    t.y_amount = y_amount;
    t.u = friction_for_tile(id, world);
    return t;
}

} // namespace maoliao

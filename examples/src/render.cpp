#include "maoliao/render.h"

#include "maoliao/constants.h"

#include <algorithm>

namespace maoliao {
namespace {

bool solid_id(int id) { return id > 0 && id < 11; }

void stamp(std::vector<std::string>& grid, int min_x, int min_y, int x, int y,
           char c) {
    const int col = x - min_x;
    const int row = y - min_y;
    if (row < 0 || col < 0) {
        return;
    }
    if (row >= static_cast<int>(grid.size())) {
        return;
    }
    if (col >= static_cast<int>(grid[static_cast<std::size_t>(row)].size())) {
        return;
    }
    char& cell = grid[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
    if (c == '#' || cell == ' ' || (c == 'e' && cell != '#') ||
        (c == 'o' && cell == ' ') || (c == '*' && cell == ' ')) {
        cell = c;
    }
}

}  // namespace

std::vector<std::string> render_ascii(const Level& level, int min_x, int max_x,
                                      int min_y, int max_y) {
    const int w = std::max(0, max_x - min_x + 1);
    const int h = std::max(0, max_y - min_y + 1);
    std::vector<std::string> grid(static_cast<std::size_t>(h),
                                  std::string(static_cast<std::size_t>(w), ' '));

    for (const MapTile& t : level.tiles) {
        const char ch = solid_id(t.id) ? '#' : '.';
        for (int j = 0; j < t.x_amount; ++j) {
            for (int k = 0; k < t.y_amount; ++k) {
                stamp(grid, min_x, min_y, t.x + j, t.y + k, ch);
            }
        }
    }
    for (const TileCoord& c : level.coins) {
        stamp(grid, min_x, min_y, c.x, c.y, 'o');
    }
    for (const FoodSpawn& f : level.food) {
        stamp(grid, min_x, min_y, f.pixel_x / kWidth, f.pixel_y / kHeight, '*');
    }
    for (const EnemySpawn& e : level.enemies) {
        stamp(grid, min_x, min_y, e.tile_x, e.tile_y, 'e');
    }
    return grid;
}

std::string render_ascii_joined(const Level& level, int min_x, int max_x,
                                int min_y, int max_y) {
    const std::vector<std::string> rows = render_ascii(level, min_x, max_x, min_y, max_y);
    std::string out;
    for (const std::string& row : rows) {
        out += row;
        out += '\n';
    }
    return out;
}

}  // namespace maoliao

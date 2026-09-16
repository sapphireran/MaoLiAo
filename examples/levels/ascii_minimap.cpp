#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "check.hpp"
#include "maoliao_const.hpp"

using namespace maoliao;
using maoliao::test::check;

struct Tile {
    int x, y, id, xAmount, yAmount;
};

static const std::vector<Tile> kWorld1 = {
    {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1}, {67, 9, 6, 4, 1},
    {74, 9, 5, 1, 1},   {78, 9, 1, 28, 1}, {0, 10, 3, 15, 2}, {18, 9, 3, 9, 3},
    {27, 10, 4, 37, 2}, {67, 10, 3, 4, 2}, {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
    {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},  {65, 4, 2, 5, 1},
    {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1}, {90, 7, 10, 1, 1},
    {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1}, {49, 7, 11, 1, 1},
    {53, 7, 11, 1, 1},  {57, 7, 11, 1, 1}, {93, 7, 11, 1, 1}, {101, 7, 12, 1, 1},
};

static char glyph(int id) {
    switch (id) {
        case 1:
        case 5:
            return '=';
        case 3:
        case 4:
            return '#';
        case 2:
            return 'o';
        case 6:
        case 7:
            return '|';
        case 8:
        case 10:
            return 'P';
        case 11:
            return 'w';
        case 12:
            return 'F';
        case 13:
            return '~';
        case 14:
            return 'T';
        default:
            return '?';
    }
}

static std::vector<std::string> raster(const std::vector<Tile>& tiles, int maxX, int maxY) {
    std::vector<std::string> rows(static_cast<std::size_t>(maxY), std::string(static_cast<std::size_t>(maxX), ' '));
    const int loaded = std::min(static_cast<int>(tiles.size()), kMapNumber);
    for (int i = 0; i < loaded; ++i) {
        const Tile& t = tiles[static_cast<std::size_t>(i)];
        const int scale = (t.id == 8 || t.id == 10) ? 2 : 1;
        for (int dy = 0; dy < t.yAmount * scale; ++dy) {
            for (int dx = 0; dx < t.xAmount * scale; ++dx) {
                const int x = t.x + dx;
                const int y = t.y + dy;
                if (x >= 0 && x < maxX && y >= 0 && y < maxY) {
                    rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = glyph(t.id);
                }
            }
        }
    }
    return rows;
}

static void stamp(std::vector<std::string>& rows, int x, int y, char c) {
    if (y >= 0 && y < static_cast<int>(rows.size()) && x >= 0 &&
        x < static_cast<int>(rows[static_cast<std::size_t>(y)].size())) {
        rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = c;
    }
}

int main() {
    std::cout << "levels / ascii_minimap  (world 1, 32-px tiles → one char)\n";
    auto rows = raster(kWorld1, 110, 13);
    stamp(rows, 2, 3, '@');  // spawn intent from define.h comments (Y0 tile)
    stamp(rows, 10, 5, '*');
    stamp(rows, 74, 5, '*');
    stamp(rows, 3, 8, 'e');
    stamp(rows, 92, 6, 'e');

    int ground = 0;
    int clouds = 0;
    int pipes = 0;
    int flag = 0;
    for (const auto& row : rows) {
        for (char c : row) {
            if (c == '=' || c == '#') {
                ++ground;
            } else if (c == 'o') {
                ++clouds;
            } else if (c == 'P' || c == '|') {
                ++pipes;
            } else if (c == 'F') {
                ++flag;
            }
        }
    }
    check(ground > 100, "long grassland");
    check(clouds == 4 + 3 + 1 + 5, "cloud cell count");
    check(pipes >= 4, "at least the four pipe mouths");
    check(flag == 1, "goal sign in the loaded set for world 1");

    std::cout << "  legend: @ spawn  e enemy  * coin  = grass  # dirt  o cloud  P pipe  F flag  w grass tuft\n";
    for (int y = 0; y < static_cast<int>(rows.size()); ++y) {
        // Skip empty sky rows to keep the log readable, keep structure rows.
        const bool empty = rows[static_cast<std::size_t>(y)].find_first_not_of(' ') == std::string::npos;
        if (empty && y < 3) {
            continue;
        }
        std::cout << "  " << (y < 10 ? " " : "") << y << '|' << rows[static_cast<std::size_t>(y)] << '\n';
    }
    std::cout << "    +" << std::string(110, '-') << '\n';
    return test::summary("ascii_minimap");
}

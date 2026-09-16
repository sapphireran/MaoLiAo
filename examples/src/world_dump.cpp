// Print campaign tables and an ASCII slice of worlds 1 and 2.
// World 3 is generated at runtime; only the enemy list is static.

#include "maoliao_model.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace maoliao;

static void stamp(std::vector<std::string>& grid, const Tile& t, int originX) {
    const int w = (t.id == 8 || t.id == 10) ? t.xAmount * 2 : t.xAmount;
    const int h = (t.id == 8 || t.id == 10) ? t.yAmount * 2 : t.yAmount;
    const char ch = tileGlyph(t.id)[0];
    for (int j = 0; j < w; ++j) {
        for (int k = 0; k < h; ++k) {
            const int x = t.x + j - originX;
            const int y = t.y + k;
            if (y < 0 || y >= static_cast<int>(grid.size())) continue;
            if (x < 0 || x >= static_cast<int>(grid[0].size())) continue;
            grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = ch;
        }
    }
}

static void dumpWorld(int world, int originX, int cols) {
    std::size_t tn = 0, en = 0, cn = 0;
    const Tile* tiles = tilesFor(world, tn);
    const Actor* enemies = enemiesFor(world, en);
    const Point* coins = coinsFor(world, cn);

    const int rows = kYSize / kHeight;  // 12
    std::vector<std::string> grid(static_cast<std::size_t>(rows), std::string(static_cast<std::size_t>(cols), ' '));

    const int loaded = loadedTileCount(world);
    for (int i = 0; i < loaded; ++i) stamp(grid, tiles[i], originX);
    for (std::size_t i = 0; i < cn; ++i) {
        const int x = coins[i].x - originX;
        const int y = coins[i].y;
        if (y >= 0 && y < rows && x >= 0 && x < cols) {
            if (grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] == ' ')
                grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = 'o';
        }
    }
    for (std::size_t i = 0; i < en; ++i) {
        const int x = enemies[i].x - originX;
        const int y = enemies[i].y;
        if (y >= 0 && y < rows && x >= 0 && x < cols)
            grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = 'e';
    }

    std::printf("\n== world %d  table_tiles=%zu loaded=%d dropped=%d enemies=%zu coins=%zu ending=%dpx ==\n",
                world, tn, loaded, static_cast<int>(tn) - loaded, en, cn, endingDistance(world));
    std::printf("    ");
    for (int x = 0; x < cols; x += 10) std::printf("%-10d", originX + x);
    std::printf("\n");
    for (int y = 0; y < rows; ++y) {
        std::printf("%2d |%s|\n", y, grid[static_cast<std::size_t>(y)].c_str());
    }
}

int main() {
    std::size_t n1 = 0, n2 = 0;
    tilesFor(1, n1);
    tilesFor(2, n2);
    if (n1 != 32) {
        std::fprintf(stderr, "world1 tile count %zu want 32\n", n1);
        return 1;
    }
    if (n2 != 36) {
        std::fprintf(stderr, "world2 tile count %zu want 36\n", n2);
        return 1;
    }
    if (loadedTileCount(1) != 30 || loadedTileCount(2) != 30) {
        std::fprintf(stderr, "MAP_NUMBER clip mismatch\n");
        return 1;
    }

    dumpWorld(1, 0, 70);
    dumpWorld(1, 60, 50);
    dumpWorld(2, 0, 70);
    dumpWorld(2, 60, 55);

    std::size_t e3 = 0;
    enemiesFor(3, e3);
    std::printf("\n== world 3  pipes are random; static enemies=%zu ending=%dpx ==\n",
                e3, endingDistance(3));
    for (std::size_t i = 0; i < e3; ++i) {
        std::printf("  enemy (%d,%d) turn=%d\n",
                    kWorld3Enemies[i].x, kWorld3Enemies[i].y, kWorld3Enemies[i].turn);
    }

    std::printf("\nworld_dump ok  glyph=#grass ~cloud *snow Hgrip e=enemy o=coin F=flag\n");
    return 0;
}

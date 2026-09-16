#include "portable/level_data.hpp"
#include "portable/maoliao_math.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr int kCols = 110;
constexpr int kRows = 14;

int gFailures = 0;

void expect(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

char overlay(char base, char next) {
    // Prefer actors over tiles when printing a cell.
    if (next == 'o' || next == 'e' || next == '*') {
        return next;
    }
    return base == ' ' ? next : base;
}

} // namespace

int main() {
    using namespace mla;
    using namespace mla::world1;

    std::vector<std::string> grid(kRows, std::string(kCols, ' '));

    auto plot = [&](int x, int y, char ch) {
        if (y < 0 || y >= kRows || x < 0 || x >= kCols) {
            return;
        }
        grid[y][x] = overlay(grid[y][x], ch);
    };

    for (int i = 0; i < kMapCount; ++i) {
        const Tile& t = kMap[i];
        const char g = glyphForId(t.id);
        const int xSpan = (t.id == 8 || t.id == 10) ? t.xAmount * 2 : t.xAmount;
        const int ySpan = (t.id == 8 || t.id == 10) ? t.yAmount * 2 : t.yAmount;
        for (int dx = 0; dx < xSpan; ++dx) {
            for (int dy = 0; dy < ySpan; ++dy) {
                plot(t.x + dx, t.y + dy, g);
            }
        }
    }

    for (int i = 0; i < kCoinCount; ++i) {
        plot(kCoins[i].x, kCoins[i].y, 'o');
    }
    for (int i = 0; i < kEnemyCount; ++i) {
        plot(kEnemies[i].x, kEnemies[i].y, 'e');
    }
    plot(kFood.x / kWidth, kFood.y / kHeight, '*');

    std::cout << "World 1 sketch (" << kCols << " x " << kRows
              << " cells). Legend: = grass  # dirt  C cloud  P pipe  "
                 "o coin  e enemy  * food  F flag  ~ water  , tuft  I slick\n";
    std::cout << "     ";
    for (int x = 0; x < kCols; x += 10) {
        std::cout << x / 10;
        std::cout << std::string(9, ' ');
    }
    std::cout << "\n";
    for (int y = 0; y < kRows; ++y) {
        if (y < 10) {
            std::cout << ' ';
        }
        std::cout << y << " | " << grid[y] << "\n";
    }

    expect("start_lawn_at_0_9", grid[9][0] == '=' || grid[9][0] == 'e');
    expect("raised_island_at_18_8", grid[8][18] == '=' || grid[8][18] == 'e');
    expect("cloud_at_10_6", grid[6][10] == 'C' || grid[6][10] == 'o');
    expect("coin_on_first_cloud", grid[5][10] == 'o');
    expect("flag_at_101_7", grid[7][101] == 'F');
    expect("food_near_14_5", grid[5][14] == '*');
    expect("slick_strip_67_9", grid[9][67] == 'I');
    expect("map_rows_exceed_capacity", kMapCount > kMapCapacity);
    expect("first_30_still_has_flag", kMap[27].id == 12);

    std::cout << "map_rows=" << kMapCount << " MAP_NUMBER=" << kMapCapacity
              << " overflow=" << (kMapCount - kMapCapacity) << "\n";
    std::cout << "coins=" << kCoinCount << " enemies=" << kEnemyCount << "\n";
    std::cout << "ending_px=" << endingDistance(1) << " (94 tiles)\n";

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "level_dump: all checks passed\n";
    return 0;
}

// Rebuilds the first seven Flappy-style pipe pairs from Scene::createMap(3).
// Prints an ASCII slice so you can see the gap travel without EasyX.

#include "maoliao_core.h"

#include <iostream>
#include <string>
#include <vector>

using namespace maoliao;

static int gFailures = 0;

static void expect(const char* name, bool cond) {
    if (!cond) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "ok   " << name << "\n";
    }
}

static void printSlice(const std::vector<MapRun>& runs) {
    // 12 rows (the window) × columns 0..110, 1 char per tile.
    constexpr int rows = 12;
    constexpr int cols = 110;
    std::vector<std::string> grid(rows, std::string(cols, '.'));

    auto paint = [&](const MapRun& m, char ch) {
        const Rect r = tileWorldRect(m);
        const int x0 = r.left / kWidth;
        const int y0 = r.top / kHeight;
        const int x1 = r.right / kWidth;
        const int y1 = r.bottom / kHeight;
        for (int y = y0; y < y1 && y < rows; ++y) {
            if (y < 0) {
                continue;
            }
            for (int x = x0; x < x1 && x < cols; ++x) {
                if (x < 0) {
                    continue;
                }
                grid[y][x] = ch;
            }
        }
    };

    for (const auto& m : runs) {
        char ch = '#';
        if (m.id == 2) {
            ch = '=';
        } else if (m.id == 12) {
            ch = 'F';
        } else if (m.id == 8 || m.id == 10) {
            ch = 'O';
        } else if (m.id == 7) {
            ch = '|';
        }
        paint(m, ch);
    }

    std::cout << "world 3 ASCII (O mouth, | shaft, = cloud, F flag)\n";
    for (int y = 0; y < rows; ++y) {
        std::cout << grid[y] << "\n";
    }
}

int main() {
    std::cout << "=== MaoLiAo world-3 pipe generator ===\n";

    const int heights[7] = {1, 2, 3, 4, 5, 6, 3};
    const auto runs = buildWorld3(heights);

    expect("30 slots like MAP_NUMBER", runs.size() == 30);
    expect("first pair x=10", runs[0].x == 10 && runs[0].id == 8);
    expect("landing cloud", runs[28].id == 2 && runs[28].x == 80 && runs[28].xAmount == 25);
    expect("victory sign", runs[29].id == 12 && runs[29].x == 101);

    const PipePair mid = makePipePair(30, 4);
    expect("height 4 top mouth y=2", mid.topMouth.y == 2);
    expect("height 4 bottom mouth y=8", mid.bottomMouth.y == 8);
    // 4-tile gap between y=2 (2-tall mouth → occupies 2,3) and y=8 (occupies 8,9)
    // free rows 4,5,6,7 — four tiles, matches createMap comments.
    expect("top shaft yAmt", mid.topShaft.yAmount == 2);
    expect("bottom shaft yAmt", mid.bottomShaft.yAmount == 2);

    printSlice(runs);

    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "all pipe-generator checks passed\n";
    return 0;
}

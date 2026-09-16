// Replay Scene::createMap(3) column math for a chosen height sequence.
// The game never calls srand, so a default CRT seed is a common case.

#include "maoliao_model.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace maoliao;

struct Pipe {
    int x;
    int height;
    Tile topMouth;
    Tile topShaft;
    Tile botMouth;
    Tile botShaft;
};

static int rnd(int a, int b) { return std::rand() % (b - a) + a; }

static std::vector<Pipe> build(const int height[10]) {
    std::vector<Pipe> out;
    for (int i = 0; i < 7; ++i) {  // i=7..9 commented out in scene.cpp
        const int x = i * 10 + 10;
        const int h = height[i];
        Pipe p;
        p.x = x;
        p.height = h;
        p.topMouth = {x, h - 2, 8, 1, 1};
        p.topShaft = {x, 0, 7, 1, std::max(0, h - 2)};
        p.botMouth = {x, 4 + h, 10, 1, 1};
        p.botShaft = {x, h + 6, 7, 1, std::max(0, 6 - h)};
        out.push_back(p);
    }
    return out;
}

int main() {
    // Deterministic fixture used in docs: heights 1..6 cycling.
    int fixture[10] = {1, 2, 3, 4, 5, 6, 1, 2, 3, 4};
    auto pipes = build(fixture);
    MLA_CHECK(pipes.size() == 7);
    MLA_CHECK(pipes[0].x == 10);
    MLA_CHECK(pipes[6].x == 70);
    MLA_CHECK(pipes[0].topShaft.yAmount == 0);           // max(0, 1-2)
    MLA_CHECK(pipes[0].botShaft.yAmount == 5);           // max(0, 6-1)
    MLA_CHECK(pipes[5].topMouth.y == 4);                 // 6-2
    MLA_CHECK(pipes[5].botMouth.y == 10);                // 4+6

    // Gap between bottom of top mouth (2 tiles tall) and top of bottom mouth.
    // Top mouth occupies y = height-2 and height-1 (double size).
    // Bottom mouth starts at y = height+4. Gap tiles: height, height+1, height+2, height+3.
    for (const auto& p : pipes) {
        const int topBottom = p.topMouth.y + 2;  // exclusive
        const int gap = p.botMouth.y - topBottom;
        MLA_CHECK(gap == 4);
    }

    std::printf("pipe_gen fixture:\n");
    std::printf("  x   h  topMouth.y  topShaft.h  botMouth.y  botShaft.h  gap\n");
    for (const auto& p : pipes) {
        const int gap = p.botMouth.y - (p.topMouth.y + 2);
        std::printf("  %-3d %-2d %-11d %-11d %-11d %-11d %d\n",
                    p.x, p.height, p.topMouth.y, p.topShaft.yAmount,
                    p.botMouth.y, p.botShaft.yAmount, gap);
    }

    // Show one CRT-default sequence so the note in study-notes is concrete.
    std::srand(1);
    int seeded[10];
    for (int i = 0; i < 10; ++i) seeded[i] = rnd(1, 7);
    std::printf("pipe_gen srand(1) heights:");
    for (int i = 0; i < 10; ++i) std::printf(" %d", seeded[i]);
    std::printf("\n");

    std::printf("pipe_gen ok  active_columns=7 cloud_runway=x80..104 flag=x101\n");
    return 0;
}

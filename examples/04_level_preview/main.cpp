#include "check.hpp"
#include "constants.hpp"
#include "map_data.hpp"

#include <cstdio>
#include <string>
#include <vector>

static void dumpWorld(const char* title, const mla::MapRec* recs, std::size_t n,
                      int tilesW, int tilesH) {
    const std::size_t loaded = mla::loadedCount(n);
    std::printf("\n== %s  authored=%zu  loaded=%zu (cap %d)  dropped=%zu ==\n",
                title, n, loaded, mla::kMapNumber, n - loaded);
    for (std::size_t i = 0; i < n; ++i) {
        const char mark = (i < loaded) ? ' ' : '!';
        std::printf("  %c [%2zu] x=%3d y=%2d id=%2d %2dx%-2d\n", mark, i, recs[i].x,
                    recs[i].y, recs[i].id, recs[i].xAmount, recs[i].yAmount);
    }
    const std::vector<std::string> grid =
        mla::rasterize(recs, n, tilesW, tilesH, loaded);
    std::printf("ASCII (tile rows 0..%d, cols 0..%d); # solid  . deco\n",
                tilesH - 1, tilesW - 1);
    for (int y = 0; y < tilesH; ++y) {
        std::printf("%2d |%s|\n", y, grid[static_cast<std::size_t>(y)].c_str());
    }
}

static void listPoints(const char* title, const mla::PointRec* p, std::size_t n) {
    std::printf("%s (%zu):", title, n);
    for (std::size_t i = 0; i < n; ++i) {
        std::printf(" (%d,%d)", p[i].x, p[i].y);
    }
    std::printf("\n");
}

static void listEnemies(const char* title, const mla::EnemyRec* e, std::size_t n) {
    std::printf("%s (%zu):", title, n);
    for (std::size_t i = 0; i < n; ++i) {
        std::printf(" (%d,%d,%+d)", e[i].x, e[i].y, e[i].turn);
    }
    std::printf("\n");
}

int main() {
    std::printf("=== 04 level preview ===\n");

    std::size_t n1 = 0, n2 = 0, c1 = 0, c2 = 0, e1 = 0, e2 = 0, t3 = 0;
    const mla::MapRec* w1 = mla::world1Map(n1);
    const mla::MapRec* w2 = mla::world2Map(n2);
    dumpWorld("world 1 grass", w1, n1, 110, 14);
    dumpWorld("world 2 ice", w2, n2, 120, 14);

    listPoints("world1 coins", mla::world1Coins(c1), c1);
    listPoints("world2 coins", mla::world2Coins(c2), c2);
    listEnemies("world1 enemies", mla::world1Enemies(e1), e1);
    listEnemies("world2 enemies", mla::world2Enemies(e2), e2);

    const mla::MapRec* tail = mla::world3FixedTail(t3);
    std::printf("world3 fixed tail (after random pipes): %zu records\n", t3);
    for (std::size_t i = 0; i < t3; ++i) {
        std::printf("  x=%d y=%d id=%d %dx%d\n", tail[i].x, tail[i].y, tail[i].id,
                    tail[i].xAmount, tail[i].yAmount);
    }
    std::printf("finish lines: w1=%d px  w2=%d px  w3=%d px\n",
                mla::endingDistance(1), mla::endingDistance(2),
                mla::endingDistance(3));

    mla::Checks chk;
    chk.expect(n1 == 32 && n2 == 36, "authored sizes match scene.cpp");
    chk.expect(c1 == 20 && c2 == 11, "coin counts");
    chk.expect(e1 == 10 && e2 == 6, "enemy counts");
    chk.expect(mla::loadedCount(n1) == 30 && mla::loadedCount(n2) == 30,
               "both worlds clip to MAP_NUMBER");
    chk.expect(mla::endingDistance(2) == 104 * 32, "world 2 finish is 104 tiles");

    const std::vector<std::string> g1 = mla::rasterize(w1, n1, 110, 14, 30);
    chk.expect(g1[9][0] == '#' && g1[9][14] == '#', "world1 grass row 9 filled");
    chk.expect(g1[6][10] == '#', "cloud at (10,6)");
    chk.expect(g1[7][101] != '#', "goal id12 is deco, not solid");
    return chk.finish("04_level_preview");
}

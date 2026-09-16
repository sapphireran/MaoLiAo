// Dump the three authored worlds as CSV. Defaults match Scene::createMap
// / Role::createEnemy with the MAP_NUMBER cap applied (world 2 truncates).

#include "maoliao_core.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using namespace maoliao;

static void usage() {
    std::fprintf(stderr,
                 "usage: level_catalog [--world N] [--seed N] [--warn] [--legacy-leq]\n"
                 "                     [--format csv|summary]\n");
}

static void dump_maps(int world, const std::vector<MapRec>& maps, bool warn,
                      int truncated, int authored) {
    std::printf("# world %d maps stored=%zu authored=%d truncated=%d cap=%d\n", world,
                maps.size(), authored, truncated, kMapNumber);
    if (warn && truncated > 0) {
        std::printf("# WARN world %d dropped %d records past MAP_NUMBER\n", world,
                    truncated);
    }
    std::printf("kind,world,i,x,y,id,name,x_amount,y_amount,u,collides\n");
    for (std::size_t i = 0; i < maps.size(); ++i) {
        const auto& m = maps[i];
        std::printf("map,%d,%zu,%d,%d,%d,%s,%d,%d,%.6f,%d\n", world, i, m.x, m.y, m.id,
                    tile_name(m.id), m.x_amount, m.y_amount, m.u, (int)collides(m.id));
    }
}

static void dump_coins(int world, const std::vector<Point>& coins) {
    std::printf("kind,world,i,x,y\n");
    for (std::size_t i = 0; i < coins.size(); ++i) {
        std::printf("coin,%d,%zu,%d,%d\n", world, i, coins[i].x, coins[i].y);
    }
}

static void dump_enemies(int world, const std::vector<EnemyRec>& emy) {
    std::printf("kind,world,i,x,y,turn\n");
    for (std::size_t i = 0; i < emy.size(); ++i) {
        std::printf("enemy,%d,%zu,%d,%d,%d\n", world, i, emy[i].x, emy[i].y, emy[i].turn);
    }
}

static void summary(int world, const std::vector<MapRec>& maps,
                    const std::vector<Point>& coins, const std::vector<EnemyRec>& emy,
                    int truncated) {
    int solid = 0;
    int decor = 0;
    int tiles = 0;
    for (const auto& m : maps) {
        if (collides(m.id)) {
            ++solid;
        } else if (m.id > 0) {
            ++decor;
        }
        tiles += m.x_amount * m.y_amount;
    }
    const int score = static_cast<int>(coins.size()) * 10 + static_cast<int>(emy.size()) * 5;
    std::printf("world=%d maps=%zu solid=%d decor=%d tile_cells=%d coins=%zu enemies=%zu "
                "max_score=%d truncated=%d ending_px=%d\n",
                world, maps.size(), solid, decor, tiles, coins.size(), emy.size(), score,
                truncated, (world == 2 ? 104 : 94) * kWidth);
}

int main(int argc, char** argv) {
    int world = 0; // 0 = all
    unsigned seed = 1;
    bool warn = false;
    bool legacy = false;
    std::string format = "csv";

    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--world" && i + 1 < argc) {
            world = std::atoi(argv[++i]);
        } else if (a == "--seed" && i + 1 < argc) {
            seed = static_cast<unsigned>(std::atoi(argv[++i]));
        } else if (a == "--warn") {
            warn = true;
        } else if (a == "--legacy-leq") {
            legacy = true;
        } else if (a == "--format" && i + 1 < argc) {
            format = argv[++i];
        } else if (a == "--help" || a == "-h") {
            usage();
            return 0;
        } else {
            usage();
            return 2;
        }
    }

    auto run = [&](int w) {
        std::vector<MapRec> maps;
        std::vector<Point> coins;
        std::vector<EnemyRec> emy;
        int truncated = 0;
        int authored = 0;
        if (w == 1) {
            maps = world1_maps();
            authored = 32;
            truncated = authored - static_cast<int>(maps.size());
            coins = world1_coins();
            emy = world1_enemies();
        } else if (w == 2) {
            maps = world2_maps(&truncated);
            authored = 36;
            coins = world2_coins();
            emy = world2_enemies();
        } else if (w == 3) {
            maps = world3_maps(seed, 7);
            authored = 7 * 4 + 2;
            truncated = authored - static_cast<int>(maps.size());
            coins = world3_coins(seed);
            emy = world3_enemies();
        } else {
            std::fprintf(stderr, "world must be 1, 2, or 3\n");
            std::exit(2);
        }

        if (legacy) {
            std::printf("# --legacy-leq: 2020 loops used i <= count and wrote one extra "
                        "slot (often garbage). This dump still uses i < count.\n");
        }

        if (format == "summary") {
            summary(w, maps, coins, emy, truncated);
        } else {
            dump_maps(w, maps, warn, truncated, authored);
            dump_coins(w, coins);
            dump_enemies(w, emy);
        }

        if (warn) {
            const int score =
                static_cast<int>(coins.size()) * 10 + static_cast<int>(emy.size()) * 5;
            if (w == 1 && (maps.size() != 30 || truncated != 2 || score != 250)) {
                std::fprintf(stderr,
                             "world 1 contract: maps=30 truncated=2 score=250 (got "
                             "maps=%zu truncated=%d score=%d)\n",
                             maps.size(), truncated, score);
                std::exit(1);
            }
            if (w == 2 && (truncated != 6 || score != 140)) {
                std::fprintf(stderr,
                             "world 2 contract: truncated=6 score=140 (got truncated=%d "
                             "score=%d)\n",
                             truncated, score);
                std::exit(1);
            }
            if (w == 3 && (coins.size() != 7 || emy.size() != 7 || score != 105)) {
                std::fprintf(stderr, "world 3 contract: 7 coins, 7 enemies, score=105\n");
                std::exit(1);
            }
        }
    };

    if (world == 0) {
        run(1);
        run(2);
        run(3);
    } else {
        run(world);
    }
    return 0;
}

// AABB vertex test copied from Role::isHit / hitMap / hitCoins.

#include "maoliao_core.hpp"

#include <cstdio>
#include <cstdlib>

using namespace maoliao;

static int fail = 0;

static void expect(const char* name, bool got, bool want) {
    if (got != want) {
        std::printf("FAIL %s: got %s want %s\n", name, got ? "true" : "false",
                    want ? "true" : "false");
        ++fail;
    } else {
        std::printf("ok   %s\n", name);
    }
}

int main() {
    // Standing on world-1 start grass: tile {0,9,id 1,15,1} covers
    // x 0..480, y 288..320. Hero at (64, 256) has feet at y=287, so
    // hitMap(x, y+1) should connect (the landing probe).
    const auto maps = world1_maps();
    const MapRec* hit = nullptr;

    expect("spawn_overlap_y256",
           hit_map(kX0, kX0, 0, maps.data(), static_cast<int>(maps.size()), 1, &hit),
           false);

    const bool land = hit_map(64, 256 + 1, 0, maps.data(), static_cast<int>(maps.size()), 1,
                              &hit);
    expect("land_probe_y257", land, true);
    if (hit) {
        std::printf("     landed on id=%d (%s) at tile (%d,%d)\n", hit->id,
                    tile_name(hit->id), hit->x, hit->y);
        if (hit->id != 1 || hit->x != 0) {
            std::printf("FAIL expected grass at (0,9)\n");
            ++fail;
        }
    }

    // Mid-air above the same tile.
    expect("air_y200",
           hit_map(64, 200, 0, maps.data(), static_cast<int>(maps.size()), 1), false);

    // Pipe mouth id 10 uses a 2x2 box. World 1 pipe at (36,7).
    // Box: x 1152..1216, y 224..288.
    bool pipe = false;
    for (const auto& m : maps) {
        if (m.x == 36 && m.id == 10) {
            Point hero[4];
            hero_vertices(1152 + 8, 224 + 8, 0, hero);
            Point box[2];
            tile_box(m, box);
            pipe = is_hit(hero, box);
            std::printf("     pipe box (%d,%d)-(%d,%d)\n", box[0].x, box[0].y, box[1].x,
                        box[1].y);
            if (box[1].x - box[0].x != 64 || box[1].y - box[0].y != 64) {
                std::printf("FAIL pipe should be 64x64\n");
                ++fail;
            }
        }
    }
    expect("inside_pipe_mouth", pipe, true);

    // Decor id 11 must not collide.
    MapRec tuft{19, 6, 11, 1, 1};
    expect("decor_skipped",
           hit_map(19 * 32, 6 * 32, 0, &tuft, 1, 1), false);

    // Coin at (10,5): 320,160 .. 352,192. Hero overlapping that tile.
    Point coin_box[2] = {{10 * 32, 5 * 32}, {11 * 32, 6 * 32}};
    Point hero[4];
    hero_vertices(320, 160, 0, hero);
    expect("coin_overlap", is_hit(hero, coin_box), true);
    hero_vertices(400, 160, 0, hero);
    expect("coin_miss", is_hit(hero, coin_box), false);

    // Inset: Role::isHit uses x+1 .. x+WIDTH-1. A 32px hero at x=-32 has
    // its right-inset vertex at x=-1, just outside a tile that starts at 0.
    // At x=-31 the right-inset vertex sits on x=0 and counts as a hit.
    MapRec origin{0, 0, 1, 1, 1};
    expect("inset_miss_left", hit_map(-32, 0, 0, &origin, 1, 1), false);
    expect("inset_edge_left", hit_map(-31, 0, 0, &origin, 1, 1), true);
    expect("inset_hit_left", hit_map(-20, 0, 0, &origin, 1, 1), true);

    // Camera: world-x = -x0 + screen_x. Hero pinned at XRIGHT=192,
    // x0 = -1000 → world x = 1192. Pipe at tile 36 starts at 1152.
    expect("camera_shifted_pipe",
           hit_map(kXRight, 230, -1000, maps.data(), static_cast<int>(maps.size()), 1),
           true);

    if (fail) {
        std::printf("\n%d check(s) failed\n", fail);
        return 1;
    }
    std::printf("\nall collision checks passed\n");
    return 0;
}

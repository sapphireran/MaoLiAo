#include "portable/aabb.hpp"
#include "portable/level_data.hpp"

#include <iostream>

namespace {

int gFailures = 0;

void expect(const char* name, bool ok) {
    if (!ok) {
        std::cerr << "FAIL " << name << "\n";
        ++gFailures;
    } else {
        std::cout << "PASS " << name << "\n";
    }
}

} // namespace

int main() {
    using namespace mla;
    using namespace mla::world1;

    // Standing on the starting lawn: hero at (64, 256) is y = 8*32, lawn is y=9.
    // Probe one pixel into the lawn from above.
    const int lawn = hitMapIndex(kX0, 9 * kHeight - kHeight + 1, 0, kMap, kMapCount);
    expect("stand_on_start_lawn", lawn >= 0 && kMap[lawn].id == 1);

    // Floating in the sky above the lawn should miss.
    expect("air_above_lawn", hitMapIndex(kX0, 2 * kHeight, 0, kMap, kMapCount) == -1);

    // Cloud at (10,6) w=4: world box [320, 448) x [192, 224).
    const int cloud = hitMapIndex(10 * kWidth + 8, 6 * kHeight + 8, 0, kMap, kMapCount);
    expect("cloud_is_solid", cloud >= 0 && kMap[cloud].id == 2);

    // Grass tuft id 11 at (19,6) must NOT collide.
    const int tuft = hitMapIndex(19 * kWidth + 8, 6 * kHeight + 8, 0, kMap, kMapCount);
    expect("scenery_does_not_collide", tuft == -1 || kMap[tuft].id != 11);

    // Pipe mouth id 10 is 64x64. A point in the extra 32 px should still hit.
    const Tile pipe{36, 7, 10, 1, 1};
    const auto pipeBox = tileRect(pipe);
    expect("pipe_is_64_wide", pipeBox.x1 - pipeBox.x0 == 64);
    expect("pipe_is_64_tall", pipeBox.y1 - pipeBox.y0 == 64);

    // isHit only tests four inset corners. A small box in the actor's
    // interior (or a 32x32 tile that only overlaps the discarded 1 px
    // border) can miss even though the full sprites overlap.
    const Rect interiorSpike{10, 10, 14, 14};
    const auto corners = actorCorners(0, 0, 0);
    expect("interior_spike_misses_corners", !isHit(corners, interiorSpike));
    const Rect fullOverlap{0, 0, 32, 32};
    expect("full_tile_hits_inset_corner", isHit(corners, fullOverlap));

    // Coin at (10,5).
    const auto heroOnCoin = actorCorners(10 * kWidth, 5 * kHeight, 0);
    expect("coin_collect", isHit(heroOnCoin, coinRect(10, 5)));
    expect("coin_miss", !isHit(heroOnCoin, coinRect(0, 1)));

    // Food pixel box at (14*32, 5*32).
    expect("food_overlap", isHit(actorCorners(kFood.x, kFood.y, 0), foodRect(kFood.x, kFood.y)));

    // Stomp vs side-hit: same boxes, different vY interpretation (documented).
    const Rect goomba = enemyRect(3 * kWidth, 8 * kHeight);
    const auto faller = actorCorners(3 * kWidth, 8 * kHeight, 0);
    expect("enemy_overlap", isHit(faller, goomba));

    const Tile pipeTile{0, 0, 8, 1, 1};
    expect("w3_pipe_kills_without_star", world3Lethal(pipeTile, false));
    expect("w3_pipe_safe_with_star", !world3Lethal(pipeTile, true));
    const Tile cloudTile{0, 0, 2, 1, 1};
    expect("w3_cloud_always_safe", !world3Lethal(cloudTile, false));
    const Tile tuftTile{0, 0, 11, 1, 1};
    expect("w3_scenery_not_lethal", !world3Lethal(tuftTile, false));

    if (gFailures) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "aabb_collision: all checks passed\n";
    return 0;
}

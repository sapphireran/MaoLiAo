// 05_map_layout — dump world 1/2 tables and check pickup placement.
//
// Prints a coarse occupancy grid (solids vs decor) and reports the
// MAP_NUMBER=30 cap that drops world-1 water and the world-2 goal sign.

#include "maoliao_core.hpp"

#include <cstdio>
#include <cstring>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        ++g_failed;
    }
}

bool solid_at(const maoliao::MapTile* tiles, int count, int tx, int ty)
{
    for (int i = 0; i < count; ++i) {
        const maoliao::MapTile& t = tiles[i];
        if (!maoliao::tile_is_solid(t.id)) {
            continue;
        }
        const int cell_w = (t.id == 8 || t.id == 10) ? 2 : 1;
        const int cell_h = (t.id == 8 || t.id == 10) ? 2 : 1;
        const int x1 = t.x + t.x_amount * cell_w;
        const int y1 = t.y + t.y_amount * cell_h;
        if (tx >= t.x && tx < x1 && ty >= t.y && ty < y1) {
            return true;
        }
    }
    return false;
}

// A coin/enemy is "supported" if the cell itself is solid or the cell
// immediately below is solid (standing on a platform).
bool supported(const maoliao::MapTile* tiles, int count, int tx, int ty)
{
    return solid_at(tiles, count, tx, ty) || solid_at(tiles, count, tx, ty + 1);
}

void print_id_histogram(const maoliao::MapTile* tiles, int count, const char* label)
{
    int hist[16];
    std::memset(hist, 0, sizeof(hist));
    for (int i = 0; i < count; ++i) {
        const int id = tiles[i].id;
        if (id >= 0 && id < 16) {
            ++hist[id];
        }
    }
    std::printf("  %s id histogram:", label);
    for (int id = 1; id < 16; ++id) {
        if (hist[id] != 0) {
            std::printf("  %d×id%d", hist[id], id);
        }
    }
    std::printf("\n");
}

}  // namespace

int main()
{
    std::printf("05_map_layout\n");

    std::printf("  world1 authored=%d copied=%d (MAP_NUMBER=%d)\n",
                maoliao::kWorld1MapCount,
                maoliao::copied_map_count(maoliao::kWorld1MapCount),
                maoliao::kMapNumber);
    std::printf("  world2 authored=%d copied=%d\n", maoliao::kWorld2MapCount,
                maoliao::copied_map_count(maoliao::kWorld2MapCount));

    expect(maoliao::kWorld1MapCount == 32, "world 1 has 32 authored tiles");
    expect(maoliao::copied_map_count(maoliao::kWorld1MapCount) == 30,
           "world 1 copies 30, drops 2 water tiles");
    expect(maoliao::kWorld2MapCount == 36, "world 2 has 36 authored tiles");
    expect(maoliao::copied_map_count(maoliao::kWorld2MapCount) == 30,
           "world 2 copies 30");

    // Last two world-1 records are water (id 13) at (71,10) and (75,10).
    expect(maoliao::kWorld1Map[30].id == 13 && maoliao::kWorld1Map[30].x == 71,
           "slot 31 is water at (71,10) — dropped in-game");
    expect(maoliao::kWorld1Map[31].id == 13 && maoliao::kWorld1Map[31].x == 75,
           "slot 32 is water at (75,10) — dropped in-game");

    // World 2 index 31 is the goal sign (0-based 31).
    expect(maoliao::kWorld2Map[31].id == 12 && maoliao::kWorld2Map[31].x == 111,
           "world 2 goal sign sits past MAP_NUMBER");

    print_id_histogram(maoliao::kWorld1Map, maoliao::kWorld1MapCount, "world1 all");
    print_id_histogram(maoliao::kWorld1Map, maoliao::copied_map_count(maoliao::kWorld1MapCount),
                       "world1 copied");

    // Friction assignment matches createMap world 1.
    {
        maoliao::MapTile grass = {0, 9, 1, 15, 1, 0};
        maoliao::assign_friction(grass, 1);
        maoliao::MapTile ice = {67, 9, 6, 4, 1, 0};
        maoliao::assign_friction(ice, 1);
        const double u2 = (maoliao::kVMax / maoliao::kT2) / maoliao::kG;
        const double u1 = (maoliao::kVMax / maoliao::kT1) / maoliao::kG;
        expect(grass.u == u2, "id 1 uses T2 friction");
        expect(ice.u == u1, "id 6 uses T1 friction");
        expect(ice.u > grass.u, "T1 u is larger (stops faster)");
    }

    // Most coins sit on or just above a solid. The x=74 stack is a
    // vertical column hanging above the isolated snow block at (74,9):
    // only (74,8) is supported; (74,5..7) float on purpose.
    int floating_coins = 0;
    for (int i = 0; i < maoliao::kWorld1CoinCount; ++i) {
        const maoliao::Vec2i c = maoliao::kWorld1Coins[i];
        if (!supported(maoliao::kWorld1Map, maoliao::kWorld1MapCount, c.x, c.y)) {
            std::printf("  floating coin (%d,%d)\n", c.x, c.y);
            ++floating_coins;
        }
    }
    expect(floating_coins == 3, "three stacked coins float above (74,9)");
    expect(maoliao::kWorld1CoinCount == 20, "world 1 coin count");

    // (66,8) sits in the pit between the snow run (ends x=63) and the
    // ice patch (starts x=67) — a troll spawn, same as the original list.
    int floating_enemies = 0;
    int pit_enemy = 0;
    for (int i = 0; i < maoliao::kWorld1EnemyCount; ++i) {
        const maoliao::Enemy e = maoliao::kWorld1EnemiesTiles[i];
        if (!supported(maoliao::kWorld1Map, maoliao::kWorld1MapCount, e.x, e.y)) {
            std::printf("  floating enemy tile (%d,%d) turn=%d\n", e.x, e.y,
                        e.turn);
            ++floating_enemies;
            if (e.x == 66 && e.y == 8) {
                pit_enemy = 1;
            }
        }
    }
    expect(floating_enemies == 1 && pit_enemy == 1,
           "only the pit enemy at (66,8) is unsupported");
    expect(maoliao::kWorld1EnemyCount == 10, "world 1 enemy count");

    // Food is pixel-space: 14*32, 5*32.
    expect(maoliao::kWorld1FoodPx[0].x == 14 * 32, "food x is pixels");
    expect(maoliao::kWorld1FoodPx[0].y == 5 * 32, "food y is pixels");

    // Enemy create multiplies by 32.
    {
        const maoliao::Enemy px = maoliao::enemy_from_tile(maoliao::kWorld1EnemiesTiles[0]);
        expect(px.x == 3 * 32 && px.y == 8 * 32 && px.turn == 1,
               "first enemy → (96, 256, +1)");
    }

    // Ending distances.
    expect(maoliao::ending_distance(1) == 94 * 32, "world 1 finish 3008 px");
    expect(maoliao::ending_distance(2) == 104 * 32, "world 2 finish 3328 px");
    expect(maoliao::ending_distance(3) == 94 * 32, "world 3 finish 3008 px");
    expect(maoliao::is_ending_distance(1, 94 * 32 + 1), "distance > 94 tiles");
    expect(!maoliao::is_ending_distance(1, 94 * 32), "equal is not yet ending");

    // ASCII slice of world 1 rows 3–10, columns 0–40 (start of the level).
    std::printf("  world 1 slice rows 3-10, cols 0-39  (# solid . empty)\n");
    for (int y = 3; y <= 10; ++y) {
        std::printf("    y=%2d ", y);
        for (int x = 0; x < 40; ++x) {
            std::putchar(solid_at(maoliao::kWorld1Map, maoliao::kWorld1MapCount, x, y)
                             ? '#'
                             : '.');
        }
        std::putchar('\n');
    }

    if (g_failed != 0) {
        std::printf("05_map_layout: %d failed\n", g_failed);
        return 1;
    }
    std::printf("05_map_layout: all passed\n");
    return 0;
}

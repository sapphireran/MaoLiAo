// 10_world_walk — hold right across world 1 using the authored solids.
//
// Not a full Role::action port (no enemy AI). It does use the same
// horizontal integrator, camera pin, ending distance, and coin AABB
// so a straight-line sprint's score, pit deaths, and finish time stay
// inspectable without EasyX.

#include "maoliao_core.hpp"

#include <cstdio>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        ++g_failed;
    }
}

bool column_has_solid(int tx, int ty)
{
    for (int i = 0; i < maoliao::kWorld1MapCount; ++i) {
        const maoliao::MapTile& t = maoliao::kWorld1Map[i];
        if (!maoliao::tile_is_solid(t.id)) {
            continue;
        }
        const maoliao::Aabb b = maoliao::tile_aabb(t);
        const int px = tx * maoliao::kWidth + maoliao::kWidth / 2;
        const int py = ty * maoliao::kHeight + maoliao::kHeight / 2;
        if (px >= b.left && px < b.right && py >= b.top && py < b.bottom) {
            return true;
        }
    }
    return false;
}

// Top pixel of the highest solid whose top is at or below feet_y.
// Returns -1 if nothing is underneath.
int surface_below(int world_px, int feet_y)
{
    int best = -1;
    for (int i = 0; i < maoliao::kWorld1MapCount; ++i) {
        const maoliao::MapTile& t = maoliao::kWorld1Map[i];
        if (!maoliao::tile_is_solid(t.id)) {
            continue;
        }
        const maoliao::Aabb b = maoliao::tile_aabb(t);
        if (world_px < b.left || world_px >= b.right) {
            continue;
        }
        if (b.top >= feet_y - 2 && (best < 0 || b.top < best)) {
            best = b.top;
        }
    }
    return best;
}

double floor_u_at(int world_px, int feet_y)
{
    for (int i = 0; i < maoliao::kWorld1MapCount; ++i) {
        maoliao::MapTile t = maoliao::kWorld1Map[i];
        if (!maoliao::tile_is_solid(t.id)) {
            continue;
        }
        maoliao::assign_friction(t, 1);
        const maoliao::Aabb b = maoliao::tile_aabb(t);
        if (world_px >= b.left && world_px < b.right && feet_y >= b.top - 2 &&
            feet_y <= b.top + 4) {
            return t.u;
        }
    }
    return maoliao::friction_u(1, 1);
}

int collect_coins(int world_px, int hero_y, bool taken[])
{
    int gained = 0;
    const int sx = world_px;  // treat hero world-x as the actor x
    for (int i = 0; i < maoliao::kWorld1CoinCount; ++i) {
        if (taken[i]) {
            continue;
        }
        const maoliao::Vec2i c = maoliao::kWorld1Coins[i];
        maoliao::Aabb box;
        box.left = c.x * maoliao::kWidth;
        box.top = c.y * maoliao::kHeight;
        box.right = box.left + maoliao::kWidth;
        box.bottom = box.top + maoliao::kHeight;
        if (maoliao::actor_hits_aabb(sx, hero_y, box)) {
            taken[i] = true;
            gained += 10;
        }
    }
    return gained;
}

}  // namespace

int main()
{
    std::printf("10_world_walk\n");

    // Sanity: start grass occupies columns 0–14 at row 9.
    expect(column_has_solid(2, 9), "spawn column is solid");
    expect(!column_has_solid(16, 9), "gap at column 16 (the first pit)");
    expect(column_has_solid(18, 8), "raised shelf at column 18");

    maoliao::HeroState h = maoliao::make_hero();
    // Stand on the grass: row 9 top is 288, hero height 32 → y = 256.
    h.y = 8 * maoliao::kHeight;
    h.yy = h.y;
    h.x = maoliao::kX0;
    h.xx = h.x;
    h.is_fly = false;

    bool taken[32];
    for (int i = 0; i < 32; ++i) {
        taken[i] = false;
    }

    const int hold = maoliao::pack_command(false, true, false, false, false,
                                           false);
    int ticks = 0;
    int first_pin = -1;
    int first_ending = -1;
    int death_tick = -1;
    const int kMaxTicks = 4000;

    for (; ticks < kMaxTicks; ++ticks) {
        const int wx = maoliao::world_x(h);
        const int feet = h.y + maoliao::kHeight;
        const int surf = surface_below(wx + maoliao::kWidth / 2, feet);
        const bool on_floor = (surf >= 0 && feet >= surf - 2 && feet <= surf + 6 &&
                               !h.is_fly) ||
                              (surf >= 0 && h.is_fly && h.vy > 0.0 &&
                               h.y + 1 >= surf - 1);

        if (!h.is_fly && surf < 0) {
            h.is_fly = true;
        }

        const double u = on_floor ? floor_u_at(wx + maoliao::kWidth / 2, feet)
                                  : 0.0;
        maoliao::tick_horizontal(h, hold, on_floor && !h.is_fly, u);

        if (h.is_fly || surf < 0) {
            const bool probe = (surf >= 0 && h.vy > 0.0 && h.y + 1 >= surf - 1);
            maoliao::tick_vertical(h, probe);
        } else if (surf >= 0 && h.y + maoliao::kHeight != surf) {
            // Stay glued while walking onto a same-height floor.
            if (h.y + maoliao::kHeight > surf + 8) {
                // buried; snap up
                h.y = surf - maoliao::kHeight;
                h.yy = h.y;
            }
        }

        h.score += collect_coins(maoliao::world_x(h), h.y, taken);

        if (first_pin < 0 && h.x == maoliao::kXRight && h.x0 < 0.0) {
            first_pin = ticks;
        }
        if (first_ending < 0 &&
            maoliao::is_ending_distance(1, maoliao::world_x(h))) {
            first_ending = ticks;
            h.ending = true;
        }
        if (h.died && death_tick < 0) {
            death_tick = ticks;
            break;
        }
        if (h.passed) {
            break;
        }
    }

    int coins_taken = 0;
    for (int i = 0; i < maoliao::kWorld1CoinCount; ++i) {
        if (taken[i]) {
            ++coins_taken;
        }
    }

    std::printf("  ticks=%d  world_x=%d  screen_x=%d  x0=%.1f  y=%d\n", ticks,
                maoliao::world_x(h), h.x, h.x0, h.y);
    std::printf("  score=%d  coins=%d/%d  pin@%d  ending@%d  death@%d  passed=%d\n",
                h.score, coins_taken, maoliao::kWorld1CoinCount, first_pin,
                first_ending, death_tick, h.passed ? 1 : 0);

    expect(first_pin >= 0, "camera pinned at some point");
    expect(h.x0 < 0.0, "scrolled right");

    // A no-jump sprint falls into the first pit (columns 15–17) unless
    // the raised shelf is reached by jumping. Document that: death
    // happens, and it happens before the ending distance.
    expect(death_tick >= 0, "no-jump sprint dies in a pit");
    expect(first_ending < 0, "never reached the 94-tile ending");
    expect(h.score == coins_taken * 10, "score is 10 per coin");
    expect(coins_taken <= 4, "at most the opening cloud coins before the pit");

    // Second run: jump whenever the probe ahead is empty (auto-hop).
    {
        maoliao::HeroState j = maoliao::make_hero();
        j.y = 8 * maoliao::kHeight;
        j.yy = j.y;
        j.x = maoliao::kX0;
        j.xx = j.x;
        j.is_fly = false;
        bool taken2[32];
        for (int i = 0; i < 32; ++i) {
            taken2[i] = false;
        }
        int end2 = -1;
        int die2 = -1;
        int t = 0;
        for (; t < kMaxTicks; ++t) {
            const int wx = maoliao::world_x(j);
            const int feet = j.y + maoliao::kHeight;
            const int mid = wx + maoliao::kWidth / 2;
            const int ahead = mid + maoliao::kWidth;
            const int surf = surface_below(mid, feet);
            const int surf_ahead = surface_below(ahead, feet);
            const bool on_floor =
                surf >= 0 && !j.is_fly && feet >= surf - 2 && feet <= surf + 6;

            if (on_floor && surf_ahead < 0) {
                maoliao::apply_jump(j);
            }
            if (!j.is_fly && surf < 0) {
                j.is_fly = true;
            }

            const double u = on_floor ? floor_u_at(mid, feet) : 0.0;
            maoliao::tick_horizontal(j, hold, on_floor, u);

            if (j.is_fly) {
                const bool probe = (surf >= 0 && j.vy > 0.0 && j.y + 1 >= surf - 1);
                maoliao::tick_vertical(j, probe);
            }

            j.score += collect_coins(maoliao::world_x(j), j.y, taken2);

            if (end2 < 0 && maoliao::is_ending_distance(1, maoliao::world_x(j))) {
                end2 = t;
                j.ending = true;
            }
            if (j.died) {
                die2 = t;
                break;
            }
            if (j.passed) {
                break;
            }
        }
        int c2 = 0;
        for (int i = 0; i < maoliao::kWorld1CoinCount; ++i) {
            if (taken2[i]) {
                ++c2;
            }
        }
        std::printf("  auto-hop: ticks=%d  world_x=%d  y=%d  coins=%d  "
                    "ending@%d  death@%d  passed=%d\n",
                    t, maoliao::world_x(j), j.y, c2, end2, die2,
                    j.passed ? 1 : 0);
        expect(die2 < 0 || end2 >= 0 || maoliao::world_x(j) > 20 * 32,
               "auto-hop clears the first pit");
        expect(c2 >= coins_taken, "hopping is not worse than walking in");
    }

    if (g_failed != 0) {
        std::printf("10_world_walk: %d failed\n", g_failed);
        return 1;
    }
    std::printf("10_world_walk: all passed\n");
    return 0;
}

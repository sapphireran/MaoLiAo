// 02_aabb — Role::isHit vertex-in-box test and pipe-sized tiles.
//
// The game does not use a full AABB intersection. It tests the four
// inset corners of a 32×32 actor against the tile rectangle. This
// program locks that rule down, including the 2×2 pipe exception.

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

}  // namespace

int main()
{
    std::printf("02_aabb\n");

    const maoliao::Aabb floor = {0, 288, 480, 320};  // y=9 row of tiles

    // Standing on the grass: feet vertices sit on y=287+1? Hero at
    // y = 256 (row 8) has bottom vertices at 256+31 = 287, which is
    // above 288 — no hit. Hero at y = 257 has bottom 288 — hit.
    expect(!maoliao::actor_hits_aabb(64, 256, floor),
           "hero on row 8 (y=256) does not touch row-9 floor");
    expect(maoliao::actor_hits_aabb(64, 257, floor),
           "hero at y=257 bottom vertices enter the floor");

    // Completely above.
    expect(!maoliao::actor_hits_aabb(64, 100, floor), "airborne, no hit");

    // Inset: a box that only overlaps the 1-px margin should miss.
    // Actor at x=479: right vertex is 479+31=510, left is 480. Floor
    // right is 480, so left vertex 480 is on the edge → hit.
    expect(maoliao::actor_hits_aabb(479, 288, floor),
           "edge-inclusive: left vertex on box.right counts");

    // Actor whose entire body is to the right of the floor.
    expect(!maoliao::actor_hits_aabb(481, 288, floor),
           "fully to the right misses (left vertex 482 > 480)");

    // Vertex helper matches role.cpp (±1 inset).
    {
        maoliao::Vec2i v[4];
        maoliao::actor_vertices(10, 20, v);
        expect(v[0].x == 11 && v[0].y == 21, "top-left inset");
        expect(v[1].x == 41 && v[1].y == 21, "top-right inset");
        expect(v[2].x == 11 && v[2].y == 51, "bottom-left inset");
        expect(v[3].x == 41 && v[3].y == 51, "bottom-right inset");
    }

    // Regular tile 1×3 at (0,9).
    {
        maoliao::MapTile t = {0, 9, 1, 15, 1, 0};
        const maoliao::Aabb b = maoliao::tile_aabb(t);
        expect(b.left == 0 && b.top == 288, "tile origin in pixels");
        expect(b.right == 15 * 32 && b.bottom == 320, "1-high × 15-wide");
        expect(maoliao::tile_is_solid(1), "id 1 is solid");
        expect(!maoliao::tile_is_solid(12), "goal sign is not solid");
        expect(!maoliao::tile_is_solid(11), "decor grass is not solid");
    }

    // Pipe mouths (id 8 / 10) are 2×2 tile cells.
    {
        maoliao::MapTile pipe = {36, 7, 10, 1, 1, 0};
        const maoliao::Aabb b = maoliao::tile_aabb(pipe);
        expect(b.left == 36 * 32 && b.top == 7 * 32, "pipe origin");
        expect(b.right == 36 * 32 + 64 && b.bottom == 7 * 32 + 64,
               "pipe 2×2 cell");
        expect(maoliao::actor_hits_aabb(36 * 32, 7 * 32, b),
               "standing in the pipe mouth hits");
        expect(!maoliao::actor_hits_aabb(36 * 32 + 64, 7 * 32, b),
               "one cell to the right of a 2-wide pipe misses");
    }

    if (g_failed != 0) {
        std::printf("02_aabb: %d failed\n", g_failed);
        return 1;
    }
    std::printf("02_aabb: all passed\n");
    return 0;
}

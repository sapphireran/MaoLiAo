#include "check.hpp"
#include "collision.hpp"
#include "constants.hpp"
#include "map_data.hpp"

#include <cstdio>
#include <string>

// Replay Role::bullteFlying without EasyX:
//   x += 4 * turn each tick
//   expire at window edges, x > MAX_DISTANCE (480), or a solid hit.

struct Shot {
    int x;
    int y;
    int turn;
    int frames;
    const char* reason;
};

static bool hitsSolid(int x, int y, const mla::MapRec* map, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        if (map[i].id < 1 || map[i].id > 10) {
            continue;
        }
        const mla::Rect box =
            mla::mapBox(map[i].x, map[i].y, map[i].id, map[i].xAmount, map[i].yAmount);
        if (mla::vertexHitsRect(mla::Point(x, y), box)) {
            return true;
        }
    }
    return false;
}

static Shot fly(int x, int y, int turn, const mla::MapRec* map, std::size_t n) {
    Shot s;
    s.x = x;
    s.y = y;
    s.turn = turn;
    s.frames = 0;
    s.reason = "alive";
    for (int i = 0; i < 400; ++i) {
        ++s.frames;
        if (s.x > mla::kXSize || s.x < -mla::kWidth) {
            s.reason = "off_window";
            break;
        }
        if (hitsSolid(s.x, s.y, map, n) || s.x > mla::kMaxDistance) {
            s.reason = (s.x > mla::kMaxDistance && !hitsSolid(s.x, s.y, map, n))
                           ? "max_distance"
                           : "hit_solid";
            break;
        }
        s.x += mla::kBulletStep * s.turn;
    }
    return s;
}

int main() {
    std::printf("=== 09 bullet flight ===\n");
    std::printf("step=%d  MAX_DISTANCE=%d  window=%d\n", mla::kBulletStep,
                mla::kMaxDistance, mla::kXSize);

    std::size_t n = 0;
    const mla::MapRec* w1 = mla::world1Map(n);
    const std::size_t loaded = mla::loadedCount(n);

    // Open sky at spawn height, facing right — should die on MAX_DISTANCE, not
    // the window, because 480 < 512.
    const Shot sky = fly(64, 64, 1, w1, loaded);
    std::printf("sky shot  start=64,64  end=%d  frames=%d  %s\n", sky.x, sky.frames,
                sky.reason);

    // Cloud at tile (10,6) sits at x=320, inside MAX_DISTANCE. A mid-height
    // pipe at x=36*32=1152 is past 480, so window-X range would expire first.
    const Shot cloud = fly(200, 6 * 32, 1, w1, loaded);
    std::printf("cloud shot start=200,192 end=%d  frames=%d  %s\n", cloud.x,
                cloud.frames, cloud.reason);

    // Facing left from mid-air near the left edge.
    const Shot left = fly(20, 40, -1, w1, loaded);
    std::printf("left shot start=20,40   end=%d  frames=%d  %s\n", left.x,
                left.frames, left.reason);

    // Spawn already past MAX_DISTANCE (the right-clamp quirk in role.cpp).
    const Shot late = fly(500, 40, 1, w1, loaded);
    std::printf("late shot start=500     end=%d  frames=%d  %s\n", late.x,
                late.frames, late.reason);

    mla::Checks c;
    c.expect(std::string(sky.reason) == "max_distance",
             "open sky expires at MAX_DISTANCE 480, not 512");
    c.expect(sky.x > mla::kMaxDistance - mla::kBulletStep, "sky x near 480");
    c.expect(std::string(cloud.reason) == "hit_solid",
             "cloud at x=320 stops a shot before MAX_DISTANCE");
    c.expect(cloud.x >= 320 - mla::kWidth && cloud.x <= 448,
             "impact overlaps the (10,6) cloud AABB");
    c.expect(std::string(left.reason) == "off_window", "leftward shot leaves x < -32");
    c.expect(std::string(late.reason) == "max_distance" ||
                 std::string(late.reason) == "off_window",
             "shot spawned at x=500 dies immediately (window-X quirk)");
    c.expect(mla::kBulletInterval == 0.2, "hold-to-fire interval is 0.2 s");
    return c.finish("09_bullet_flight");
}

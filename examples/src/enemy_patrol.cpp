#include "maoliao_model.hpp"

#include <iostream>
#include <vector>

using namespace maoliao;

static int fail(const char* msg) {
    std::cerr << "FAIL: " << msg << "\n";
    return 1;
}

struct Walker {
    int x = 0; // world pixels
    int y = 0;
    int turn = 1;
};

static Box platform() {
    // 5 tiles starting at column 10, row 8 → [320, 480] x [256, 288]
    return tile_box(10, 8, 5, 1, 1);
}

static bool body_hits(const Walker& e, const Box& ground) {
    Box body{e.x, e.y, e.x + WIDTH, e.y + HEIGHT};
    Point c[4] = {
        {e.x + 1, e.y + 1},
        {e.x + WIDTH - 1, e.y + 1},
        {e.x + 1, e.y + HEIGHT - 1},
        {e.x + WIDTH - 1, e.y + HEIGHT - 1},
    };
    (void)body;
    return corner_hits(c, ground);
}

static bool ahead_has_ground(const Walker& e, const Box& ground) {
    const int px = e.x + e.turn * WIDTH;
    const int py = e.y + 1;
    Point c[4] = {
        {px + 1, py + 1},
        {px + WIDTH - 1, py + 1},
        {px + 1, py + HEIGHT - 1},
        {px + WIDTH - 1, py + HEIGHT - 1},
    };
    return corner_hits(c, ground);
}

static void step(Walker& e, const Box& ground) {
    e.x += e.turn * ENEMY_STEP;
    if (body_hits(e, ground) || !ahead_has_ground(e, ground)) e.turn *= -1;
}

enum class Contact { Stomp, Death, None };

static Contact hero_vs_enemy(double v_y, bool overlap) {
    if (!overlap) return Contact::None;
    if (v_y > 0.0) return Contact::Stomp;
    return Contact::Death;
}

int main() {
    const Box ground = platform();
    std::cout << "platform [" << ground.x0 << "," << ground.x1 << "] x [" << ground.y0
              << "," << ground.y1 << "]\n";

    Walker e;
    e.x = 10 * WIDTH + 16;
    e.y = 7 * HEIGHT; // standing in the row above the platform art
    e.turn = 1;

    // Place the walker so its body sits on the platform (y = 256-32 = 224? row 7 is 224).
    // Ahead probe uses y+1; we need the body box to overlap the platform top.
    e.y = ground.y0 - HEIGHT; // 224; feet at 256, probe y+1 = 225 → bottom 255? 
    // Inset bottom of ahead probe: (y+1)+31 = y+32. For y=224, ahead bottom=256, on the top edge.

    int reversals = 0;
    int last_turn = e.turn;
    std::vector<int> xs;
    for (int i = 0; i < 400; ++i) {
        step(e, ground);
        xs.push_back(e.x);
        if (e.turn != last_turn) {
            ++reversals;
            last_turn = e.turn;
            std::cout << "  reverse #" << reversals << " at x=" << e.x << " turn=" << e.turn << "\n";
        }
    }
    if (reversals < 2) return fail("should reverse at both ends of a short ledge");

    int xmin = xs[0], xmax = xs[0];
    for (int x : xs) {
        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
    }
    std::cout << "patrol x in [" << xmin << "," << xmax << "] reversals=" << reversals << "\n";
    if (xmin < ground.x0 - WIDTH || xmax > ground.x1 + WIDTH) {
        return fail("walker wandered more than one sprite off the platform");
    }

    if (hero_vs_enemy(8.0, true) != Contact::Stomp) return fail("falling stomp");
    if (hero_vs_enemy(-14.0, true) != Contact::Death) return fail("rising jump is death");
    if (hero_vs_enemy(0.0, true) != Contact::Death) return fail("walk-in is death");
    if (hero_vs_enemy(8.0, false) != Contact::None) return fail("no overlap");

    int score = 0;
    if (hero_vs_enemy(1.0, true) == Contact::Stomp) score = apply_pickup(score, Pickup::Stomp);
    if (score != SCORE_ENEMY) return fail("stomp points");

    std::cout << "enemy_patrol: ok\n";
    return 0;
}

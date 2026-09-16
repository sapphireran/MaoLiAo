#include "check.hpp"
#include "constants.hpp"
#include "map_data.hpp"

#include <cstdio>
#include <set>
#include <string>
#include <vector>

// Scoring table from Role::action / bullteFlying, plus a replay of
// walking the world-1 coin cells in tile order.

enum EventKind { kCoin, kStomp, kBullet };

struct Event {
    EventKind kind;
    int x;
    int y;
};

static int points(EventKind k) {
    if (k == kCoin) {
        return 10;
    }
    return 5;
}

static const char* name(EventKind k) {
    if (k == kCoin) {
        return "coin";
    }
    if (k == kStomp) {
        return "stomp";
    }
    return "bullet";
}

int main() {
    std::printf("=== 10 score events ===\n");
    std::printf("coin=+10  stomp=+5  bullet=+5  lives=%d\n", mla::kLife);

    std::size_t n = 0;
    const mla::PointRec* coins = mla::world1Coins(n);

    std::vector<Event> run;
    for (std::size_t i = 0; i < n; ++i) {
        Event e;
        e.kind = kCoin;
        e.x = coins[i].x;
        e.y = coins[i].y;
        run.push_back(e);
    }
    // World 1 has 10 enemies; stomp three, shoot two (as a sample clear).
    const Event extra[] = {
        {kStomp, 3, 8}, {kStomp, 18, 7}, {kStomp, 25, 7},
        {kBullet, 28, 8}, {kBullet, 33, 8},
    };
    for (std::size_t i = 0; i < sizeof(extra) / sizeof(extra[0]); ++i) {
        run.push_back(extra[i]);
    }

    int score = 0;
    int coinsGot = 0;
    int stomps = 0;
    int bullets = 0;
    std::set<std::string> seen;
    for (std::size_t i = 0; i < run.size(); ++i) {
        const Event& e = run[i];
        char key[32];
        std::snprintf(key, sizeof(key), "%s:%d:%d", name(e.kind), e.x, e.y);
        if (seen.count(key)) {
            std::printf("  skip duplicate %s\n", key);
            continue;
        }
        seen.insert(key);
        score += points(e.kind);
        if (e.kind == kCoin) {
            ++coinsGot;
        } else if (e.kind == kStomp) {
            ++stomps;
        } else {
            ++bullets;
        }
        std::printf("  %+d  %-6s (%d,%d)  total=%d\n", points(e.kind), name(e.kind),
                    e.x, e.y, score);
    }

    const int expect = coinsGot * 10 + stomps * 5 + bullets * 5;
    std::printf("picked %d/%zu coins  stomps=%d  bullets=%d  score=%d\n", coinsGot, n,
                stomps, bullets, score);

    mla::Checks c;
    c.expect(n == 20, "world1 has 20 authored coins");
    c.expect(coinsGot == 20, "all world1 coins collected in the replay");
    c.expect(score == expect, "score matches 10/5/5 table");
    c.expect(score == 20 * 10 + 3 * 5 + 2 * 5, "200 + 15 + 10 = 225");
    c.expect(mla::kLife == 5, "LIFE is 5");

    // Duplicate coin should not add again (setScorePos + zeroing in the game).
    Event dup = run[0];
    const int before = score;
    char key[32];
    std::snprintf(key, sizeof(key), "%s:%d:%d", name(dup.kind), dup.x, dup.y);
    if (!seen.count(key)) {
        score += points(dup.kind);
    }
    c.expect(score == before, "second pickup of the same coin is ignored");
    return c.finish("10_score_events");
}

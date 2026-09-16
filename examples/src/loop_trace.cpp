// Replay the life / world state machine from main.cpp with scripted events.

#include "maoliao_model.h"

#include <cstdio>
#include <vector>

using namespace maoliao;

int main() {
    LoopState s;

    // Five deaths on world 1 → game over, back to menu, life restored.
    for (int i = 0; i < 4; ++i) {
        LoopEvent e = applyDeath(s);
        MLA_CHECK(e == LoopEvent::Died);
        MLA_CHECK(s.life == kLife - 1 - i);
        MLA_CHECK(s.world == 1);
        MLA_CHECK(!s.inMenu);
    }
    MLA_CHECK(applyDeath(s) == LoopEvent::GameOver);
    MLA_CHECK(s.life == kLife && s.world == 1 && s.inMenu);

    // Clear worlds 1 → 2 → 3 → all_clear.
    s = LoopState{};
    MLA_CHECK(applyPass(s) == LoopEvent::Passed && s.world == 2);
    MLA_CHECK(applyPass(s) == LoopEvent::Passed && s.world == 3);
    MLA_CHECK(applyPass(s) == LoopEvent::AllClear);
    MLA_CHECK(s.world == 1 && s.life == kLife && s.inMenu);

    // Pause virtual keys.
    s = LoopState{};
    s.world = 2;
    s.life = 3;
    s.score = 40;
    MLA_CHECK(applyKey(s, kVirRestart) == LoopEvent::Restart);
    MLA_CHECK(s.world == 2 && s.life == 3);  // restart does not refill lives
    MLA_CHECK(applyKey(s, kVirHome) == LoopEvent::Home);
    MLA_CHECK(s.world == 1 && s.life == kLife && s.score == 0 && s.inMenu);

    // Score events from Role::action (documented, not simulated physics).
    int score = 0;
    score += 10;  // coin
    score += 5;   // stomp
    score += 5;   // bullet
    MLA_CHECK(score == 20);

    // Scripted campaign trace for the log.
    s = LoopState{};
    struct Trace {
        const char* what;
        LoopEvent got;
        int life;
        int world;
    };
    std::vector<Trace> log;
    auto rec = [&](const char* what, LoopEvent e) {
        log.push_back({what, e, s.life, s.world});
    };
    rec("start", LoopEvent::None);
    rec("pass_w1", applyPass(s));
    rec("die", applyDeath(s));
    rec("pass_w2", applyPass(s));
    rec("pass_w3", applyPass(s));

    MLA_CHECK(log.size() == 5);
    MLA_CHECK(log[1].world == 2 && log[1].got == LoopEvent::Passed);
    MLA_CHECK(log[2].life == 4 && log[2].got == LoopEvent::Died);
    MLA_CHECK(log[3].world == 3);
    MLA_CHECK(log[4].got == LoopEvent::AllClear && log[4].world == 1);

    std::printf("loop_trace ok\n");
    for (const auto& t : log) {
        std::printf("  %-10s event=%-10s life=%d world=%d\n",
                    t.what, eventName(t.got), t.life, t.world);
    }
    return 0;
}

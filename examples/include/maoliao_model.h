// Portable slice of MaoLiAo rules: constants, kinematics, AABB,
// command bits, friction, save format, and the three campaign tables.
// No EasyX / Win32. Used by the examples/ demos only.

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace maoliao {

// --- constants from define.h (literal values, not the semicolon macros) ---

constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;
constexpr int kY0 = 3 * kHeight;
constexpr double kTime = 0.01;
constexpr int kStep = 10;
constexpr int kEnemyStep = 1;
constexpr int kMapBg = 5;
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;
constexpr double kRealHeight = 3.5;
constexpr double kUnrealHeight = 3 * kHeight + 5;
constexpr double kG = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;
constexpr int kLife = 5;

constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kFoodNumber = 5;
constexpr int kEnemyTote = 30;
constexpr int kMaxDistance = 480;
constexpr int kBulletStep = 4;

constexpr double kPixelPerMeter = kUnrealHeight / kRealHeight;

// --- kinematics (Inertia::move) ---

inline double move(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

inline double jumpSpeed() {
    return -std::sqrt(2.0 * kG * kRealHeight);
}

inline double toPixels(double meters) {
    return meters * kPixelPerMeter;
}

// --- AABB matching Role::isHit ---

struct Point {
    int x;
    int y;
};

inline void heroCorners(int screenX, int screenY, int x0, Point out[4]) {
    out[0] = {-x0 + screenX + 1, screenY + 1};
    out[1] = {-x0 + screenX + kWidth - 1, screenY + 1};
    out[2] = {-x0 + screenX + 1, screenY + kHeight - 1};
    out[3] = {-x0 + screenX + kWidth - 1, screenY + kHeight - 1};
}

inline bool isHit(const Point body[4], Point topLeft, Point bottomRight) {
    for (int i = 0; i < 4; ++i) {
        if (body[i].x >= topLeft.x && body[i].y >= topLeft.y &&
            body[i].x <= bottomRight.x && body[i].y <= bottomRight.y) {
            return true;
        }
    }
    return false;
}

// --- command mask ---

struct CommandBits {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool shoot = false;
    bool esc = false;
    bool virReturn = false;
    bool virRestart = false;
    bool virHome = false;
};

inline CommandBits decodeCommand(int key) {
    CommandBits c;
    c.left = (key & kCmdLeft) != 0;
    c.right = (key & kCmdRight) != 0;
    c.up = (key & kCmdUp) != 0;
    c.down = (key & kCmdDown) != 0;
    c.shoot = (key & kCmdShoot) != 0;
    c.esc = (key & kCmdEsc) != 0;
    c.virReturn = (key & kVirReturn) != 0;
    c.virRestart = (key & kVirRestart) != 0;
    c.virHome = (key & kVirHome) != 0;
    return c;
}

inline const char* commandName(int bit) {
    switch (bit) {
        case kCmdLeft: return "CMD_LEFT";
        case kCmdRight: return "CMD_RIGHT";
        case kCmdUp: return "CMD_UP";
        case kCmdDown: return "CMD_DOWN";
        case kCmdShoot: return "CMD_SHOOT";
        case kCmdEsc: return "CMD_ESC";
        case kVirReturn: return "VIR_RETURN";
        case kVirRestart: return "VIR_RESTART";
        case kVirHome: return "VIR_HOME";
        default: return "?";
    }
}

inline std::vector<const char*> commandList(int key) {
    static const int bits[] = {
        kCmdLeft, kCmdRight, kCmdUp, kCmdDown, kCmdShoot, kCmdEsc,
        kVirReturn, kVirRestart, kVirHome};
    std::vector<const char*> names;
    for (int bit : bits) {
        if (key & bit) names.push_back(commandName(bit));
    }
    return names;
}

// --- friction from tile id (createMap switch) ---

enum class Feel { High, Mid, Low };

inline Feel feelForIdWorld12(int id) {
    switch (id) {
        case 6: return Feel::High;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5: return Feel::Mid;
        default: return Feel::Low;
    }
}

inline Feel feelForIdWorld3(int id) {
    switch (id) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6: return Feel::High;
        case 2: return Feel::Mid;
        default: return Feel::Low;
    }
}

inline double muFromFeel(Feel feel) {
    switch (feel) {
        case Feel::High: return (kVMax / kT1) / kG;
        case Feel::Mid: return (kVMax / kT2) / kG;
        case Feel::Low: return (kVMax / kT3) / kG;
    }
    return 0;
}

inline const char* feelName(Feel feel) {
    switch (feel) {
        case Feel::High: return "high (T1=0.5)";
        case Feel::Mid: return "mid (T2=1.2)";
        case Feel::Low: return "low (T3=1.5)";
    }
    return "?";
}

// --- save file: one integer world ---

inline bool writeSave(const char* path, int world) {
    FILE* fp = std::fopen(path, "w");
    if (!fp) return false;
    std::fprintf(fp, "%d", world);
    std::fclose(fp);
    return true;
}

inline bool readSave(const char* path, int& world) {
    FILE* fp = std::fopen(path, "r");
    if (!fp) return false;
    int flag = 0;
    if (std::fscanf(fp, "%d", &flag) != 1) {
        std::fclose(fp);
        return false;
    }
    std::fclose(fp);
    if (flag <= 0 || flag >= 4) return false;
    world = flag;
    return true;
}

// --- tile / actor tables ---

struct Tile {
    int x;
    int y;
    int id;
    int xAmount;
    int yAmount;
};

struct Actor {
    int x;
    int y;
    int turn; // 1 right, -1 left, 0 unused
};

constexpr Tile kWorld1Tiles[] = {
    {0, 9, 1, 15, 1},   {18, 8, 1, 9, 1},  {27, 9, 5, 37, 1},
    {67, 9, 6, 4, 1},   {74, 9, 5, 1, 1},  {78, 9, 1, 28, 1},
    {0, 10, 3, 15, 2},  {18, 9, 3, 9, 3},  {27, 10, 4, 37, 2},
    {67, 10, 3, 4, 2},  {74, 10, 4, 1, 2}, {78, 10, 3, 28, 2},
    {10, 6, 2, 4, 1},   {40, 4, 2, 3, 1},  {62, 6, 2, 1, 1},
    {65, 4, 2, 5, 1},
    {36, 7, 10, 1, 1},  {45, 7, 10, 1, 1}, {80, 7, 10, 1, 1},
    {90, 7, 10, 1, 1},
    {19, 6, 11, 1, 1},  {23, 6, 11, 1, 1}, {32, 7, 11, 1, 1},
    {49, 7, 11, 1, 1},  {53, 7, 11, 1, 1}, {57, 7, 11, 1, 1},
    {93, 7, 11, 1, 1},
    {101, 7, 12, 1, 1},
    {15, 10, 13, 1, 1}, {64, 10, 13, 1, 1}, {71, 10, 13, 1, 1},
    {75, 10, 13, 1, 1},
};

constexpr Tile kWorld2Tiles[] = {
    {18, 9, 2, 4, 1},   {23, 6, 2, 6, 1},  {25, 3, 2, 3, 1},
    {31, 10, 2, 2, 1},  {39, 7, 2, 1, 1},  {39, 4, 2, 1, 1},
    {96, 7, 2, 2, 1},   {99, 5, 2, 2, 1},  {102, 3, 2, 12, 1},
    {0, 10, 5, 15, 1},  {38, 10, 5, 10, 1},{50, 10, 5, 1, 1},
    {53, 10, 5, 1, 1},  {56, 10, 5, 1, 1}, {61, 10, 5, 10, 1},
    {75, 10, 5, 1, 1},  {82, 10, 5, 11, 1},
    {0, 11, 4, 15, 2},  {38, 11, 4, 10, 2},{50, 11, 4, 1, 2},
    {53, 11, 4, 1, 2},  {56, 11, 4, 1, 2}, {61, 11, 4, 10, 2},
    {75, 11, 4, 1, 2},  {82, 11, 4, 11, 2},
    {67, 9, 2, 1, 1},   {68, 8, 2, 1, 2},  {69, 7, 2, 1, 3},
    {70, 6, 2, 1, 4},   {75, 6, 2, 1, 4},
    {42, 8, 11, 1, 1},
    {111, 1, 12, 1, 1},
    {4, 8, 14, 1, 1},   {9, 8, 14, 1, 1},  {83, 8, 14, 1, 1},
    {89, 8, 14, 1, 1},
};

constexpr Actor kWorld1Enemies[] = {
    {3, 8, 1}, {18, 7, -1}, {25, 7, 1}, {28, 8, -1}, {33, 8, -1},
    {39, 8, 1}, {68, 3, 1}, {66, 8, -1}, {81, 6, 1}, {92, 6, 1},
};

constexpr Actor kWorld2Enemies[] = {
    {18, 8, 1}, {24, 5, 1}, {28, 5, -1}, {44, 9, 1}, {97, 6, 1}, {99, 4, 1},
};

constexpr Actor kWorld3Enemies[] = {
    {24, 4, -1}, {34, 6, 1}, {37, 3, -1}, {43, 5, -1},
    {63, 7, 1}, {67, 8, -1}, {86, 5, 1},
};

constexpr Point kWorld1Coins[] = {
    {10, 5}, {11, 5}, {12, 5}, {13, 5}, {66, 3}, {67, 3}, {68, 3}, {69, 3},
    {74, 5}, {74, 6}, {74, 7}, {74, 8}, {82, 7}, {83, 8}, {84, 7}, {85, 8},
    {86, 7}, {87, 8}, {88, 7}, {89, 8},
};

constexpr Point kWorld2Coins[] = {
    {25, 2}, {26, 2}, {27, 2}, {32, 4}, {32, 5}, {32, 6}, {32, 7}, {32, 8},
    {50, 6}, {53, 6}, {56, 6},
};

inline const Tile* tilesFor(int world, std::size_t& count) {
    if (world == 1) {
        count = sizeof(kWorld1Tiles) / sizeof(kWorld1Tiles[0]);
        return kWorld1Tiles;
    }
    if (world == 2) {
        count = sizeof(kWorld2Tiles) / sizeof(kWorld2Tiles[0]);
        return kWorld2Tiles;
    }
    count = 0;
    return nullptr;
}

inline const Actor* enemiesFor(int world, std::size_t& count) {
    if (world == 1) {
        count = sizeof(kWorld1Enemies) / sizeof(kWorld1Enemies[0]);
        return kWorld1Enemies;
    }
    if (world == 2) {
        count = sizeof(kWorld2Enemies) / sizeof(kWorld2Enemies[0]);
        return kWorld2Enemies;
    }
    if (world == 3) {
        count = sizeof(kWorld3Enemies) / sizeof(kWorld3Enemies[0]);
        return kWorld3Enemies;
    }
    count = 0;
    return nullptr;
}

inline const Point* coinsFor(int world, std::size_t& count) {
    if (world == 1) {
        count = sizeof(kWorld1Coins) / sizeof(kWorld1Coins[0]);
        return kWorld1Coins;
    }
    if (world == 2) {
        count = sizeof(kWorld2Coins) / sizeof(kWorld2Coins[0]);
        return kWorld2Coins;
    }
    count = 0;
    return nullptr;
}

inline int loadedTileCount(int world) {
    std::size_t n = 0;
    tilesFor(world, n);
    return static_cast<int>(n > kMapNumber ? kMapNumber : n);
}

inline int endingDistance(int world) {
    if (world == 2) return 104 * kWidth;
    return 94 * kWidth;
}

inline bool collides(int id) { return id >= 1 && id <= 10; }

inline const char* tileGlyph(int id) {
    switch (id) {
        case 1: return "#";   // grass
        case 2: return "~";   // cloud
        case 3: return "=";   // dirt
        case 4: return "=";   // snow under
        case 5: return "*";   // snow
        case 6: return "H";   // grippy
        case 7: return "|";   // pipe shaft
        case 8: return "v";   // down mouth
        case 10: return "^";  // up mouth
        case 11: return ",";  // grass deco
        case 12: return "F";  // flag
        case 13: return "~";  // water
        case 14: return "T";  // tree
        default: return "?";
    }
}

// --- tiny game-loop model from main.cpp ---

enum class LoopEvent { None, Died, GameOver, Passed, AllClear, Restart, Home };

struct LoopState {
    int life = kLife;
    int world = 1;
    int score = 0;
    bool inMenu = false;
};

inline LoopEvent applyDeath(LoopState& s) {
    s.life -= 1;
    if (s.life == 0) {
        s.life = kLife;
        s.world = 1;
        s.score = 0;
        s.inMenu = true;
        return LoopEvent::GameOver;
    }
    return LoopEvent::Died;
}

inline LoopEvent applyPass(LoopState& s) {
    if (s.world == 3) {
        s.life = kLife;
        s.world = 1;
        s.score = 0;
        s.inMenu = true;
        return LoopEvent::AllClear;
    }
    s.world += 1;
    return LoopEvent::Passed;
}

inline LoopEvent applyKey(LoopState& s, int key) {
    if (key == kVirRestart) return LoopEvent::Restart;
    if (key == kVirHome) {
        s.life = kLife;
        s.world = 1;
        s.score = 0;
        s.inMenu = true;
        return LoopEvent::Home;
    }
    return LoopEvent::None;
}

inline const char* eventName(LoopEvent e) {
    switch (e) {
        case LoopEvent::None: return "none";
        case LoopEvent::Died: return "died";
        case LoopEvent::GameOver: return "game_over";
        case LoopEvent::Passed: return "passed";
        case LoopEvent::AllClear: return "all_clear";
        case LoopEvent::Restart: return "restart";
        case LoopEvent::Home: return "home";
    }
    return "?";
}

// --- tiny self-test helper ---

inline int fail(const char* file, int line, const char* msg) {
    std::fprintf(stderr, "FAIL %s:%d %s\n", file, line, msg);
    return 1;
}

}  // namespace maoliao

#define MLA_CHECK(cond)                                                         \
    do {                                                                        \
        if (!(cond)) return maoliao::fail(__FILE__, __LINE__, #cond);           \
    } while (0)

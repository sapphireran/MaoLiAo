// Portable extract of MaoLiAo rules. No EasyX, no Win32.
// Numbers match MaoLiAo/define.h, scene.h, and role.h as of the 2020 V2.0 tree.
#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace maoliao {

// --- screen / tiles ---
constexpr int kXSize = 512;
constexpr int kYSize = 384;
constexpr int kWidth = 32;
constexpr int kHeight = 32;
constexpr int kX0 = 2 * kWidth;
constexpr int kY0 = 3 * kHeight;

// --- timing ---
constexpr double kTime = 0.01;
constexpr int kStep = 10;
constexpr int kEnemyStep = 1;
constexpr int kMapBg = 5;
constexpr int kLife = 5;

// --- camera ---
constexpr int kXLeft = 0;
constexpr int kXRight = kWidth * 6;

// --- physics ---
constexpr double kRealHeight = 3.5;
constexpr double kUnrealHeight = 3 * kHeight + 5;
constexpr double kGravity = 30.0;
constexpr double kVMax = 8.0;
constexpr double kARole = 20.0;
constexpr double kT1 = 0.5;
constexpr double kT2 = 1.2;
constexpr double kT3 = 1.5;

// --- command bits ---
constexpr int kCmdLeft = 1;
constexpr int kCmdRight = 2;
constexpr int kCmdUp = 4;
constexpr int kCmdDown = 8;
constexpr int kCmdShoot = 16;
constexpr int kCmdEsc = 32;
constexpr int kVirReturn = 64;
constexpr int kVirRestart = 128;
constexpr int kVirHome = 256;

// --- combat / score ---
constexpr int kBulletStep = 4;
constexpr double kBulletInterval = 0.2;
constexpr int kBulletMaxDistance = 480;
constexpr int kScoreCoin = 10;
constexpr int kScoreEnemy = 5;

// --- capacities (same names as the headers) ---
constexpr int kMapNumber = 30;
constexpr int kCoinsNumber = 70;
constexpr int kFoodNumber = 5;
constexpr int kEnemyTote = 30;

enum class TileId : int {
    Empty = 0,
    Grass = 1,
    Cloud = 2,
    GrassSub = 3,
    SnowSub = 4,
    Snow = 5,
    PipeMid = 6,
    PipeShaft = 7,
    PipeMouthDown = 8,
    PipeMouthUpAlt = 9,
    PipeMouthUp = 10,
    SceneryGrass = 11,
    Victory = 12,
    Water = 13,
    Tree = 14,
};

struct Vec2i {
    int x = 0;
    int y = 0;
};

struct Rect {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
};

struct MapRun {
    int x = 0;
    int y = 0;
    int id = 0;
    int xAmount = 0;
    int yAmount = 0;
    double u = 0.0;
};

struct Hero {
    int x = kX0;
    int y = kX0;  // the shipped constructor uses X0 for both axes
    double xx = kX0;
    double yy = kX0;
    double x0 = 0.0;
    double vX = 0.0;
    double vY = 0.0;
    bool isFly = true;
    bool isShoot = false;
    bool died = false;
    bool ending = false;
    bool passed = false;
};

// s = vt + ½at², then v += at. Matches Inertia::move.
double integrate(double& v, double t, double a);

// Jump launch used by Role::action.
double jumpVelocity();

// Pixel scale from the fake-meter space.
constexpr double pixelScale() { return kUnrealHeight / kRealHeight; }

// Friction coefficient stored on Map::u.
double frictionU(int tileId, int world);

// Sign-ish k from Role::action ("+3-3调节惯性").
int frictionSignK(double vX);

// Four-corner test: any of p1[4] inside [p2[0], p2[1]].
bool cornersHit(const Vec2i p1[4], const Vec2i p2[2]);

// Build the shrunk 32×32 corners in world space (Role::hitMap).
void heroWorldCorners(int screenX, int screenY, int x0, Vec2i out[4]);

// Tile AABB in world pixels. Pipe mouths 8 and 10 are 2× cells.
Rect tileWorldRect(const MapRun& m);

bool isSolidTileId(int id);
bool isSceneryTileId(int id);

// World-3 contact rule: any solid that is not a cloud kills unless armed.
bool world3ContactKills(int tileId, bool isShoot);

// Camera: pin screen x to [0, 192], spilling leftover into x0.
void pinCamera(Hero& h, double rawScreenX);

// Parallax step used when the hero is pinned and moving right.
double parallaxStep(double absVX);

// Stage-end distance in world pixels.
int endingDistance(int world);
bool isEnding(int world, int worldX);

// Command-bit helpers (GetCommand).
int packCommand(bool left, bool right, bool up, bool down, bool shoot, bool esc);
bool hasCommand(int bits, int mask);

// Save file: one integer world index, accepted iff 1..3.
enum class SaveStatus { Ok, Missing, OutOfRange, IoError };

SaveStatus writeWorldSave(const std::string& path, int world);
SaveStatus readWorldSave(const std::string& path, int& worldOut);

// World-3 pipe pair at column x with height in 1..6 (random(1,7) range).
struct PipePair {
    MapRun topMouth;
    MapRun topShaft;
    MapRun bottomMouth;
    MapRun bottomShaft;
};

PipePair makePipePair(int tileX, int height);

// Seven pairs + landing cloud + victory sign, same slot budget as createMap(3).
std::vector<MapRun> buildWorld3(const int heights[7]);

// Score helper used by the examples and tests.
int applyPickupScore(int score, bool coin, bool stompedEnemy, bool shotEnemy);

}  // namespace maoliao

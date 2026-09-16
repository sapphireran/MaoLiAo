#include "maoliao_core.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace maoliao {

double integrate(double& v, double t, double a) {
    const double x = v * t + 0.5 * a * t * t;
    v = v + a * t;
    return x;
}

double jumpVelocity() {
    return -std::sqrt(2.0 * kGravity * kRealHeight);
}

double frictionU(int tileId, int world) {
    // Mirrors Scene::createMap switches.
    if (world == 3) {
        switch (tileId) {
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
                return (kVMax / kT1) / kGravity;
            case 2:
                return (kVMax / kT2) / kGravity;
            default:
                return (kVMax / kT3) / kGravity;
        }
    }
    switch (tileId) {
        case 1:
        case 3:
        case 4:
        case 5:
            return (kVMax / kT2) / kGravity;
        case 6:
            return (kVMax / kT1) / kGravity;
        case 2:
            return (kVMax / kT2) / kGravity;
        default:
            return (kVMax / kT3) / kGravity;
    }
}

int frictionSignK(double vX) {
    if (vX == 0.0) {
        return 0;
    }
    const double xAbs = (vX < 0.0) ? -vX : vX;
    if (vX < 0.0) {
        return static_cast<int>(xAbs / vX + 3.0);  // -1 + 3 = 2
    }
    return static_cast<int>(xAbs / vX - 3.0);  // 1 - 3 = -2
}

bool cornersHit(const Vec2i p1[4], const Vec2i p2[2]) {
    for (int i = 0; i < 4; ++i) {
        if (p1[i].x >= p2[0].x && p1[i].y >= p2[0].y &&
            p1[i].x <= p2[1].x && p1[i].y <= p2[1].y) {
            return true;
        }
    }
    return false;
}

void heroWorldCorners(int screenX, int screenY, int x0, Vec2i out[4]) {
    out[0] = {-x0 + screenX + 1, screenY + 1};
    out[1] = {-x0 + screenX + kWidth - 1, screenY + 1};
    out[2] = {-x0 + screenX + 1, screenY + kHeight - 1};
    out[3] = {-x0 + screenX + kWidth - 1, screenY + kHeight - 1};
}

Rect tileWorldRect(const MapRun& m) {
    Rect r;
    r.left = m.x * kWidth;
    r.top = m.y * kHeight;
    const bool fatPipe = (m.id == 8 || m.id == 10);
    const int cellW = fatPipe ? 2 * kWidth : kWidth;
    const int cellH = fatPipe ? 2 * kHeight : kHeight;
    r.right = r.left + m.xAmount * cellW;
    r.bottom = r.top + m.yAmount * cellH;
    return r;
}

bool isSolidTileId(int id) { return id > 0 && id < 11; }

bool isSceneryTileId(int id) { return id >= 11 && id <= 14; }

bool world3ContactKills(int tileId, bool isShoot) {
    if (isShoot) {
        return false;
    }
    if (!isSolidTileId(tileId)) {
        return false;
    }
    return tileId != static_cast<int>(TileId::Cloud);
}

void pinCamera(Hero& h, double rawScreenX) {
    if (rawScreenX < kXLeft) {
        h.x = kXLeft;
        h.xx = kXLeft;
        h.vX = 0.0;
        return;
    }
    if (rawScreenX > kXRight && !h.ending) {
        h.x0 -= (rawScreenX - kXRight);
        h.x = kXRight;
        h.xx = kXRight;
        return;
    }
    h.x = static_cast<int>(rawScreenX);
    h.xx = rawScreenX;
}

double parallaxStep(double absVX) {
    return absVX * kTime * pixelScale() / static_cast<double>(kMapBg);
}

int endingDistance(int world) {
    switch (world) {
        case 1:
            return 94 * kWidth;
        case 2:
            return 104 * kWidth;
        case 3:
            return 94 * kWidth;
        default:
            return 0;
    }
}

bool isEnding(int world, int worldX) { return worldX > endingDistance(world); }

int packCommand(bool left, bool right, bool up, bool down, bool shoot, bool esc) {
    int c = 0;
    if (left) {
        c |= kCmdLeft;
    }
    if (right) {
        c |= kCmdRight;
    }
    if (up) {
        c |= kCmdUp;
    }
    if (down) {
        c |= kCmdDown;
    }
    if (shoot) {
        c |= kCmdShoot;
    }
    if (esc) {
        c |= kCmdEsc;
    }
    return c;
}

bool hasCommand(int bits, int mask) { return (bits & mask) != 0; }

SaveStatus writeWorldSave(const std::string& path, int world) {
    if (world < 1 || world > 3) {
        return SaveStatus::OutOfRange;
    }
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return SaveStatus::IoError;
    }
    out << world;
    return out ? SaveStatus::Ok : SaveStatus::IoError;
}

SaveStatus readWorldSave(const std::string& path, int& worldOut) {
    std::ifstream in(path);
    if (!in) {
        return SaveStatus::Missing;
    }
    std::string raw;
    if (!(in >> raw)) {
        return SaveStatus::Missing;
    }
    std::istringstream iss(raw);
    int flag = 0;
    if (!(iss >> flag)) {
        return SaveStatus::Missing;
    }
    if (flag < 1 || flag > 3) {
        return SaveStatus::OutOfRange;
    }
    worldOut = flag;
    return SaveStatus::Ok;
}

PipePair makePipePair(int tileX, int height) {
    if (height < 1) {
        height = 1;
    }
    if (height > 6) {
        height = 6;
    }
    PipePair p;
    p.topMouth = {tileX, height - 2, 8, 1, 1, frictionU(8, 3)};
    p.topShaft = {tileX, 0, 7, 1, std::max(0, height - 2), frictionU(7, 3)};
    p.bottomMouth = {tileX, 4 + height, 10, 1, 1, frictionU(10, 3)};
    p.bottomShaft = {tileX, height + 6, 7, 1, std::max(0, 6 - height), frictionU(7, 3)};
    return p;
}

std::vector<MapRun> buildWorld3(const int heights[7]) {
    std::vector<MapRun> runs;
    runs.reserve(kMapNumber);
    for (int i = 0; i < 7; ++i) {
        const int x = i * 10 + 10;
        const PipePair p = makePipePair(x, heights[i]);
        runs.push_back(p.topMouth);
        runs.push_back(p.topShaft);
        runs.push_back(p.bottomMouth);
        runs.push_back(p.bottomShaft);
    }
    runs.push_back({80, 6, 2, 25, 1, frictionU(2, 3)});
    runs.push_back({101, 4, 12, 1, 1, frictionU(12, 3)});
    return runs;
}

int applyPickupScore(int score, bool coin, bool stompedEnemy, bool shotEnemy) {
    if (coin) {
        score += kScoreCoin;
    }
    if (stompedEnemy) {
        score += kScoreEnemy;
    }
    if (shotEnemy) {
        score += kScoreEnemy;
    }
    return score;
}

}  // namespace maoliao

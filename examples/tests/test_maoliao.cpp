#include "maoliao/aabb.h"
#include "maoliao/camera.h"
#include "maoliao/commands.h"
#include "maoliao/constants.h"
#include "maoliao/hero.h"
#include "maoliao/inertia.h"
#include "maoliao/levels.h"
#include "maoliao/render.h"
#include "maoliao/savefile.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>

namespace {

int g_failed = 0;
int g_passed = 0;

void expect(bool cond, const char* msg) {
    if (cond) {
        ++g_passed;
        return;
    }
    ++g_failed;
    std::cerr << "FAIL: " << msg << "\n";
}

void expect_near(double a, double b, double eps, const char* msg) {
    expect(std::fabs(a - b) <= eps, msg);
}

}  // namespace

int main() {
    using namespace maoliao;

    // --- constants ---
    expect(kXSize == 512 && kYSize == 384, "window 512x384");
    expect(kWidth == 32 && kHeight == 32, "tile 32");
    expect(kXRight == 192, "XRIGHT = 6 tiles");
    expect(kUnrealHeight == 101, "UNREAL_HEIGHT");
    expect(kLife == 5, "LIFE");
    expect_near(kPxPerMetre, 101.0 / 3.5, 1e-12, "px/m");
    expect_near(jump_speed(), -std::sqrt(210.0), 1e-9, "jump vY");
    expect_near(friction_u(kT1), (8.0 / 0.5) / 30.0, 1e-12, "u T1");
    expect_near(friction_u(kT2), (8.0 / 1.2) / 30.0, 1e-12, "u T2");
    expect_near(friction_u(kT3), (8.0 / 1.5) / 30.0, 1e-12, "u T3");

    // --- inertia ---
    {
        double v = 0;
        const double dx = inertia_move(v, 0.01, 20.0);
        expect_near(dx, 0.001, 1e-12, "suvat from rest");
        expect_near(v, 0.2, 1e-12, "v after one step");
        double vpx = 0;
        const double px = inertia_move_pixels(vpx, 0.01, 20.0);
        expect_near(px, 0.001 * kPxPerMetre, 1e-12, "pixel scale");
    }

    // --- jump apex ---
    {
        HeroState h;
        h.y = 300;
        h.airborne = false;
        HeroInput in;
        in.jump = true;
        const double start = h.y;
        step_hero_free(h, in, friction_u(kT2));
        in.jump = false;
        double min_y = h.y;
        for (int i = 0; i < 80; ++i) {
            step_hero_free(h, in, friction_u(kT2));
            if (h.y < min_y) {
                min_y = h.y;
            }
        }
        expect_near(start - min_y, kUnrealHeight, 0.5, "apex near 101 px");
    }

    // --- friction stop on ice ---
    {
        HeroState h;
        h.airborne = false;
        h.v_x = kVMax;
        HeroInput in;
        int n = 0;
        while (std::fabs(h.v_x) > 1e-9 && n < 200) {
            step_hero_free(h, in, friction_u(kT1));
            ++n;
        }
        expect(n > 0 && n < 50, "ice stops quickly");
        expect_near(h.v_x, 0, 1e-9, "stopped");
    }

    // --- AABB ---
    {
        const SpriteBox hero{64, 96, 32, 32};
        Vec2 c[4];
        inset_corners(hero, c);
        expect(c[0].x == 65 && c[3].x == 95, "inset corners");
        const Aabb ground = tile_aabb(2, 4, 3, 1, 1);
        // tile (2,4) is px (64,128) — below the hero at y=96
        expect(!vertex_in_aabb(c, ground), "hero above ground, no hit");
        const Aabb overlap = tile_aabb(2, 3, 1, 1, 1);
        expect(vertex_in_aabb(c, overlap), "hero on own tile");
        const Aabb pipe = tile_aabb(0, 0, 1, 1, 10);
        expect(pipe.max_x == 64 && pipe.max_y == 64, "pipe mouth 64x64");
        const Aabb interior{hero.x + 8, hero.y + 8, hero.x + 24, hero.y + 24};
        expect(!vertex_in_aabb(c, interior), "vertex test misses interior box");
        expect(aabb_overlap(sprite_aabb(hero), interior), "full AABB hits interior");
    }

    // --- commands ---
    {
        const int m = pack_commands(true, false, true, false, true, false);
        expect(m == (kCmdLeft | kCmdUp | kCmdShoot), "pack bits");
        bool l, r, u, d, s, e;
        unpack_commands(m, l, r, u, d, s, e);
        expect(l && !r && u && !d && s && !e, "unpack bits");
        expect(describe_commands(0) == "NONE", "empty describe");
        expect(describe_commands(kCmdRight | kCmdUp) == "RIGHT|UP", "describe");
        expect(sticky_update(2, 0, false) == 2, "sticky keeps");
        expect(sticky_update(2, 0, true) == 0, "sticky refreshes");
    }

    // --- save ---
    {
        const std::string path = "data/_test_save.dat";
        std::remove(path.c_str());
        expect(write_save(path, 2) == SaveError::ok, "write 2");
        const SaveResult r = read_save(path);
        expect(r.error == SaveError::ok && r.world == 2, "read 2");
        expect(write_save(path, 0) == SaveError::out_of_range, "write 0");
        expect(read_save("data/_missing.dat").error == SaveError::missing_file,
               "missing");
        {
            FILE* fp = std::fopen(path.c_str(), "w");
            expect(fp != nullptr, "open overwrite");
            if (fp) {
                std::fputs("4", fp);
                std::fclose(fp);
            }
        }
        expect(read_save(path).error == SaveError::out_of_range, "read 4");
        std::remove(path.c_str());

        const SaveResult shipped = read_save("../MaoLiAo/gameRecord.dat");
        if (shipped.error == SaveError::ok) {
            expect(shipped.world >= 1 && shipped.world <= 3, "shipped save in range");
        } else {
            expect(true, "shipped save optional");
        }
    }

    // --- levels ---
    {
        const Level w1 = authored_level(1);
        const Level w2 = authored_level(2);
        expect(w1.tiles.size() == 32, "world1 authored 32 tiles");
        expect(w2.tiles.size() == 36, "world2 authored 36 tiles");
        expect(loaded_level(1).tiles.size() == 30, "world1 clipped to 30");
        expect(loaded_level(2).tiles.size() == 30, "world2 clipped to 30");
        expect(w1.coins.size() == 20, "world1 coins");
        expect(w1.enemies.size() == 10, "world1 enemies");
        expect(w2.coins.size() == 11, "world2 coins");
        expect(w2.enemies.size() == 6, "world2 enemies");
        expect(ending_distance_px(1) == 94 * 32, "end w1");
        expect(ending_distance_px(2) == 104 * 32, "end w2");
        expect(ending_distance_px(3) == 94 * 32, "end w3");
        expect(w1.tiles.back().id == 13, "last authored w1 is water");
        expect(loaded_level(1).tiles.back().id == 13, "loaded w1 last kept water@64");
        expect(w2.tiles.back().id == 14, "last authored w2 is tree");
        expect(loaded_level(2).tiles.back().id != 14, "tree dropped by cap");

        const Level w3 = authored_world3(7);
        expect(w3.tiles.size() == 30, "world3 7*4+2 tiles");
        expect(w3.coins.size() == 7, "world3 coins");
        expect(w3.food[0].pixel_x == 10 && w3.food[0].pixel_y == 10, "w3 food");
        expect(w3.enemies.size() == 7, "world3 enemies");

        expect_near(friction_for_id(1, 6), friction_u(kT1), 1e-12, "id6 ice");
        expect_near(friction_for_id(1, 1), friction_u(kT2), 1e-12, "id1 grass");
        expect_near(friction_for_id(3, 2), friction_u(kT2), 1e-12, "w3 cloud T2");
        expect_near(friction_for_id(3, 8), friction_u(kT3), 1e-12, "w3 pipe T3");
    }

    // --- render ---
    {
        const Level w1 = authored_level(1);
        const std::string ascii = render_ascii_joined(w1, 0, 20, 8, 10);
        expect(ascii.find('#') != std::string::npos, "ground drawn");
        expect(ascii.find('e') != std::string::npos, "enemy drawn");
    }

    // --- camera ---
    {
        CameraState cam;
        cam.screen_x = 250;
        cam.origin_x = 0;
        lock_camera(cam, false);
        expect_near(cam.screen_x, 192, 1e-12, "lock screen");
        expect_near(cam.origin_x, -58, 1e-12, "lock origin");
        CameraState end = cam;
        end.screen_x = 250;
        lock_camera(end, true);
        expect_near(end.screen_x, 250, 1e-12, "ending no lock");
    }

    std::cout << "passed " << g_passed << "  failed " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}

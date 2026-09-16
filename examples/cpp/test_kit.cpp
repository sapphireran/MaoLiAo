#include "maoliao_kit.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

static int g_failed = 0;

static void expect(bool ok, const char* msg) {
    if (!ok) {
        std::cerr << "FAIL " << msg << "\n";
        ++g_failed;
    }
}

int main(int argc, char** argv) {
    using namespace maoliao;

    expect(XRIGHT == 192, "XRIGHT");
    expect(UNREAL_HEIGHT == 101, "UNREAL_HEIGHT");
    expect(std::abs(pixel_scale() - (101.0 / 3.5)) < 1e-12, "scale");
    expect(std::abs(launch_vy() + std::sqrt(210.0)) < 1e-9, "launch");
    expect(std::abs(friction_u(T2) - (8.0 / 1.2) / 30.0) < 1e-12, "u T2");

    const auto samples = jump_profile();
    const auto top = apex(samples);
    expect(top.frame == 48, "apex frame");
    expect(std::abs(top.rise_px - 101.0) < 0.01, "apex ~101");
    expect(samples.size() >= 96 && samples.size() <= 98, "hang ~97");

    expect((command_from_keys("AD") & CMD_LEFT) && (command_from_keys("AD") & CMD_RIGHT),
           "bitset OR");
    expect(command_from_keys("WK") == CMD_UP, "W and K alias");

    expect(is_ending(3008, 1) == false, "ending exclusive 94 tiles");
    expect(is_ending(3009, 1) == true, "ending 94*32+1");
    expect(is_ending(3328, 2) == false, "world2 tripwire");
    expect(is_ending(3329, 2) == true, "world2 past");

    double x = 200, x0 = 0;
    apply_rail(x, x0, false);
    expect(x == XRIGHT, "rail pin");
    expect(x0 == -8, "rail x0");

    MsvcRand rng(1);
    expect(rng.rand() == 41, "msvc first rand");
    MsvcRand s1(1);
    int c0 = s1.random_ab(3, 7);
    expect(c0 == 4, "seed1 first coin height");

    if (argc >= 2) {
        const std::string root = argv[1];
        auto ok = read_record(root + "/catalog/saves/world3.dat");
        expect(ok.status == "ok" && ok.world == 3, "read world3.dat");
        auto bad = read_record(root + "/catalog/saves/invalid-four.dat");
        expect(bad.status == "invalid" && bad.world == 4, "invalid 4");
        auto junk = read_record(root + "/catalog/saves/garbage.dat");
        expect(junk.status == "garbage", "garbage");
    }

    if (g_failed) {
        std::cerr << g_failed << " failed\n";
        return 1;
    }
    std::cout << "cpp kit ok\n";
    return 0;
}

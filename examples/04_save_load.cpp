// 04_save_load — gameRecord.dat as a single world integer.
//
// The EasyX loader does not check fopen. This portable pair rejects
// missing files, out-of-range worlds, and empty reads so the format
// is documented and the happy path stays compatible with the game.

#include "maoliao_core.hpp"

#include <cstdio>
#include <cstring>

namespace {

int g_failed = 0;

void expect(bool ok, const char* name)
{
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) {
        ++g_failed;
    }
}

void expect_eq(int got, int want, const char* name)
{
    const bool ok = got == want;
    std::printf("  [%s] %s  got=%d want=%d\n", ok ? "PASS" : "FAIL", name, got,
                want);
    if (!ok) {
        ++g_failed;
    }
}

}  // namespace

int main()
{
    std::printf("04_save_load\n");

    expect(maoliao::world_is_valid(1), "world 1 ok");
    expect(maoliao::world_is_valid(2), "world 2 ok");
    expect(maoliao::world_is_valid(3), "world 3 ok");
    expect(!maoliao::world_is_valid(0), "world 0 rejected");
    expect(!maoliao::world_is_valid(4), "world 4 rejected (title MessageBox)");
    expect(!maoliao::world_is_valid(-1), "negative rejected");

    const char* path = "bin/gameRecord.test.dat";

    expect(!maoliao::write_save(path, 0), "refuse to write invalid world");
    expect(maoliao::write_save(path, 2), "write world 2");

    int world = 0;
    expect(maoliao::read_save(path, world), "read back");
    expect_eq(world, 2, "round-trip world 2");

    expect(maoliao::write_save(path, 3), "overwrite with world 3");
    expect(maoliao::read_save(path, world), "read world 3");
    expect_eq(world, 3, "round-trip world 3");

    // Compatibility: the game writes "%d" with no newline. Read a
    // hand-authored file the same way.
    {
        FILE* fp = std::fopen(path, "w");
        if (fp) {
            std::fprintf(fp, "1");
            std::fclose(fp);
        }
        world = 0;
        expect(maoliao::read_save(path, world), "bare '1' is valid");
        expect_eq(world, 1, "bare '1' loads as world 1");
    }

    expect(!maoliao::read_save("bin/does-not-exist.dat", world),
           "missing file fails closed");
    expect(!maoliao::read_save(0, world), "null path fails");

    {
        FILE* fp = std::fopen(path, "w");
        if (fp) {
            std::fprintf(fp, "not-a-number");
            std::fclose(fp);
        }
        expect(!maoliao::read_save(path, world), "garbage payload fails");
    }

    if (g_failed != 0) {
        std::printf("04_save_load: %d failed\n", g_failed);
        return 1;
    }
    std::printf("04_save_load: all passed\n");
    return 0;
}

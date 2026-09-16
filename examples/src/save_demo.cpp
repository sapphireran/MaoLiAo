// gameRecord.dat is a single integer world id in [1, 3].

#include "maoliao_model.h"

#include <cstdio>

using namespace maoliao;

int main(int argc, char** argv) {
    const char* path = (argc > 1) ? argv[1] : "testdata/gameRecord.roundtrip.dat";

    MLA_CHECK(writeSave(path, 2));
    int world = 0;
    MLA_CHECK(readSave(path, world));
    MLA_CHECK(world == 2);

    MLA_CHECK(writeSave(path, 3));
    MLA_CHECK(readSave(path, world));
    MLA_CHECK(world == 3);

    MLA_CHECK(writeSave(path, 1));
    MLA_CHECK(readSave(path, world));
    MLA_CHECK(world == 1);

    // Out of range matches Control::gameStart's "missing save" branch.
    MLA_CHECK(writeSave(path, 4));
    MLA_CHECK(!readSave(path, world));
    MLA_CHECK(writeSave(path, 0));
    MLA_CHECK(!readSave(path, world));
    MLA_CHECK(writeSave(path, -1));
    MLA_CHECK(!readSave(path, world));

    MLA_CHECK(!readSave("testdata/does_not_exist.dat", world));

    // Sample checked into the repo (copy of MaoLiAo/gameRecord.dat).
    int sample = 0;
    MLA_CHECK(readSave("testdata/gameRecord.sample.dat", sample));
    MLA_CHECK(sample == 1);

    std::printf("save_demo ok  sample_world=%d path=%s\n", sample, path);
    return 0;
}

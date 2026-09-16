#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "maoliao/check.hpp"
#include "maoliao/save.hpp"

static std::string testdata(const char* name) {
    return std::string("testdata/") + name;
}

int main() {
    using namespace maoliao;

    int sample = 0;
    MLA_CHECK(read_record(testdata("gameRecord.sample.dat"), sample));
    MLA_CHECK(sample == 1);
    MLA_CHECK(record_ok(sample));

    MLA_CHECK(record_ok(1) && record_ok(2) && record_ok(3));
    MLA_CHECK(!record_ok(0) && !record_ok(4) && !record_ok(-1));

    const char* tmp = "bin/save_roundtrip.dat";
    MLA_CHECK(write_record(tmp, 2));
    int got = 0;
    MLA_CHECK(read_record(tmp, got));
    MLA_CHECK(got == 2);

    // Homework writes "%d" with no newline. Accept a trailing newline too.
    {
        FILE* fp = std::fopen("bin/save_nl.dat", "w");
        MLA_CHECK(fp != nullptr);
        std::fprintf(fp, "3\n");
        std::fclose(fp);
        int w = 0;
        MLA_CHECK(read_record("bin/save_nl.dat", w));
        MLA_CHECK(w == 3);
    }

    std::cout << "sample world   " << sample << "\n";
    std::cout << "roundtrip      " << got << "\n";
    return done("save_lab");
}

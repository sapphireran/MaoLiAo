#include "check.hpp"
#include "maoliao.hpp"

#include <filesystem>
#include <fstream>

int main() {
    using namespace maoliao;

    const auto dir = std::filesystem::temp_directory_path();
    const auto path = dir / "maoliao_test_save.dat";
    const std::string p = path.string();

    CHECK(write_world(p, 1));
    CHECK(read_world(p) == 1);
    CHECK(write_world(p, 3));
    CHECK(read_world(p) == 3);
    CHECK(!write_world(p, 0));
    CHECK(!write_world(p, 4));

    {
        std::ofstream bad(p, std::ios::trunc);
        bad << 9;
    }
    CHECK(!read_world(p).has_value());
    CHECK(!read_world(p + ".nope").has_value());

    std::filesystem::remove(path);
    return report("test_save");
}

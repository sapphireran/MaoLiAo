#include "maoliao.hpp"

#include <filesystem>
#include <iostream>
#include <string>

int main() {
    using namespace maoliao;

    const auto path = std::filesystem::temp_directory_path() / "maoliao_demo_save.dat";
    const std::string p = path.string();

    std::cout << "Writing world 2 to " << p << '\n';
    if (!write_world(p, 2)) {
        std::cerr << "write failed\n";
        return 1;
    }
    const auto loaded = read_world(p);
    std::cout << "read back: " << (loaded ? std::to_string(*loaded) : "nullopt")
              << '\n';

    std::cout << "reject 0: " << write_world(p, 0) << '\n';
    std::cout << "reject 4: " << write_world(p, 4) << '\n';
    std::cout << "missing file: "
              << read_world((path.string() + ".missing")).has_value() << '\n';

    std::filesystem::remove(path);
    return loaded == 2 ? 0 : 1;
}

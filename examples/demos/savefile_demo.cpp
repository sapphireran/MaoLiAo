#include "maoliao/savefile.h"

#include <cstdio>
#include <iostream>
#include <string>

int main() {
    using namespace maoliao;

    const std::string path = "data/_demo_save.dat";
    std::remove(path.c_str());

    std::cout << "Write worlds 1-3, read back:\n";
    for (int w = 1; w <= 3; ++w) {
        const SaveError we = write_save(path, w);
        const SaveResult r = read_save(path);
        std::cout << "  wrote " << w << " (" << save_error_name(we) << ") read "
                  << r.world << " (" << save_error_name(r.error) << ")\n";
    }

    std::cout << "Reject 0 and 4:\n";
    std::cout << "  write 0 -> " << save_error_name(write_save(path, 0)) << "\n";
    std::cout << "  write 4 -> " << save_error_name(write_save(path, 4)) << "\n";

    {
        FILE* fp = std::fopen(path.c_str(), "w");
        if (fp) {
            std::fputs("99", fp);
            std::fclose(fp);
        }
        const SaveResult bad = read_save(path);
        std::cout << "  file '99' -> " << save_error_name(bad.error)
                  << " world=" << bad.world << "\n";
    }

    const SaveResult missing = read_save("data/_no_such_save.dat");
    std::cout << "Missing file -> " << save_error_name(missing.error) << "\n";

    std::remove(path.c_str());
    return 0;
}

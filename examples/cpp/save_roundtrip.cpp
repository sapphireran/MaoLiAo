#include "maoliao_kit.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: save_roundtrip read PATH | write PATH WORLD\n";
        return 2;
    }
    const std::string cmd = argv[1];
    if (cmd == "read") {
        auto r = maoliao::read_record(argv[2]);
        std::cout << r.status;
        if (r.status == "ok" || r.status == "invalid") std::cout << " " << r.world;
        std::cout << "\n";
        return r.status == "ok" ? 0 : 1;
    }
    if (cmd == "write") {
        maoliao::write_record(argv[2], std::stoi(argv[3]));
        return 0;
    }
    return 2;
}

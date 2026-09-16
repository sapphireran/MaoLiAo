#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main() {
    const char* bins[] = {
        "bin/jump_lab",      "bin/friction_lab", "bin/aabb_lab",
        "bin/command_lab",   "bin/save_lab",     "bin/camera_lab",
        "bin/world_atlas",   "bin/enemy_patrol", "bin/pipe_field",
        "bin/score_lab",     "bin/tick_replay",
    };

    int failed = 0;
    for (const char* b : bins) {
        const int rc = std::system(b);
        if (rc != 0) {
            std::cerr << "runner: " << b << " exited " << rc << "\n";
            ++failed;
        }
    }
    if (failed == 0) {
        std::cout << "ALL EXAMPLE DEMOS PASSED\n";
        return 0;
    }
    std::cout << "EXAMPLE DEMOS FAILED: " << failed << "\n";
    return 1;
}

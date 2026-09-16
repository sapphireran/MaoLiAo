// Portable copy of Role::isHit + the inset quad from Role::hitMap.
// Build: g++ -std=c++17 -o hit_test_example examples/snippets/hit_test_example.cpp

#include <iostream>

struct Point {
    int x;
    int y;
};

bool is_hit(const Point hero[4], const Point box[2])
{
    for (int i = 0; i < 4; ++i) {
        if (hero[i].x >= box[0].x && hero[i].y >= box[0].y &&
            hero[i].x <= box[1].x && hero[i].y <= box[1].y)
            return true;
    }
    return false;
}

void inset_quad(int screen_x, int screen_y, int x0, Point out[4])
{
    const int width = 32;
    const int height = 32;
    out[0] = {-x0 + screen_x + 1, screen_y + 1};
    out[1] = {-x0 + screen_x + width - 1, screen_y + 1};
    out[2] = {-x0 + screen_x + 1, screen_y + height - 1};
    out[3] = {-x0 + screen_x + width - 1, screen_y + height - 1};
}

int main()
{
    Point hero[4];
    inset_quad(64, 64, 0, hero);

    Point floor[2] = {{0, 96}, {480, 128}};
    Point overlap[2] = {{60, 60}, {70, 70}};
    Point pipe[2] = {{36 * 32, 7 * 32}, {36 * 32 + 64, 7 * 32 + 64}};  // id 10

    const bool miss_floor = is_hit(hero, floor);
    const bool hit_box = is_hit(hero, overlap);
    const bool miss_pipe = is_hit(hero, pipe);

    std::cout << "hero_tl " << hero[0].x << "," << hero[0].y << "\n";
    std::cout << "hero_br " << hero[3].x << "," << hero[3].y << "\n";
    std::cout << "miss_floor " << (miss_floor ? "yes" : "no") << "\n";
    std::cout << "hit_overlap " << (hit_box ? "yes" : "no") << "\n";
    std::cout << "miss_distant_pipe " << (miss_pipe ? "yes" : "no") << "\n";

    return (miss_floor == false && hit_box == true && miss_pipe == false) ? 0 : 1;
}

#include <iostream>

#include "maoliao/aabb.hpp"
#include "maoliao/check.hpp"
#include "maoliao/worlds.hpp"

int main() {
    using namespace maoliao;

    // A representative field: heights 1..6 cycling, groups x=10,20,...,70.
    const int height[7] = {3, 1, 6, 4, 2, 5, 3};
    const auto field = world3_from_heights(height);

    std::cout << "world3 tiles   " << field.size() << "\n";
    MLA_CHECK(field.size() == 30);

    // First group at x=10, height=3:
    //   mouth8 at y=1, shaft7 from y=0 h=1, mouth10 at y=7, shaft7 at y=9 h=3
    MLA_CHECK(field[0].x == 10 && field[0].id == 8 && field[0].y == 1);
    MLA_CHECK(field[1].id == 7 && field[1].y == 0 && field[1].y_amount == 1);
    MLA_CHECK(field[2].id == 10 && field[2].y == 7);
    MLA_CHECK(field[3].id == 7 && field[3].y == 9 && field[3].y_amount == 3);

    // Last two records are the cloud runway and the flag.
    MLA_CHECK(field[28].id == 2 && field[28].x == 80 && field[28].x_amount == 25);
    MLA_CHECK(field[29].id == 12 && field[29].x == 101);

    // Safe store keeps all 30 (unlike the homework `<= sizeof` write).
    const auto stored = stored_map(field);
    MLA_CHECK(stored.size() == 30);

    // Height 1: upper shaft yAmount = max(0, 1-2) = 0; lower = max(0, 5) = 5.
    const auto g1 = pipe_group(10, 1);
    MLA_CHECK(g1[1].y_amount == 0);
    MLA_CHECK(g1[3].y_amount == 5);

    // Height 6: upper shaft 4, lower 0.
    const auto g6 = pipe_group(20, 6);
    MLA_CHECK(g6[1].y_amount == 4);
    MLA_CHECK(g6[3].y_amount == 0);

    // 2× mouth boxes vs 1× shafts.
    MLA_CHECK(tile_box(field[0]).x1 - tile_box(field[0]).x0 == 64);
    MLA_CHECK(tile_box(field[1]).x1 - tile_box(field[1]).x0 == 32);

    // World-3 u: pipes default T3, cloud T2.
    MLA_NEAR(field[0].u, u_t3(), 1e-12);
    MLA_NEAR(field[28].u, u_t2(), 1e-12);

    const auto enemies = world3_enemies();
    MLA_CHECK(enemies.size() == 7);
    MLA_CHECK(enemies[0].x == 24 && enemies[0].turn == -1);

    return done("pipe_field");
}

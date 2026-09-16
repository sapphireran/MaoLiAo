from __future__ import annotations

import unittest

from examples.maoliao_lab.constants import HEIGHT, MAP_NUMBER, WIDTH
from examples.maoliao_lab.world3 import LAB_PIPE_HEIGHTS, coins_from_heights, column_x, crt_random, pipes_from_heights
from examples.maoliao_lab.worlds import cap_map, food_pixels_world1, food_pixels_world2, load_world, world1, world2, world3


class MapCap(unittest.TestCase):
    def test_world1_drops_two_water_tiles(self) -> None:
        w = world1()
        self.assertEqual(w["authored_tile_count"], 32)
        self.assertEqual(len(w["tiles"]), MAP_NUMBER)
        xs_water = [t["x"] for t in w["tiles"] if t["id"] == 13]
        self.assertEqual(xs_water, [15, 64])

    def test_world2_drops_goal_and_trees(self) -> None:
        w = world2()
        self.assertEqual(w["authored_tile_count"], 36)
        self.assertEqual(len(w["tiles"]), MAP_NUMBER)
        dropped_ids = [t["id"] for t in w["dropped"]]
        self.assertEqual(dropped_ids, [11, 12, 14, 14, 14, 14])
        self.assertFalse(any(t.get("is_goal") for t in w["tiles"]))
        self.assertTrue(any(t["id"] == 12 for t in w["dropped"]))

    def test_world3_fills_exactly_thirty_slots(self) -> None:
        tiles = pipes_from_heights(LAB_PIPE_HEIGHTS)
        self.assertEqual(len(tiles), 30)
        w = world3()
        self.assertEqual(len(w["tiles"]), 30)
        self.assertTrue(any(t.get("is_goal") for t in w["tiles"]))
        self.assertEqual(w["tiles"][-2]["id"], 2)
        self.assertEqual(w["tiles"][-2]["xAmount"], 25)

    def test_cap_map_is_a_prefix(self) -> None:
        rows = [{"i": i} for i in range(40)]
        self.assertEqual(len(cap_map(rows)), 30)
        self.assertEqual(cap_map(rows)[-1], {"i": 29})


class Actors(unittest.TestCase):
    def test_world1_counts(self) -> None:
        w = world1()
        self.assertEqual(len(w["coins"]), 20)
        self.assertEqual(len(w["enemies"]), 10)
        self.assertEqual(w["food_pixels"], [[14 * WIDTH, 5 * HEIGHT]])
        self.assertEqual(food_pixels_world1(), (448, 160))

    def test_world2_food_uses_integer_division(self) -> None:
        self.assertEqual(food_pixels_world2(), (39 * 32 - 10, 96 + 6))
        self.assertEqual(world2()["food_pixels"], [[1238, 102]])

    def test_world3_coins_and_weapon(self) -> None:
        w = world3()
        self.assertEqual(len(w["coins"]), 7)
        self.assertEqual(w["coins"][0], (5, 3))
        self.assertEqual(w["food_pixels"], [[10, 10]])
        self.assertEqual(len(w["enemies"]), 7)

    def test_load_world_dispatch(self) -> None:
        self.assertEqual(load_world(1)["world"], 1)
        self.assertEqual(load_world(2)["world"], 2)
        self.assertEqual(load_world(3)["world"], 3)
        with self.assertRaises(ValueError):
            load_world(4)


class World3Generator(unittest.TestCase):
    def test_column_spacing(self) -> None:
        self.assertEqual([column_x(i) for i in range(7)], [10, 20, 30, 40, 50, 60, 70])

    def test_crt_random_formula(self) -> None:
        # random(1,7) = rand()%6 + 1 → 1..6
        self.assertEqual(crt_random(0, 1, 7), 1)
        self.assertEqual(crt_random(5, 1, 7), 6)
        self.assertEqual(crt_random(6, 1, 7), 1)

    def test_pipe_tile_ids_per_column(self) -> None:
        col = [t for t in pipes_from_heights([4], count=1) if t["x"] == 10]
        self.assertEqual([t["id"] for t in col], [8, 7, 10, 7])
        self.assertEqual(col[1]["yAmount"], max(0, 4 - 2))
        self.assertEqual(col[3]["yAmount"], max(0, 6 - 4))

    def test_coin_xs(self) -> None:
        self.assertEqual([c[0] for c in coins_from_heights([3, 4, 5, 6, 3, 4, 5])], [5, 15, 25, 35, 45, 55, 65])


class FrictionByWorld(unittest.TestCase):
    def test_world1_ice_is_high_friction(self) -> None:
        ice = next(t for t in world1()["tiles"] if t["id"] == 6)
        grass = next(t for t in world1()["tiles"] if t["id"] == 1)
        self.assertGreater(ice["u"], grass["u"])

    def test_world3_ground_ids_use_t1(self) -> None:
        cloud = next(t for t in world3()["tiles"] if t["id"] == 2)
        pipe = next(t for t in world3()["tiles"] if t["id"] == 8)
        # World 3 switch: 1,3,4,5,6 → T1; 2 → T2; else T3.
        # Pipe mouth 8 is else → T3 (U_DEFAULT). Cloud is T2 (U_NORMAL).
        self.assertGreater(cloud["u"], pipe["u"])


if __name__ == "__main__":
    unittest.main()

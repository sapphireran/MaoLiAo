# C++ snippets

| File | Use |
| --- | --- |
| `world4_create_map.cpp` | Paste blocks for the canyon example (`#if 0` so a casual include does nothing) |
| `hit_test_example.cpp` | Compilable copy of `Role::isHit` + the inset quad |

```bash
g++ -std=c++17 -o /tmp/hit_test examples/snippets/hit_test_example.cpp
/tmp/hit_test
```

Regenerate the canyon paste from JSON:

```bash
python3 examples/toolkit/maoliao_levels.py export-cpp \
    examples/levels/example-world-4-canyon.json
```

The Visual Studio project does not compile these files. Follow [docs/adding-a-level.md](../../docs/adding-a-level.md) to wire a fourth world into `scene.cpp` / `role.cpp` / `main.cpp` on Windows.

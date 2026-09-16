# Tools

Dependency-free Python 3.9+ helpers. They never import EasyX or call the Windows exe.

| Script | Purpose |
| --- | --- |
| `validate_fixtures.py` | Assert fixtures match `define.h` / `scene.cpp` / `role.cpp`, rebuild world 3 from the example heights, diff ASCII goldens, classify saves |
| `render_level.py --world N` | Print the runtime silhouette. `--write-expected` refreshes `fixtures/expected/` |
| `extract_source.py` | Dump the live C++ tables as JSON on stdout |
| `maoliao_fixtures.py` | Shared parser / renderer (imported by the three CLIs) |

```bash
python3 tools/validate_fixtures.py
python3 tools/validate_fixtures.py --list-saves
python3 tools/render_level.py --world 1
python3 tools/extract_source.py | python3 -m json.tool | head
```

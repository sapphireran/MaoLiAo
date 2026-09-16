# Golden ASCII dumps

Checked in so `make test` can diff `level_dump` against a known map.

| File | Command |
| --- | --- |
| `world1_stored.txt` | `level_dump --stored 1` |
| `world2_stored.txt` | `level_dump --stored 2` |
| `world2_authored.txt` | `level_dump --authored 2` |
| `world3_sample.txt` | `level_dump 3` |

World 3 is the LCG sample from `generateWorld3Pipes`, not MSVC `rand()`. If you change stamp tables or glyphs, regenerate:

```bash
bin/level_dump --stored 1 | sed '/^OK$/d' > expected/world1_stored.txt
bin/level_dump --stored 2 | sed '/^OK$/d' > expected/world2_stored.txt
bin/level_dump --authored 2 | sed '/^OK$/d' > expected/world2_authored.txt
bin/level_dump 3 | sed '/^OK$/d' > expected/world3_sample.txt
```

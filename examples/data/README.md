# Level tables

CSV dumps of the arrays in `Scene::createMap` / `createCoin` / `createFood` and `Role::createEnemy`.

World 3 is not listed here: pipe and coin Y values are rolled with `rand()` at runtime. Use `authored_world3(seed)` in the C++ library or `build/level_preview` to print one sample.

`MAP_NUMBER` is 30. Rows annotated `*_dropped_by_MAP_NUMBER` are authored in the source but never copied into `Scene::map`.

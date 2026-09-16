.PHONY: test python cpp snapshots clean

PYTHON ?= python3

test: python cpp

python:
	$(PYTHON) -m unittest discover -s python/tests -v

cpp:
	$(MAKE) -C cpp test

snapshots:
	$(PYTHON) python/cli.py preview --world 1 > output/world1.txt
	$(PYTHON) python/cli.py preview --world 2 > output/world2.txt
	$(PYTHON) python/cli.py preview --world 3 --seed 1 > output/world3_seed1.txt
	$(PYTHON) python/cli.py lint > output/lint.txt
	$(PYTHON) python/cli.py physics-jump --ticks 8 > output/physics_jump_head.txt
	$(PYTHON) python/cli.py extent --world 1 > output/extent_world1.json
	$(PYTHON) python/cli.py extent --world 2 > output/extent_world2.json
	$(PYTHON) python/cli.py decode --keys A+W+J > output/decode_awj.txt
	$(PYTHON) python/cli.py save-read ../MaoLiAo/gameRecord.dat > output/save_read.txt
	$(MAKE) -C cpp inertia_demo
	./cpp/inertia_demo | head -n 12 > output/inertia_demo_head.txt

clean:
	$(MAKE) -C cpp clean
	find python -name '__pycache__' -type d -exec rm -rf {} +

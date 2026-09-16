# Headless helpers. The real game still builds from MaoLiAo.sln on Windows.
CXX      ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra
PYTHON   ?= python3

.PHONY: all test demo maps clean

all: test

test:
	$(PYTHON) tests/test_all.py

demo: /tmp/maoliao_inertia_demo
	/tmp/maoliao_inertia_demo

/tmp/maoliao_inertia_demo: physics/inertia_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

maps:
	$(PYTHON) levels/ascii_map.py --world 1
	$(PYTHON) levels/ascii_map.py --world 2
	$(PYTHON) levels/ascii_map.py --world 3 --seed 2020

clean:
	rm -f /tmp/maoliao_inertia_demo /tmp/inertia_demo

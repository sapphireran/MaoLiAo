CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic
OUT := out
CPP := cpp
KIT := $(OUT)/test_kit $(OUT)/jump_table $(OUT)/aabb_check $(OUT)/save_roundtrip

.PHONY: all cpp python maps check clean

all: check

$(OUT):
	mkdir -p $(OUT)

$(OUT)/test_kit: $(CPP)/test_kit.cpp $(CPP)/maoliao_kit.hpp | $(OUT)
	$(CXX) $(CXXFLAGS) -o $@ $(CPP)/test_kit.cpp

$(OUT)/jump_table: $(CPP)/jump_table.cpp $(CPP)/maoliao_kit.hpp | $(OUT)
	$(CXX) $(CXXFLAGS) -o $@ $(CPP)/jump_table.cpp

$(OUT)/aabb_check: $(CPP)/aabb_check.cpp $(CPP)/maoliao_kit.hpp | $(OUT)
	$(CXX) $(CXXFLAGS) -o $@ $(CPP)/aabb_check.cpp

$(OUT)/save_roundtrip: $(CPP)/save_roundtrip.cpp $(CPP)/maoliao_kit.hpp | $(OUT)
	$(CXX) $(CXXFLAGS) -o $@ $(CPP)/save_roundtrip.cpp

cpp: $(KIT)
	$(OUT)/test_kit .
	$(OUT)/jump_table
	$(OUT)/aabb_check
	$(OUT)/save_roundtrip read catalog/saves/world1.dat

python:
	python3 tests/test_kit.py
	python3 tools/jump_table.py | head -n 12
	python3 tools/measure_gaps.py --world 1
	python3 tools/replay.py --world 1 replays/world1_opening.txt

maps: | $(OUT)
	python3 tools/dump_worlds.py -o $(OUT)

check: cpp python maps
	@echo "examples check ok"

clean:
	rm -rf $(OUT)

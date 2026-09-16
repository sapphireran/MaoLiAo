CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -O2
CPPFLAGS += -I lib

BIN_DIR := bin

PROGS := inertia_sim collision_probe level_dump save_roundtrip camera_parallax input_decode

.PHONY: all test clean

all: $(addprefix $(BIN_DIR)/,$(PROGS))

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/inertia_sim: inertia_sim/main.cpp lib/kinematics.hpp lib/constants.hpp | $(BIN_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ inertia_sim/main.cpp

$(BIN_DIR)/collision_probe: collision_probe/main.cpp lib/aabb.hpp lib/worlds.hpp lib/constants.hpp | $(BIN_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ collision_probe/main.cpp

$(BIN_DIR)/level_dump: level_dump/main.cpp lib/worlds.hpp lib/aabb.hpp lib/constants.hpp | $(BIN_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ level_dump/main.cpp

$(BIN_DIR)/save_roundtrip: save_roundtrip/main.cpp lib/save_format.hpp | $(BIN_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ save_roundtrip/main.cpp

$(BIN_DIR)/camera_parallax: camera_parallax/main.cpp lib/aabb.hpp lib/kinematics.hpp lib/constants.hpp | $(BIN_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ camera_parallax/main.cpp

$(BIN_DIR)/input_decode: input_decode/main.cpp lib/input_bits.hpp lib/constants.hpp | $(BIN_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ input_decode/main.cpp

test: all
	@set -e; \
	$(BIN_DIR)/inertia_sim >/tmp/maoliao_inertia.txt; \
	$(BIN_DIR)/collision_probe >/tmp/maoliao_collision.txt; \
	$(BIN_DIR)/level_dump --stored >/tmp/maoliao_levels.txt; \
	$(BIN_DIR)/save_roundtrip >/tmp/maoliao_save.txt; \
	$(BIN_DIR)/camera_parallax >/tmp/maoliao_camera.txt; \
	$(BIN_DIR)/input_decode >/tmp/maoliao_input.txt; \
	$(BIN_DIR)/level_dump --stored 1 | sed '/^OK$$/d' | diff -u expected/world1_stored.txt -; \
	$(BIN_DIR)/level_dump --stored 2 | sed '/^OK$$/d' | diff -u expected/world2_stored.txt -; \
	$(BIN_DIR)/level_dump --authored 2 | sed '/^OK$$/d' | diff -u expected/world2_authored.txt -; \
	$(BIN_DIR)/level_dump 3 | sed '/^OK$$/d' | diff -u expected/world3_sample.txt -; \
	echo "all example programs returned 0 and level dumps match expected/"

clean:
	rm -rf $(BIN_DIR)

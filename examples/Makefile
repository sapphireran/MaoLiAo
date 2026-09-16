# Portable reconstructions of 猫里奥 systems. No EasyX.
# Usage:  make test

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
INCLUDES  = -Icommon

BUILD = build

COMMON_SRC = \
	common/inertia_portable.cpp \
	common/collision_portable.cpp \
	common/map_portable.cpp \
	common/save_portable.cpp \
	common/hero_sim.cpp

COMMON_OBJ = $(COMMON_SRC:common/%.cpp=$(BUILD)/%.o)

PROGS = \
	01_inertia_motion \
	02_aabb_hit \
	03_command_bits \
	04_save_record \
	05_level_friction \
	06_side_scroller_sim

BINS = $(PROGS:%=$(BUILD)/%)

.PHONY: all test clean

all: $(BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: common/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/01_inertia_motion: 01_inertia_motion/main.cpp $(COMMON_OBJ) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(COMMON_OBJ) -o $@

$(BUILD)/02_aabb_hit: 02_aabb_hit/main.cpp $(COMMON_OBJ) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(COMMON_OBJ) -o $@

$(BUILD)/03_command_bits: 03_command_bits/main.cpp $(COMMON_OBJ) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(COMMON_OBJ) -o $@

$(BUILD)/04_save_record: 04_save_record/main.cpp $(COMMON_OBJ) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(COMMON_OBJ) -o $@

$(BUILD)/05_level_friction: 05_level_friction/main.cpp $(COMMON_OBJ) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(COMMON_OBJ) -o $@

$(BUILD)/06_side_scroller_sim: 06_side_scroller_sim/main.cpp $(COMMON_OBJ) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(COMMON_OBJ) -o $@

test: all
	@set -e; \
	for p in $(PROGS); do \
		echo "---- $$p ----"; \
		./$(BUILD)/$$p; \
	done; \
	echo "---- all example programs passed ----"

clean:
	rm -rf $(BUILD)

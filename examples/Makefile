CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
INCLUDES := -I common

BUILD := build

PROGRAMS := \
	$(BUILD)/inertia_jump \
	$(BUILD)/command_bits \
	$(BUILD)/aabb_vertices \
	$(BUILD)/save_format \
	$(BUILD)/world_catalog \
	$(BUILD)/ascii_minimap \
	$(BUILD)/walk_frames \
	$(BUILD)/parallax_scroll \
	$(BUILD)/tick_budget

.PHONY: all test clean

all: $(PROGRAMS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/inertia_jump: physics/inertia_jump.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ physics/inertia_jump.cpp

$(BUILD)/command_bits: input/command_bits.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ input/command_bits.cpp

$(BUILD)/aabb_vertices: collision/aabb_vertices.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ collision/aabb_vertices.cpp

$(BUILD)/save_format: save/save_format.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ save/save_format.cpp

$(BUILD)/world_catalog: levels/world_catalog.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ levels/world_catalog.cpp

$(BUILD)/ascii_minimap: levels/ascii_minimap.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ levels/ascii_minimap.cpp

$(BUILD)/walk_frames: sprites/walk_frames.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ sprites/walk_frames.cpp

$(BUILD)/parallax_scroll: camera/parallax_scroll.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ camera/parallax_scroll.cpp

$(BUILD)/tick_budget: gameloop/tick_budget.cpp common/*.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ gameloop/tick_budget.cpp

test: all
	@fail=0; \
	for p in $(PROGRAMS); do \
		echo "======== $$p ========"; \
		"$$p" || fail=1; \
	done; \
	exit $$fail

clean:
	rm -rf $(BUILD)

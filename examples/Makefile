# Portable MaoLiAo examples — no EasyX, no Windows headers.
#   make         build all
#   make test    build and run; non-zero if any check fails
#   make clean

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Wpedantic -O2
INCLUDES  = -Icommon
BUILD     = build

COMMON_SRC = \
	common/inertia.cpp \
	common/collision.cpp \
	common/map_data.cpp \
	common/save.cpp \
	common/camera.cpp \
	common/enemy.cpp

COMMON_OBJ = $(patsubst common/%.cpp,$(BUILD)/common_%.o,$(COMMON_SRC))

PROGS = \
	$(BUILD)/01_inertia_jump \
	$(BUILD)/02_friction_run \
	$(BUILD)/03_collision_aabb \
	$(BUILD)/04_level_preview \
	$(BUILD)/05_command_mask \
	$(BUILD)/06_save_roundtrip \
	$(BUILD)/07_camera_scroll \
	$(BUILD)/08_enemy_patrol

.PHONY: all test clean dirs

all: dirs $(PROGS)

dirs:
	@mkdir -p $(BUILD) $(BUILD)/tmp

$(BUILD)/common_%.o: common/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD)/01_inertia_jump: 01_inertia_jump/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/02_friction_run: 02_friction_run/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/03_collision_aabb: 03_collision_aabb/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/04_level_preview: 04_level_preview/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/05_command_mask: 05_command_mask/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/06_save_roundtrip: 06_save_roundtrip/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/07_camera_scroll: 07_camera_scroll/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD)/08_enemy_patrol: 08_enemy_patrol/main.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

test: all
	@fail=0; \
	for p in $(PROGS); do \
		echo; \
		echo "---- $$p ----"; \
		if ! ./$$p; then fail=1; fi; \
	done; \
	if [ $$fail -ne 0 ]; then echo; echo "ONE OR MORE EXAMPLES FAILED"; exit 1; fi; \
	echo; echo "ALL EXAMPLES PASSED"

clean:
	rm -rf $(BUILD)

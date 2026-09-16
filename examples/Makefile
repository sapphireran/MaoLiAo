CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
INCLUDES  = -Iinclude
SRC_DIR   = src
BUILD     = build

DEMOS = \
	inertia_demo \
	aabb_demo \
	command_demo \
	save_demo \
	friction_demo \
	world_dump \
	pipe_gen \
	loop_trace \
	run_all

BINS = $(addprefix $(BUILD)/,$(DEMOS))

.PHONY: all test clean

all: $(BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: $(SRC_DIR)/%.cpp include/maoliao_model.h | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $<

test: all
	@set -e; \
	$(BUILD)/inertia_demo; \
	$(BUILD)/aabb_demo; \
	$(BUILD)/command_demo; \
	$(BUILD)/save_demo testdata/gameRecord.roundtrip.dat; \
	$(BUILD)/friction_demo; \
	$(BUILD)/world_dump; \
	$(BUILD)/pipe_gen; \
	$(BUILD)/loop_trace; \
	$(BUILD)/run_all; \
	echo "ALL EXAMPLE DEMOS PASSED"

clean:
	rm -rf $(BUILD) testdata/gameRecord.roundtrip.dat

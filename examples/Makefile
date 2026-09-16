CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude
BIN      := bin

LABS := \
	jump_lab \
	friction_lab \
	aabb_lab \
	command_lab \
	save_lab \
	camera_lab \
	world_atlas \
	enemy_patrol \
	pipe_field \
	score_lab \
	tick_replay

BINS := $(addprefix $(BIN)/,$(LABS)) $(BIN)/run_all

.PHONY: all test clean

all: $(BINS)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/%: src/%.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $<

test: all
	@set -e; \
	for b in $(LABS); do \
		echo "== $$b =="; \
		$(BIN)/$$b; \
	done; \
	echo "== run_all =="; \
	$(BIN)/run_all

clean:
	rm -rf $(BIN)

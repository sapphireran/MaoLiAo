CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude
BIN      := bin

PROGS := jump_trajectory inertia_kinematics aabb_collision command_bits \
         map_layout scoring_rules camera_scroll world3_pipes enemy_patrol

.PHONY: all test clean python

all: $(addprefix $(BIN)/,$(PROGS))

$(BIN):
	mkdir -p $(BIN)

$(BIN)/%: src/%.cpp include/maoliao_model.hpp | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $<

test: all python
	@set -e; \
	for p in $(PROGS); do \
		echo "==== $$p ===="; \
		$(BIN)/$$p; \
	done
	@echo "==== all example programs passed ===="

python:
	python3 python/simulate_jump.py
	python3 python/dump_world_maps.py
	python3 python/check_testdata.py

clean:
	rm -rf $(BIN)

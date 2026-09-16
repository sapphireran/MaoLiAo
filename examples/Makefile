# Portable MaoLiAo extracts. No EasyX.
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
INCLUDES  = -Icommon

CORE = common/maoliao_core.cpp

BINS = \
	bin/01_inertia \
	bin/02_collision \
	bin/03_input_bits \
	bin/04_save_record \
	bin/05_camera_parallax \
	bin/06_level_end \
	bin/07_world3_pipes \
	bin/08_score_rules \
	bin/09_frame_sim

.PHONY: all test clean

all: $(BINS)

bin:
	mkdir -p bin

bin/01_inertia: 01_inertia/inertia_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 01_inertia/inertia_example.cpp $(CORE) -o $@

bin/02_collision: 02_collision/collision_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 02_collision/collision_example.cpp $(CORE) -o $@

bin/03_input_bits: 03_input_bits/input_bits_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 03_input_bits/input_bits_example.cpp $(CORE) -o $@

bin/04_save_record: 04_save_record/save_record_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 04_save_record/save_record_example.cpp $(CORE) -o $@

bin/05_camera_parallax: 05_camera_parallax/camera_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 05_camera_parallax/camera_example.cpp $(CORE) -o $@

bin/06_level_end: 06_level_end/level_end_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 06_level_end/level_end_example.cpp $(CORE) -o $@

bin/07_world3_pipes: 07_world3_pipes/pipe_generator_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 07_world3_pipes/pipe_generator_example.cpp $(CORE) -o $@

bin/08_score_rules: 08_score_rules/score_example.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 08_score_rules/score_example.cpp $(CORE) -o $@

bin/09_frame_sim: 09_frame_sim/frame_sim.cpp $(CORE) common/maoliao_core.h | bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) 09_frame_sim/frame_sim.cpp $(CORE) -o $@

test: all
	@set -e; \
	for b in $(BINS); do \
		echo; echo ">>>> $$b"; \
		./$$b; \
	done; \
	echo; echo "all example binaries passed"

clean:
	rm -rf bin

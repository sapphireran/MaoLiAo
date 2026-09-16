# Portable MaoLiAo examples. No EasyX. Requires a C++17 compiler.
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
INCLUDES  = -Iinclude

BINS = inertia_demo collision_demo command_bits_demo save_record_demo \
       jump_table camera_scroll_demo level_catalog

.PHONY: all test clean

all: $(BINS)

%: %.cpp include/maoliao_core.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $<

test: all
	@set -e; \
	./inertia_demo; \
	./collision_demo; \
	./command_bits_demo; \
	./save_record_demo --in fixtures/gameRecord.sample.dat --out fixtures/gameRecord.roundtrip.dat; \
	./jump_table > /tmp/maoliao_jump.tsv; \
	./camera_scroll_demo; \
	./level_catalog --warn --format summary; \
	./level_catalog --world 2 --warn --format summary; \
	echo "all example programs passed"

clean:
	rm -f $(BINS) fixtures/gameRecord.roundtrip.dat

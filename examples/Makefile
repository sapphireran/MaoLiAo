# Portable MaoLiAo examples — no EasyX, no Win32.
#   make        build all
#   make test   build and run; non-zero if any program fails
#   make clean  remove bin/

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2 -Iinclude
LDFLAGS  ?=

BIN      := bin
CORE_OBJ := $(BIN)/maoliao_core.o
CORE_SRC := src/maoliao_core.cpp
CORE_HDR := include/maoliao_core.hpp

PROGRAMS := \
	01_inertia \
	02_aabb \
	03_commands \
	04_save_load \
	05_map_layout \
	06_jump_sim \
	07_friction \
	08_bullet

BINS := $(addprefix $(BIN)/,$(PROGRAMS))

.PHONY: all test clean

all: $(BINS)

$(BIN):
	mkdir -p $(BIN)

$(CORE_OBJ): $(CORE_SRC) $(CORE_HDR) | $(BIN)
	$(CXX) $(CXXFLAGS) -c $(CORE_SRC) -o $@

$(BIN)/%: %.cpp $(CORE_OBJ) $(CORE_HDR) | $(BIN)
	$(CXX) $(CXXFLAGS) $< $(CORE_OBJ) $(LDFLAGS) -o $@

test: all
	@fail=0; \
	for p in $(PROGRAMS); do \
		echo "-------- $$p --------"; \
		if ! $(BIN)/$$p; then \
			echo "$$p FAILED"; \
			fail=1; \
		fi; \
		echo; \
	done; \
	if [ $$fail -ne 0 ]; then \
		echo "examples: one or more programs failed"; \
		exit 1; \
	fi; \
	echo "examples: all programs passed"

clean:
	rm -rf $(BIN)

CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2

.PHONY: all test clean run

all: physics_sandbox test_physics

physics_sandbox: physics_sandbox.cpp inertia_portable.h physics_constants.h
	$(CXX) $(CXXFLAGS) -o $@ physics_sandbox.cpp

test_physics: test_physics.cpp inertia_portable.h physics_constants.h
	$(CXX) $(CXXFLAGS) -o $@ test_physics.cpp

test: test_physics physics_sandbox
	./test_physics
	./physics_sandbox jump
	./physics_sandbox friction
	./physics_sandbox rail

run: physics_sandbox
	./physics_sandbox jump

clean:
	rm -f physics_sandbox test_physics

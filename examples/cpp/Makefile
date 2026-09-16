CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

.PHONY: all clean test

all: inertia_demo command_flags

inertia_demo: inertia_demo.cpp constants.hpp
	$(CXX) $(CXXFLAGS) -o $@ inertia_demo.cpp

command_flags: command_flags.cpp constants.hpp
	$(CXX) $(CXXFLAGS) -o $@ command_flags.cpp

test: all
	./inertia_demo | head -n 6
	./command_flags 21
	./command_flags A+W+J

clean:
	rm -f inertia_demo command_flags

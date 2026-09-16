CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -O2 -Iinclude
LDFLAGS ?=

SRC = \
	src/inertia.cpp \
	src/aabb.cpp \
	src/commands.cpp \
	src/savefile.cpp \
	src/levels.cpp \
	src/camera.cpp \
	src/hero.cpp \
	src/render.cpp

OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

DEMOS = \
	build/inertia_demo \
	build/jump_arc \
	build/aabb_demo \
	build/command_demo \
	build/savefile_demo \
	build/level_preview \
	build/camera_demo

.PHONY: all test clean dirs demos

all: dirs demos build/test_maoliao

dirs:
	@mkdir -p build data

demos: $(DEMOS)

build/%.o: src/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/inertia_demo: demos/inertia_demo.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/jump_arc: demos/jump_arc.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/aabb_demo: demos/aabb_demo.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/command_demo: demos/command_demo.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/savefile_demo: demos/savefile_demo.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/level_preview: demos/level_preview.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/camera_demo: demos/camera_demo.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

build/test_maoliao: tests/test_maoliao.cpp $(OBJ) | dirs
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@

test: all
	./build/test_maoliao

clean:
	rm -rf build
	rm -f data/_demo_save.dat data/_test_save.dat

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -O2
INCLUDES := -Icommon
BIN_DIR := bin

.PHONY: all test clean

all: \
	$(BIN_DIR)/inertia_demo \
	$(BIN_DIR)/collision_demo \
	$(BIN_DIR)/map_layout \
	$(BIN_DIR)/save_file \
	$(BIN_DIR)/input_bits

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/inertia_demo: inertia_demo/main.cpp common/inertia_portable.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ inertia_demo/main.cpp common/inertia_portable.cpp

$(BIN_DIR)/collision_demo: collision_demo/main.cpp common/aabb.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ collision_demo/main.cpp common/aabb.cpp

$(BIN_DIR)/map_layout: map_layout/main.cpp common/map_data.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ map_layout/main.cpp common/map_data.cpp

$(BIN_DIR)/save_file: save_file/main.cpp common/save_format.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ save_file/main.cpp common/save_format.cpp

$(BIN_DIR)/input_bits: input_bits/main.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ input_bits/main.cpp

test: all
	$(BIN_DIR)/inertia_demo
	$(BIN_DIR)/collision_demo
	$(BIN_DIR)/map_layout
	$(BIN_DIR)/save_file
	$(BIN_DIR)/input_bits

clean:
	rm -rf $(BIN_DIR)

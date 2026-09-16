CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -I.

DEMOS = physics_kinematics jump_arc aabb_collision level_dump command_bits save_record camera_rail friction_slide life_cycle

.PHONY: all test clean

all: $(DEMOS)

physics_kinematics: physics_kinematics.cpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ physics_kinematics.cpp

jump_arc: jump_arc.cpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ jump_arc.cpp

aabb_collision: aabb_collision.cpp portable/aabb.hpp portable/level_data.hpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ aabb_collision.cpp

level_dump: level_dump.cpp portable/level_data.hpp portable/aabb.hpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ level_dump.cpp

command_bits: command_bits.cpp portable/commands.hpp
	$(CXX) $(CXXFLAGS) -o $@ command_bits.cpp

save_record: save_record.cpp portable/save_io.hpp
	$(CXX) $(CXXFLAGS) -o $@ save_record.cpp

camera_rail: camera_rail.cpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ camera_rail.cpp

friction_slide: friction_slide.cpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ friction_slide.cpp

life_cycle: life_cycle.cpp portable/maoliao_math.hpp
	$(CXX) $(CXXFLAGS) -o $@ life_cycle.cpp

test: all
	./physics_kinematics
	./jump_arc
	./aabb_collision
	./level_dump
	./command_bits
	./save_record
	./camera_rail
	./friction_slide
	./life_cycle

clean:
	rm -f $(DEMOS) gameRecord.example.dat

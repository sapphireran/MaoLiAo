CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -O2
INCLUDES := -Iinclude -Itests

BUILD := build

DEMOS := demo_jump demo_friction demo_collision demo_commands demo_save \
	demo_pipes demo_camera demo_scoring demo_walkthrough
TESTS := test_inertia test_collision test_friction test_commands test_save \
	test_pipes test_camera test_scoring test_hero

DEMO_BINS := $(addprefix $(BUILD)/,$(DEMOS))
TEST_BINS := $(addprefix $(BUILD)/,$(TESTS))

.PHONY: all demos tests test run-demos clean

all: demos tests

demos: $(DEMO_BINS)

tests: $(TEST_BINS)

$(BUILD)/demo_%: src/demo_%.cpp include/maoliao.hpp include/maoliao/*.hpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

$(BUILD)/test_%: tests/test_%.cpp tests/check.hpp include/maoliao.hpp include/maoliao/*.hpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

test: tests
	@fail=0; \
	for t in $(TESTS); do \
		$(BUILD)/$$t || fail=1; \
	done; \
	exit $$fail

run-demos: demos
	@for d in $(DEMOS); do \
		echo "===== $$d ====="; \
		$(BUILD)/$$d; \
	done

clean:
	rm -rf $(BUILD)

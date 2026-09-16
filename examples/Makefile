.PHONY: test physics toolkit snippets

test: toolkit physics snippets

toolkit:
	python3 toolkit/test_maoliao_levels.py

physics:
	$(MAKE) -C physics test

snippets:
	g++ -std=c++17 -Wall -Wextra -o /tmp/hit_test_example snippets/hit_test_example.cpp
	/tmp/hit_test_example

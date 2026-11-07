CPPC=g++
CPPFLAGS=-Wall -Wextra -O2 -std=c++23

.PHONY: all clean 

all: test

test.cpp:

test: test.cpp
	$(CPPC) $(CPPFLAGS) $< -o $@

example: trains_example.cpp
	$(CPPC) $(CPPFLAGS) $< -o $@
 
# clean:
# 	rm -f *.o test

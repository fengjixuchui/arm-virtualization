
# Makefile to build trace plugin
# Copyright ARM Limited 2009-2010 All Rights Reserved.
#

BITS ?= 32

CPPFLAGS = -I ../../../include
CXXFLAGS = -W -Wall -Werror -m$(BITS) -O3 -DNDEBUG -fomit-frame-pointer -fPIC

ifeq ($(BITS),32)
CXXFLAGS += -march=pentiumpro -mtune=pentium4
endif

all: RunTimeParameterTest.so

RunTimeParameterTest.o: RunTimeParameterTest.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

RunTimeParameterTest.so: RunTimeParameterTest.o
	$(CXX) -m$(BITS) -pthread -ldl -lrt -shared -o $@ RunTimeParameterTest.o

clean:
	rm -f *.o
	rm -f RunTimeParameterTest.so

# Use C++ linker:
CC=CXX

# End of Makefile

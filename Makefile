# CXX=mpiCC
CXX=g++
#CXXFLAGS:=-Wall -Wextra -Werror -pedantic -std=c++17
CXXFLAGS:=-Wall -Wextra -pedantic -std=c++17
RELEASEFLAGS:=-O3
DEBUGFLAGS:=-g
SOURCEDIR=src
APPNAME:=troons
SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
TESTCASESDIR=testcases
TESTCASEFILE:= $(TESTCASESDIR)/example.in
SIMPLETESTCASEFILE := $(TESTCASESDIR)/sample1.in

.PHONY: all clean test generateTest quickTest
all: submission

submission: main.o
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $(APPNAME) $^ $(shell find -name '*.o' ! -name 'main.o')

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -c $^ $(SOURCES)

clean:
	$(RM) *.o troons test generateTest

debug: main.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -D DEBUG -o troons main.cpp $(SOURCES)

generateTest: lib/GenerateTest.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o generateTest $^
	./generateTest 5000 5000 4000 > $(TESTCASEFILE)

quickTest: clean submission
	./$(APPNAME) $(TESTCASEFILE)

simpleTest: clean submission
	./$(APPNAME) $(SIMPLETESTCASEFILE)
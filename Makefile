#CXX=mpiCC
CXX=g++
CXXFLAGS:=-Wall -Wextra -Werror -pedantic -std=c++17
RELEASEFLAGS:=-O3
DEBUGFLAGS:=-g
SOURCEDIR=src
APPNAME:=troons
SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
TESTCASESDIR=testcases
TESTCASEFILE:= $(TESTCASESDIR)/generatedInput.in
SIMPLETESTCASEFILE := $(TESTCASESDIR)/sample2.in

.PHONY: all clean test generateTest quickTest compareOutput
all: submission

submission: main.o
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $(APPNAME) $^ $(shell find -name '*.o' ! -name 'main.o')

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -c $^ $(SOURCES)

clean:
	$(RM) *.o troons test generateTest *.out

debug: main.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -D DEBUG -o troons main.cpp $(SOURCES)

generateTest: lib/GenerateTest.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o generateTest $^
	./generateTest 17000 200000 5 > $(TESTCASEFILE)

quickTest: clean submission
	./$(APPNAME) $(TESTCASEFILE)

simpleTest: clean submission
	./$(APPNAME) $(SIMPLETESTCASEFILE)

compareOutput: clean submission
	./$(APPNAME) $(SIMPLETESTCASEFILE) > troons.out
	./troons_seq $(SIMPLETESTCASEFILE) > troons_seq.out
	diff -ZB troons.out troons_seq.out
CXX=mpiCC
#CXX=g++
#CXXFLAGS:=-Wall -Werror -pedantic -std=c++17
CXXFLAGS:=-Wall -Werror -pedantic -std=c++17
RELEASEFLAGS:=-O3
DEBUGFLAGS:=-g
SOURCEDIR=src
APPNAME:=troons
TESTCASESDIR=testcases
TESTCASEFILE:= $(TESTCASESDIR)/generatedInput.in
SIMPLETESTCASEFILE := $(TESTCASESDIR)/sample2.in

.PHONY: all clean test generateTest quickTest compareOutput compareTimingSeq
all: submission

compareTimingSeq: clean submission generateTest
	mkdir -p result
	perf stat -o result/our_result.out ./$(APPNAME) $(TESTCASEFILE)
	perf stat -o result/troons_seq1_result.out ./troons_seq $(TESTCASEFILE)

submission: main.o
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $(APPNAME) $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -c $^

clean:
	$(RM) *.o troons test generateTest *.out

debug: main.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -D DEBUG -o troons $^

generateTest: lib/GenerateTest.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o generateTest $^
	./generateTest 17000 200000 100 > $(TESTCASEFILE)

quickTest: clean submission
	./$(APPNAME) $(TESTCASEFILE)

simpleTest: clean submission
	./$(APPNAME) $(SIMPLETESTCASEFILE)

compareOutput: clean submission
	./$(APPNAME) $(SIMPLETESTCASEFILE) > troons.out
	./troons_seq $(SIMPLETESTCASEFILE) > troons_seq.out
	diff -ZB troons.out troons_seq.out

copySlurm: clean submission
	cp $(TESTCASEFILE) /nfs/home/${USER}
	cp ./$(APPNAME) /nfs/home/${USER}
	# srun -N 1 /nfs/home/${USER}/troons /nfs/home/${USER}/$(TESTCASEFILE) need to be run manually in ~
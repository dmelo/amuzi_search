CXX=g++
CXXFLAGS=-g -O3

all: amuzi_search catuint

clean:
	rm *.o amuzi_search catuint

amuzi_search: timer.o bmh.o suffixarray.o testsort.o main.cc
	g++ -O3 -g timer.o bmh.o suffixarray.o testsort.o searchalg.o main.cc -o amuzi_search -lpthread

searchalg.o: searchalg.cc searchalg.h defs.h
	$(CXX) $(CXXFLAGS) -c searchalg.cc -o searchalg.o

timer.o: timer.cc timer.h searchalg.o
	$(CXX) $(CXXFLAGS) -c timer.cc -o timer.o

bmh.o: bmh.cc bmh.h searchalg.o
	$(CXX) $(CXXFLAGS) -c bmh.cc -o bmh.o

testsort.o: testsort.cc testsort.h searchalg.o
	$(CXX) $(CXXFLAGS) -c testsort.cc -o testsort.o

suffixarray.o: suffixarray.cc suffixarray.h searchalg.o
	$(CXX) $(CXXFLAGS) -c suffixarray.cc -o suffixarray.o

catuint: catuint.cc defs.h
	$(CXX) $(CXXFLAGS) catuint.cc -o catuint


CXX=g++
CXXFLAGS=-g -O3

all: amuzi_search

amuzi_search: timer.o bmh.o suffixarray.o main.cc
	g++ -O3 -g *.cc -o amuzi_search -lpthread

searchalg.o:
	$(CXX) $(CXXFLAGS) -c searchalg.cc -o searchalg.o

timer.o: timer.cc timer.h searchalg.o
	$(CXX) $(CXXFLAGS) -c timer.cc -o timer.o

bmh.o: bmh.cc bmh.h searchalg.o
	$(CXX) $(CXXFLAGS) -c bmh.cc -o bmh.o

suffixarray.o: suffixarray.cc suffixarray.h searchalg.o
	$(CXX) $(CXXFLAGS) -c suffixarray.cc -o suffixarray.o


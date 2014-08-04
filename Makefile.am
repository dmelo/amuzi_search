SRC=./src
BIN=./bin
CXX=g++
CXXFLAGS=-g -Ofast -march=native
#CXXFLAGS=-g

all: ${BIN}/amuzi_search ${BIN}/catuint

clean:
	rm *.o ${BIN}/amuzi_search ${BIN}catuint

${BIN}/amuzi_search: ${BIN}/timer.o ${BIN}/bmh.o ${BIN}/suffixarray.o ${BIN}/testsort.o ${SRC}/main.cc
	$(CXX) $(CXXFLAGS) ${BIN}/timer.o ${BIN}/bmh.o ${BIN}/suffixarray.o ${BIN}/testsort.o ${BIN}/searchalg.o ${SRC}/main.cc -o ${BIN}/amuzi_search -lpthread

${BIN}/searchalg.o: ${SRC}/searchalg.cc ${SRC}/searchalg.h ${SRC}/defs.h
	$(CXX) $(CXXFLAGS) -c ${SRC}/searchalg.cc -o ${BIN}/searchalg.o

${BIN}/timer.o: ${SRC}/timer.cc ${SRC}/timer.h ${BIN}/searchalg.o
	$(CXX) $(CXXFLAGS) -c ${SRC}/timer.cc -o ${BIN}/timer.o

${BIN}/bmh.o: ${SRC}/bmh.cc ${SRC}/bmh.h ${BIN}/searchalg.o
	$(CXX) $(CXXFLAGS) -c ${SRC}/bmh.cc -o ${BIN}/bmh.o

${BIN}/testsort.o: ${SRC}/testsort.cc ${SRC}/testsort.h ${BIN}/searchalg.o
	$(CXX) $(CXXFLAGS) -c ${SRC}/testsort.cc -o ${BIN}/testsort.o

${BIN}/suffixarray.o: ${SRC}/suffixarray.cc ${SRC}/suffixarray.h ${BIN}/searchalg.o
	$(CXX) $(CXXFLAGS) -c ${SRC}/suffixarray.cc -o ${BIN}/suffixarray.o

${BIN}/catuint: ${SRC}/catuint.cc ${SRC}/defs.h
	$(CXX) $(CXXFLAGS) ${SRC}/catuint.cc -o ${BIN}/catuint


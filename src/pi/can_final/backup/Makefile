CXX = g++
FLAGS = -w -std=c++11

all: test_can

test_can: lib.o can_lib.o test_can.o
	$(CXX) $(FLAGS) -o test_can test_can.o


# Object files

lib.o: lib.c
	$(CXX) $(FLAGS) -c lib.c

can_lib.o: can_lib.cpp
	$(CXX) $(FLAGS) -c can_lib.cpp

test_can.o: test_can.cpp
	$(CXX) $(FLAGS) -c test_can.cpp

clean:
	-rm -f *.o *.gch
	-rm -f test_can

run: test_can
	./test_can

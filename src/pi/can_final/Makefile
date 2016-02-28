CXX = g++
FLAGS = -w -std=c++11

all: test_can

test_can: lib.o candump_lib.o test_can.o
	$(CXX) $(FLAGS) -o test_can test_can.o


# Object files

lib.o: lib.c
	$(CXX) $(FLAGS) -c lib.c

candump_lib.o: candump_lib.c
	$(CXX) $(FLAGS) -c candump_lib.c

test_can.o: test_can.c
	$(CXX) $(FLAGS) -c test_can.c

clean:
	-rm -f *.o
	-rm -f test_can

run: test_can
	./test_can

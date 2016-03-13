CXX = g++
FLAGS = -w -std=c++11

all: main_can

main_can: can_lib.o main_can.o bt_lib.o
	$(CXX) $(FLAGS) -o main_can main_can.o can_lib.o bt_lib.o


# Object files

main_can.o: main_can.cpp
	$(CXX) $(FLAGS) -c main_can.cpp


can_lib.o: can_lib.cpp
	$(CXX) $(FLAGS) -c can_lib.cpp

bt_lib.o: bt_lib.cpp
	$(CXX) $(FLAGS) -c bt_lib.cpp

clean:
	-rm -f *.o
	-rm -f main_can

remake: clean all

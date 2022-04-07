APP_NAME=sudoku
APP_NAME2=sudoku_backtrack

OBJS=sudoku.o
OBJS2=sudoku_backtrack.o

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -Wall -fopenmp -Wno-unknown-pragmas

default: $(APP_NAME)

b: $(APP_NAME2)

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(APP_NAME2): $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS2)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME) *.class
	/bin/rm -rf *~ *.o $(APP_NAME2) *.class
	rm *.txt


CXX = g++
CXXFLAGS = -Wall -g -MMD -std=c++11

output: sraFilter.o
	g++ sraFilter.o -o sraFilter

sraFilter.o: sraFilter.cpp
	g++ -c sraFilter.cpp

clean:
	rm *.o sraFilter

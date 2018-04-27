CXX = g++
CXXFLAGS = -fPIC -std=c++0x
CXXFILES = timing.cpp
OFILES = $(CXXFILES:.cpp=.o)

all: timing.so

timing.so: timing.o
	$(CXX) $(CXXFLAGS) -shared -o libtiming.so $(OFILES)
timing.o:
	$(CXX) $(CXXFLAGS) -c -o timing.o $(CXXFILES)
timingTest:
	$(CXX) $(CXXFLAGS) -L. -ltiming -o timingTest timingTest.cpp
clean:
	rm libtiming.so timingTest timing.o

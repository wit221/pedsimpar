CXX = g++
CXXFLAGS = -g -O2 -fno-inline -std=c++0x -fopenmp -fPIC
CXXFILES = ped_vector.cpp ped_outputwriter.cpp ped_scene.cpp ped_tree.cpp ped_obstacle.cpp ped_waypoint.cpp ped_agent.cpp
OFILES = $(CXXFILES:.cpp=.o)

all: libpedsimpar.so

libpedsimpar.so: $(OFILES)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,libpedsimpar.so -o libpedsimpar.so $(OFILES)

examples: libpedsim.so examples/example*.cpp
	g++ examples/example01.cpp -o example01 -I. -lpedsim -L. -g -std=c++0x
	g++ examples/example02.cpp -o example02 -I. -lpedsim -L. -g -std=c++0x
	g++ examples/example03.cpp -o example03 -I. -lpedsim -L. -g -std=c++0x
	g++ examples/example04.cpp -o example04 -I. -lpedsim -L. -g -std=c++0x
	g++ examples/example05.cpp -o example05 -I. -lpedsim -L. -g -std=c++0x

doc: documentation/doxygen.conf
	doxygen documentation/doxygen.conf
	cp ./documentation/*.png ./documentation/latex
	cp ./documentation/*.png ./documentation/html
	cp ../ecosystem/2dvis/documentation/*.png documentation/html
	cp ../ecosystem/2dvis/documentation/*.png documentation/latex
	cp ../ecosystem/3dvis/documentation/*.png documentation/html
	cp ../ecosystem/3dvis/documentation/*.png documentation/latex
	cp ../ecosystem/demoapp/documentation/*.png documentation/html
	cp ../ecosystem/demoapp/documentation/*.png documentation/latex
#	cp ./documentation/distributed/images/* documentation/html
#	cp ./documentation/distributed/images/* documentation/latex

clean:
	rm *.o *.so test_all example*

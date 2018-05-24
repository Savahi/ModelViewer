all: 3d

3d: 3d.o Spdr3dModel.o
	g++ 3d.o Spdr3dModel.o -o 3d -lGL -lGLU -lglut

3d.o: 3d.cpp
	g++ -c 3d.cpp

Spdr3dModel.o: Spdr3dModel.cpp
	g++ -c Spdr3dModel.cpp

clean:
	rm -f *.o
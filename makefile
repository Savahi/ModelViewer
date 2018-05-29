all: ModelViewer

ModelViewer: ModelViewer.o Spdr3dModel.o
	g++ ModelViewer.o Spdr3dModel.o -o ModelViewer -lGL -lGLU -lglut

ModelViewer.o: ModelViewer.cpp
	g++ -c ModelViewer.cpp

Spdr3dModel.o: Spdr3dModel.cpp
	g++ -c -std=c++11 Spdr3dModel.cpp

clean:
	rm -f *.o
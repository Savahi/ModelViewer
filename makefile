all: SpdrModelViewer

SpdrModelViewer: SpdrModelViewer.o Spdr3dModel.o
	g++ SpdrModelViewer.o Spdr3dModel.o -o SpdrModelViewer -lGL -lGLU -lglut

SpdrModelViewer.o: SpdrModelViewer.cpp
	g++ -c SpdrModelViewer.cpp

Spdr3dModel.o: Spdr3dModel.cpp
	g++ -c Spdr3dModel.cpp

clean:
	rm -f *.o
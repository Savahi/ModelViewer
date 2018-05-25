#include <vector>
#include <iostream>

class Spdr3dVertex {
	public:
		float mX, mY, mZ;
		Spdr3dVertex( void ) {
			std::cout << "Constructor for Spdr3dVertex\n"; 
		}
		Spdr3dVertex( float x, float y, float z ) : mX(x), mY(y), mZ(z) {
			std::cout << "Constructor for Spdr3dVertex\n"; 
		}
		~Spdr3dVertex(){
			std::cout << "Destructor for Spdr3dVertex\n"; 
		}
		int setXYZ( float x, float y, float z ) {
			this->mX = x;
			this->mY = y;
			this->mZ = z;
		}
};

class Spdr3dFacet {
	public:
		std::vector<Spdr3dVertex> mVertices;

		int add( Spdr3dVertex& vertex ) {
			this->mVertices.push_back( vertex );
		}

		int clear( void ) {
			this->mVertices.clear();
		}

		Spdr3dFacet() {
			std::cout << "Constructor for Spdr3dFacet\n";
		}
		~Spdr3dFacet() {
			std::cout << "Destructor for Spdr3dFacet\n";
		}
};

class Spdr3dObject {
	public:
		std::vector<Spdr3dFacet> mFacets; 

		int add( Spdr3dFacet& facet ) {
			this->mFacets.push_back( facet );
		}

		Spdr3dObject() {
			std::cout << "Constructor for Spdr3dObject\n";
		}

		~Spdr3dObject() {
			std::cout << "Destructor for Spdr3dObject\n";
		}
};

class Spdr3dOperation {
	public:
		std::vector<Spdr3dObject> mObjects; 

		int add( Spdr3dObject& object ) {
			this->mObjects.push_back( object );
		}

		Spdr3dOperation() {
			std::cout << "Constructor for Spdr3dOperation\n";
		}

		~Spdr3dOperation() {
			std::cout << "Destructor for Spdr3dOperation\n";
		}
};

class Spdr3dModel {
	public:
		std::vector<Spdr3dOperation> mOperations; 

		int add( Spdr3dOperation& operation ) {
			this->mOperations.push_back( operation );
		}

		Spdr3dModel() {
			std::cout << "Constructor for Spdr3dModel\n";
		}

		~Spdr3dModel() {
			std::cout << "Destructor for Spdr3dModel\n";
		}
		
		static void display( Spdr3dModel& model );
};


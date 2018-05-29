#include <vector>
#include <iostream>
#include <string>
#include <ctime>

class Spdr3dVertex {
	public:
		float mX, mY, mZ;
		Spdr3dVertex( void ) {
			; // std::cout << "Constructor for Spdr3dVertex\n"; 
		}
		Spdr3dVertex( float x, float y, float z ) : mX(x), mY(y), mZ(z) {
			; // std::cout << "Constructor for Spdr3dVertex\n"; 
		}
		~Spdr3dVertex(){
			; // std::cout << "Destructor for Spdr3dVertex\n"; 
		}
		int setXYZ( float x, float y, float z ) {
			this->mX = x;
			this->mY = y;
			this->mZ = z;
			return 0;
		}
};

class Spdr3dFacet {
	public:
		std::vector<Spdr3dVertex> mVertices;

		int add( Spdr3dVertex& vertex ) {
			this->mVertices.push_back( vertex );
			return 0;
		}

		int clear( void ) {
			this->mVertices.clear();
		}

		Spdr3dFacet() {
			; // std::cout << "Constructor for Spdr3dFacet\n";
		}
		~Spdr3dFacet() {
			; // std::cout << "Destructor for Spdr3dFacet\n";
		}
};

class Spdr3dObject {
	public:
		std::vector<Spdr3dFacet> mFacets; 

		int add( Spdr3dFacet& facet ) {
			this->mFacets.push_back( facet );
			return 0;
		}

		Spdr3dObject() {
			; // std::cout << "Constructor for Spdr3dObject\n";
		}

		~Spdr3dObject() {
			; // std::cout << "Destructor for Spdr3dObject\n";
		}
};

class Spdr3dOperation {
	public:
		std::vector<Spdr3dObject> mObjects; 
		
		tm tmActualStart, tmActualFinish, tmAsapStart, tmAsapFinish;
		time_t mActualStart, mActualFinish, mAsapStart, mAsapFinish;
		bool bDatesInitialized;

		int iProgress;

		int add( Spdr3dObject& object ) {
			this->mObjects.push_back( object );
			return 0;
		}

		int setProgress( int iProgress ) {
			this->iProgress == iProgress;
			return 0;
		}

		int parseSpdrDates( char *cpActualStart, char *cpActualFinish, char *cpAsapStart, char *cpAsapFinish );

		Spdr3dOperation() : bDatesInitialized(false), iProgress(100) {
			; // std::cout << "Constructor for Spdr3dOperation\n";
		}

		~Spdr3dOperation() {
			; // std::cout << "Destructor for Spdr3dOperation\n";
		}
};

class Spdr3dModel {
	public:
		std::vector<Spdr3dOperation> mOperations; 

		int add( Spdr3dOperation& operation ) {
			this->mOperations.push_back( operation );
			return 0;
		}

		bool mAxis;
		float mGrid;
		float mXMargin, mYMargin, mZMargin;

		Spdr3dModel() {
			; // std::cout << "Constructor for Spdr3dModel\n";
		}

		~Spdr3dModel() {
			; // std::cout << "Destructor for Spdr3dModel\n";
		}

		void display( int argc, char* argv[] );
		
		static void display( Spdr3dModel& model, int argc, char* argv[] );
};


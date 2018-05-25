#include <vector>
#include <iostream>
#include <math.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

#include "Spdr3dModel.hpp"

static void displayCoordsInitializer( Spdr3dModel& model );

static void displayModel();
static void displayFacet( Spdr3dFacet& facet );

static float f_ModelW, f_ModelL, f_ModelH, f_ModelMinX, f_ModelMaxX, f_ModelMinY, f_ModelMaxY, f_ModelMinZ, f_ModelMaxZ;
static int i_ModelRotateX, i_ModelRotateY;
static Spdr3dModel *o_Model;

static void displayKeys( int key, int x, int y );
static void displayReshape( GLsizei width, GLsizei height );


void Spdr3dModel::display( Spdr3dModel& model, int argc, char* argv[] ) {
	displayCoordsInitializer( model );
  	o_Model = &model;

  	i_ModelRotateX = 30;
  	i_ModelRotateY = 15;

  	glutInit( &argc, argv ); // Initialize GLUT and process user parameters

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Request double buffered true color window with Z-buffer

	glutInitWindowSize(640, 480);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner  
	glutCreateWindow("How the Building has being Built...");

	glEnable(GL_DEPTH_TEST); // Enable Z-buffer depth test

	// Callback functions
	glutDisplayFunc(displayModel);
	glutSpecialFunc(displayKeys);
	glutReshapeFunc(displayReshape);       // Register callback handler for window re-size event

	//  Pass control to GLUT for events
	glutMainLoop();
}


static void displayCoordsInitializer( Spdr3dModel& model ) {

	bool bFirst = true;
	for( std::vector<Spdr3dOperation>::iterator  op = model.mOperations.begin() ; op != model.mOperations.end() ; ++op ) {
	    for( std::vector<Spdr3dObject>::iterator ob = (*op).mObjects.begin() ; ob != (*op).mObjects.end() ; ++ob ) {
		    for( std::vector<Spdr3dFacet>::iterator fa = (*ob).mFacets.begin() ; fa != (*ob).mFacets.end() ; ++fa ) {	
			    for( std::vector<Spdr3dVertex>::iterator ve = (*fa).mVertices.begin() ; ve != (*fa).mVertices.end() ; ++ve ) {
			    	float x = (*ve).mX;
			    	float y = (*ve).mY;
			    	float z = (*ve).mZ;

			    	if( bFirst ) {
			    		f_ModelMinX = f_ModelMaxX = x;
			    		f_ModelMinY = f_ModelMaxY = y;
			    		f_ModelMinZ = f_ModelMaxZ = z;
			    		bFirst = false;
			    	} else {
			    		if( x < f_ModelMinX ) {
			    			f_ModelMinX = x;
			    		}
			    		if( x > f_ModelMaxX ) {
			    			f_ModelMaxX = y;
			    		}
			    		if( y < f_ModelMinY ) {
			    			f_ModelMinY = y;
			    		}
			    		if( y > f_ModelMaxY ) {
			    			f_ModelMaxY = y;
			    		}
			    		if( z < f_ModelMinZ ) {
			    			f_ModelMinZ = z;
			    		}
			    		if( z > f_ModelMaxZ ) {
			    			f_ModelMaxZ = z;
			    		}
			    	}
			    }
			}
		}
	}
	f_ModelW = f_ModelMaxX - f_ModelMinX;
	f_ModelL = f_ModelMaxY - f_ModelMinY;
	f_ModelH = f_ModelMaxZ - f_ModelMinZ;
}


static void displayModel( void ) {
 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   //  Clear screen and Z-buffer
	
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
 
 	glLoadIdentity(); // Reset transformations
  
	glPushMatrix();

	glTranslatef( (f_ModelW)/2.0, 0.0, (f_ModelL)/2.0 );  
	glRotatef( i_ModelRotateY, 0.0, 1.0, 0.0 );
	glTranslatef( -(f_ModelW)/2.0, 0.0, -(f_ModelL)/2.0  );

	glTranslatef( 0.0, f_ModelH/2.0, (f_ModelL)/2.0 );
	glRotatef( i_ModelRotateX, 1.0, 0.0, 0.0 );
	glTranslatef( 0.0, -f_ModelH/2.0, -(f_ModelL)/2.0 );

    for( std::vector<Spdr3dOperation>::iterator  op = (*o_Model).mOperations.begin() ; op != (*o_Model).mOperations.end() ; ++op ) {
        for( std::vector<Spdr3dObject>::iterator ob = (*op).mObjects.begin() ; ob != (*op).mObjects.end() ; ++ob ) {
	        for( std::vector<Spdr3dFacet>::iterator fa = (*ob).mFacets.begin() ; fa != (*ob).mFacets.end() ; ++fa ) {
	        	displayFacet( *fa );
			}
		}
    }	

	glPopMatrix();
	glFlush();
	glutSwapBuffers(); 
}


static void displayFacet( Spdr3dFacet& facet ) {
	glBegin(GL_POLYGON);
	glColor3f( 1.0, 1.0, 1.0 );
    for( std::vector<Spdr3dVertex>::iterator ve = facet.mVertices.begin() ; ve != facet.mVertices.end() ; ++ve ) {
    	std::cout << "x=" << (*ve).mX << ", y=" << (*ve).mY << ", z=" << (*ve).mZ << "\n";
		glVertex3f(  (*ve).mX, (*ve).mY, (*ve).mZ );
    }
	glEnd();
}


static void displayKeys( int key, int x, int y ) {
 
	switch(key) {
		case GLUT_KEY_RIGHT:
		  i_ModelRotateY += 5;
		  break;
		case GLUT_KEY_LEFT:
		  i_ModelRotateY -= 5;
		  break;
		case GLUT_KEY_UP:
		  i_ModelRotateX += 5;
		  break;
		case GLUT_KEY_DOWN:
		  i_ModelRotateX -= 5;
		  break;
		/*		 
		case GLUT_KEY_PAGE_UP:
		  constructionStep = (constructionStep < constructionStepsNum-1) ? (constructionStep+1) : constructionStepsNum-1;
		  break;
		case GLUT_KEY_PAGE_DOWN:
		  constructionStep = (constructionStep > 0) ? (constructionStep-1) : 0;
		  break;
		*/
	} 
  //  Request display update
  glutPostRedisplay();
}


static void displayReshape( GLsizei width, GLsizei height ) {

  if( width > height ) {
    width = height;
  } else {
    height = width;
  }

   // Set the viewport to cover the new window
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset
  float fMarginX =  f_ModelW*1.0;
  float fMarginY =  f_ModelL*1.0;
  float fMarginZ =  f_ModelH*1.0;
  glOrtho( f_ModelMinX-fMarginX, f_ModelMaxX+fMarginX, f_ModelMinZ-fMarginZ, f_ModelMaxZ+fMarginZ, f_ModelMinY-fMarginY, f_ModelMaxY+fMarginY );
  //std::cout << "f_ModelMinX=" << f_ModelMinX <<", f_ModelMaxX=" << f_ModelMaxX;
  //std::cout << ", f_ModelMinY=" << f_ModelMinY << ", f_ModelMaxY=" << f_ModelMaxY;
  //std::cout << ", f_ModelMinZ=" << f_ModelMinZ << ", f_ModelMaxZ=" << f_ModelMaxZ <<"\n";
 }

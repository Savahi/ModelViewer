#include <vector>
#include <iostream>
#include <math.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

#include "Spdr3dModel.hpp"

static void displayViewer( void );
static void displayKeys( int key, int x, int y );
static void displayReshape( GLsizei width, GLsizei height );


void Spdr3dModel::display( void ) {
	int argc=0;
	char **argv=NULL;
  	glutInit(&argc,argv); // Initialize GLUT and process user parameters

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Request double buffered true color window with Z-buffer

  glutInitWindowSize(640, 480);   // Set the window's initial width & height
  glutInitWindowPosition(50, 50); // Position the window's initial top-left corner  
  glutCreateWindow("How the Building has being Built...");

  glEnable(GL_DEPTH_TEST); // Enable Z-buffer depth test

  // Callback functions
  glutDisplayFunc(displayViewer);
  glutSpecialFunc(displayKeys);
  glutReshapeFunc(displayReshape);       // Register callback handler for window re-size event

  //  Pass control to GLUT for events
  glutMainLoop();
}


static void displayViewer( void ) {
 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   //  Clear screen and Z-buffer
	
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
 
 	glLoadIdentity(); // Reset transformations
  
	glPushMatrix();
/*
	static int displayModel( Spdr3dModel& model ) {

	    for( auto& object : model ) {
	        for( auto& facet : object ) {
	            for( auto& vertex : facet ) {

	            }
	        }
	    }
	}

	glTranslatef( (constructionW+0.5)/2.0, 0.0, (constructionL+0.5)/2.0 );  
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );
	glTranslatef( -(constructionW+0.5)/2.0, 0.0, -(constructionL+0.5)/2.0  );

	glTranslatef( 0.0, bH/2.0, (constructionL+0.5)/2.0 );
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glTranslatef( 0.0, -bH/2.0, -(constructionL+0.5)/2.0 );

	for( int i = 0 ; i < constructionItemsNum ; i++ ) {
		int itemDone = constructionSteps[constructionStep][i];
		if( itemDone == 1 ) {
		  displayCube( constructionItems[i][0],constructionItems[i][1],constructionItems[i][2] );
		} else {
		  displayCubeCarcas( constructionItems[i][0],constructionItems[i][1],constructionItems[i][2] );      
		}
	}
*/
	glPopMatrix();
	glFlush();
	glutSwapBuffers(); 
}


static void displayKeys( int key, int x, int y ) {
 
  //  Right arrow - increase rotation by 5 degree
	/*
  switch(key) {
    case GLUT_KEY_RIGHT:
      rotate_y += 5;
      break;
    case GLUT_KEY_LEFT:
      rotate_y -= 5;
      break;
    case GLUT_KEY_UP:
      rotate_x += 5;
      break;
    case GLUT_KEY_DOWN:
      rotate_x -= 5;
      break;
    case GLUT_KEY_PAGE_UP:
      constructionStep = (constructionStep < constructionStepsNum-1) ? (constructionStep+1) : constructionStepsNum-1;
      break;
    case GLUT_KEY_PAGE_DOWN:
      constructionStep = (constructionStep > 0) ? (constructionStep-1) : 0;
      break;
  } 
  //  Request display update
  glutPostRedisplay();
  */
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
  //glOrtho( -constructionW*0.75, constructionW*1.5, -bH*0.75, bH*1.5, -constructionL*10.75, constructionL*10.5 );
 }

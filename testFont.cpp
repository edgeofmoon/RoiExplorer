
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include "MyVec.h"
#include "MyUiPanel.h"
#include "MyGLHeader.h"
#include "MyFont.h"		// Header for our little font library.

// This holds all the information for the font that we are going to create.
MyFont font;
MyVec4i GlobalViewport(0, 0, 1920, 1000);

void myGlutDisplay(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	// Blue text
	glColor3ub(0, 0, 0xff);
	//Here we print some text using our freetype font
	//The only really important command is the actual print() call,
	//but for the sake of making the results a bit more interesting
	//I have put in some code to rotate and scale the text.
	glPushMatrix();
	glLoadIdentity();

	font.Render(MyString("I Hope the Font Works."), MyBox2f(MyVec2f(0, 0), MyVec2f(0.5, 0.5)), 0);

	font.Render(MyString("This is a test string."), MyBox2f(MyVec2f(0, 0.5), MyVec2f(0.5, 1)), 0);

	glColor4f(1, 1, 1, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0.5, 0);
	glVertex2f(0.5, 0.5);
	glVertex2f(0, 0.5);
	glEnd();

	glPopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void myGlutReshape(int width, int height){
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void myGlutKeyboard(unsigned char key, int x, int y){
}

void myGlutMouse(int button, int state, int x, int y){
}

void myGlutMotion(int x, int y){
}

void myGlutPassiveMotion(int x, int y){
}

void myGlutMouseWheel(int button, int dir, int x, int y){
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	//font.Load("test.TTF", 16);					    //Build the freetype font
	font.Load("fonts\\Vera.ttf", 16);					    //Build the freetype font
	//font.Load("fonts\\LuckiestGuy.ttf", 16);					    //Build the freetype font

	return TRUE;										// Initialization Went OK
}

int main(int argc, char* argv[]){

	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);
	InitGL();


	MyUiPanel a;
	a.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}

// disable warnings
// disable size_t to in
// disable double to GLfloat
#pragma warning( disable : 4267 4244 )

#include <iostream>
#include <cassert>
#include <ctime>
using namespace std;

#include "MyUiPanel.h"
#include "MyVoxContainer.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"
#include "MyVolSegVoxContourTree.h"
#include "MyDataReader.h"

MyVec4i GlobalViewport(0,0,1920,1000);

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(0, 1, 1, 1);
	glutSolidSphere(0.5, 100, 100);
	glutSwapBuffers();
}

void myGlutReshape(int w, int h){
	MyUiPanel::GetViewport(GlobalViewport[0], 
		GlobalViewport[1], GlobalViewport[2], GlobalViewport[3]);
	MyGraphicsTool::SetViewport(GlobalViewport);
	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
}

void myGlutMouse(int button, int state, int x, int y){
}

void myGlutMotion(int x, int y){
}

void myGlutPassiveMotion(int x, int y){
}

int main(int argc, char* argv[]){
	
	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);

	

	MyUiPanel a;
	a.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
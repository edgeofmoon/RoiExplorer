
// disable warnings
// disable size_t to in
// disable double to GLfloat
#pragma warning( disable : 4267 4244 4305 )

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
using namespace std;

// always include
#include "MyGLHeader.h"
#include "MyApp.h"
#include "MyUiPanel.h"

MyVec4i vp;
MyApp app;

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	app.Render();

	glutSwapBuffers();
}

void myGlutReshape(int w, int h){
	MyUiPanel::GetViewport(vp[0], vp[1], vp[2], vp[3]);
	MyBox2i vpt;
	vpt.Set(MyVec2i(vp[0], vp[1]), MyVec2i(vp[0] + vp[2], vp[1] + vp[3]));
	app.Set(vpt);
	app.HandleResizeEvent();
	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
	app.HandleKeyboardEvent(key, x, vp[3] - y);
	glutPostRedisplay();
}

void myGlutMouse(int button, int state, int x, int y){
	app.HandleMouseBottonEvent(button, state, x, vp[3] - y);
	glutPostRedisplay();
}

void myGlutMouseWheel(int button, int dir, int x, int y){
	app.HandleMouseWheelEvent(button, dir, x, vp[3] - y);
	glutPostRedisplay();
}

void myGlutMotion(int x, int y){
	app.HandleMouseMoveEvent(x, vp[3] - y);
	glutPostRedisplay();
}

void myGlutPassiveMotion(int x, int y){
	app.HandleMousePassiveMotionEvent(x, vp[3] - y);
	//glutPostRedisplay();
}

int main(int argc, char* argv[]){
	app.Init();
	glutMainLoop();
	return EXIT_SUCCESS;
}
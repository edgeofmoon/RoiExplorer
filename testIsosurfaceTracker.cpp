
// disable warnings
// disable size_t to in
// disable double to GLfloat
#pragma warning( disable : 4267 4244 )

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
using namespace std;

// always include
#include "MyGLHeader.h"
#include "MyDataReader.h"
#include "MyCanvas.h"
#include "MyTrackBall.h"
#include "MyUiPanel.h"
#include "MyGraphicsTool.h"

// to be tested
#include "MyIsosurfaceTracker.h"
#include "MySurfaceRenderer.h"

MyTrackBall trackBall;
MyVec4i GlobalViewport(0, 0, 1920, 1000);
MyUiPanel uiPanel;
MyCanvas canvas;
My3dArrayfSPtr vol;
MyIsosurfaceTracker tracker;
MySurfaceRenderer surfaceRender;
float isoValue = 0.5;

void myGlutDisplay(){
	canvas.On();
	canvas.Clear();

	glPushMatrix(); {
		MyGraphicsTool::LoadTrackBall(&trackBall);
		MyGraphicsTool::Translate(-tracker.GetBoundingBox().GetCenter());
		surfaceRender.Render();
	}glPopMatrix();

	canvas.Off();

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	canvas.Show(GlobalViewport);
	glutSwapBuffers();
}

void myGlutReshape(int w, int h){
	MyUiPanel::GetViewport(GlobalViewport[0],
		GlobalViewport[1], GlobalViewport[2], GlobalViewport[3]);
	MyGraphicsTool::SetViewport(GlobalViewport);
	trackBall.Reshape(GlobalViewport[2], GlobalViewport[3]);
	MyMatrixf projectionMatrix = MyMatrixf::PerspectiveMatrix(
		45, 1, 0.01f, 1000);
	MyGraphicsTool::LoadProjectionMatrix(&projectionMatrix);
	MyGraphicsTool::LoadModelViewMatrix(&MyMatrixf::IdentityMatrix());
	gluLookAt(0, 0, 400, 0, 0, 0, 0, 1, 0);

	/***************** Resize Buffers **************/
	canvas.Resize(GlobalViewport[2], GlobalViewport[3]);
	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
	static int shaderMode = 0;
	switch (key){
	case 27:
		exit(1);
		break;
	case 'r':
	case 'R':
		int startIdx = vol->GetVolume() / 2;
		isoValue += 0.025f;
		while (vol->At(++startIdx) < isoValue);
		tracker.SetStartIndex(startIdx);
		tracker.SetIsovalue(isoValue);
		tracker.Update();
		surfaceRender.SetGeometry(tracker.GetVertices().get(),
			tracker.GetNormals().get(), tracker.GetTriangles().get());
		surfaceRender.Update();
		glutPostRedisplay();
		break;
	}
}

void myGlutMouse(int button, int state, int x, int y){
	if (state == GLUT_DOWN){
		trackBall.StartMotion(x, y);
		MyVec4i name = canvas.GetName(MyVec2i(x, GlobalViewport[3] - y));
		cout << "Name: " << name[0] << ", " << name[1]
			<< ", " << name[2] << ", " << name[3] << endl;
	}
	else{
		trackBall.EndMotion(x, y);
	}
}

void myGlutMouseWheel(int button, int dir, int x, int y){
	if (dir > 0){
		trackBall.ScaleMultiply(1.05f);
	}
	else{
		trackBall.ScaleMultiply(1 / 1.05f);
	}
	glutPostRedisplay();
}

void myGlutMotion(int x, int y){
	trackBall.RotateMotion(x, y);
	glutPostRedisplay();
}

void myGlutPassiveMotion(int x, int y){
}

int main(int argc, char* argv[]){

	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);

	//vol = MyDataReader::LoadVolumeFromFile("target_div10000.nii.gz");
	vol = MyDataReader::LoadVolumeFromFile("controlAverage.nii.gz");
	tracker.SetVolumn(vol);
	int startIdx = vol->GetVolume() / 2;
	while (vol->At(++startIdx) < isoValue);
	tracker.SetStartIndex(startIdx);
	//tracker.SetStartIndex(3310852);
	//tracker.SetIsovalue(0.650468);
	tracker.SetIsovalue(isoValue);
	tracker.Update();
	surfaceRender.SetGeometry(tracker.GetVertices().get(), 
		tracker.GetNormals().get(), tracker.GetTriangles().get());
	surfaceRender.SetName(MyVec4i(1, 2, 3, 4));
	surfaceRender.Update();

	glEnable(GL_DEPTH_TEST);
	uiPanel.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
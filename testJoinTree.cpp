
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
#include "MyDataReader.h"
#include "MyTimer.h"
#include "MyVolumeSegmenterTemplate.h"
#include "ColorScaleTable.h"
#include "MyPrimitiveDrawer.h"

// class to be test
#include "MyJoinTree.h"
#include "MyVolSegVoxContourTree.h"
#include "MyJoinTreeLayout.h"
#include "MyJoinTreeDrawer.h"

#include "MyCanvas.h"

int ROI_Index = 0;
MyUiPanel UI;
MyJoinTreeDrawer joinTreeDrawer;
MyJoinTreeLayoutSPtr layout;
MyArray<const MySegmentNode*> ROIPtr;

MyCanvas canvas;

MyVec4i GlobalViewport(0, 0, 1920, 1000);

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	canvas.On();
	canvas.Clear();


	glViewport(GlobalViewport[0], GlobalViewport[1] + GlobalViewport[3] / 2,
		GlobalViewport[2], GlobalViewport[3] / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// draw other stuff
	MyBox2f box = layout->GetTreeBox();
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	float widthBoarder = box.GetSize(0)*0.05;
	float heightBoarder = box.GetSize(1)*0.05;
	glOrtho(box.GetLowPos()[0] - widthBoarder, box.GetHighPos()[0] + widthBoarder,
		box.GetLowPos()[1] - heightBoarder, box.GetHighPos()[1] + heightBoarder, -1, 1);

	joinTreeDrawer.Render();


	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	canvas.Off();
	canvas.Show(GlobalViewport);
	glutSwapBuffers();
}

void myGlutReshape(int w, int h){
	MyUiPanel::GetViewport(GlobalViewport[0],
		GlobalViewport[1], GlobalViewport[2], GlobalViewport[3]);
	MyGraphicsTool::SetViewport(GlobalViewport);

	/***************** Resize Buffers **************/
	canvas.Resize(GlobalViewport[2], GlobalViewport[3]);

	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
}

void myGlutMouse(int button, int state, int x, int y){
}

void myGlutMotion(int x, int y){
}

void myGlutMouseWheel(int button, int dir, int x, int y){

}

void myGlutPassiveMotion(int x, int y){
}

int main(int argc, char* argv[]){

	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);

	MyTimer timer;
	timer.Restart();
	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_sub2_sub2_div10000.nii.gz");
	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("controlAverage.nii.gz");
	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_div10000.nii");

	timer.Restart();
	MyJoinTreeSPtr joinTree = make_shared<MyJoinTree>();
	MyComponentFilterSPtr filter = make_shared<MyComponentFilter>();
	filter->SetSizeThreshold(0);
	joinTree->SetVolumn(vol);
	joinTree->SetComponentFilter(filter);
	joinTree->Update();
	cout << timer.GetElapsed() << " seconds to make join tree.\n";
	timer.Restart();
	layout = make_shared<MyJoinTreeLayout>();
	layout->SetJoinTreeRoot(joinTree->GetRoot());
	layout->Update();
	cout << timer.GetElapsed() << " seconds to layout join tree.\n";
	timer.Restart();
	joinTreeDrawer.SetJoinTree(joinTree);
	joinTreeDrawer.SetJoinTreeLayout(layout);

	UI.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
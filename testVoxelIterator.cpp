#include "MyJoinTree.h"
#include "MyDataReader.h"
#include "MyGLHeader.h"
#include "MyJoinTreeDrawer.h"
#include "MyGraphicsTool.h"
#include "MyUiPanel.h"

#include <iostream>
using namespace std;

MyUiPanel UI;
MyVec4i GlobalViewport(0, 0, 1920, 1000);
MyJoinTreeDrawer joinTreeDrawer;
MyJoinTreeLayoutSPtr layout;

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void myGlutMouseWheel(int button, int dir, int x, int y){

}

void myGlutPassiveMotion(int x, int y){
}
int main(int argc, char* argv[]){
	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);

	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile(
		"target_sub2_sub2_div10000.nii.gz");
	MyJoinTreeSPtr joinTree = make_shared<MyJoinTree>();
	MyComponentFilterSPtr filter = make_shared<MyComponentFilter>();
	filter->SetSizeThreshold(20);
	joinTree->SetVolumn(vol);
	joinTree->SetComponentFilter(filter);
	joinTree->Update();
	layout = make_shared<MyJoinTreeLayout>();
	layout->SetJoinTreeRoot(joinTree->GetRoot());
	layout->Update();
	joinTreeDrawer.SetJoinTree(joinTree);
	joinTreeDrawer.SetJoinTreeLayout(layout);
	const MySegmentNode* joinTreeRoot = joinTree->GetRoot().get();

	// pre-test
	cout << "volumn: " << vol->GetVolume() << endl;
	MyArray<const MySegmentNode*> nodeStack;
	nodeStack << joinTreeRoot;
	int numVoxels = 0; 
	while (!nodeStack.empty()){
		const MySegmentNode* current = nodeStack.back();
		nodeStack.pop_back();
		for (int i = 0; i < current->GetNumChildren(); i++){
			nodeStack << current->GetChild(i).get();
		}
		numVoxels += current->GetUniqueVoxes()->GetNumVoxes();
	}
	cout << "numVoxels: " << numVoxels << endl;

	// begin test
	MySegmentNode::const_VoxelIterator voxItr = joinTreeRoot->VoxelBegin();
	MySegmentNode::const_VoxelIterator voxEnd = joinTreeRoot->VoxelEnd();
	MyArrayi voxIndices;
	MyArrayf voxValues;
	//voxIndices.reserve(this->GetNumTotalVoxes());
	while (voxItr != voxEnd){
		voxIndices << voxItr.GetVoxelIndex();
		voxValues << *voxItr;
		if (voxIndices.size() == 79548){
			int debug = 1;
		}
		++voxItr;
	}
	cout << "numVoxels: " << voxIndices.size() << endl;

	UI.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
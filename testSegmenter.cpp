
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

MyVec4i GlobalViewport(0, 0, 1920, 1000);

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

void myGlutMouseWheel(int button, int dir, int x, int y){

}

void myGlutPassiveMotion(int x, int y){
}

int main(int argc, char* argv[]){

	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);


	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("controlAverage.nii.gz");
	MyVolSegVoxContourTree segmenter;
	segmenter.SetVolumeData(vol);
	time_t thisTime, thatTime;
	time(&thatTime);
	MyArraySPtr<MyVoxContainerfSPtr> segs = segmenter.MakeSegments();
	time(&thisTime);
	cout << difftime(thisTime, thatTime) << " seconds to make segments\n";

	time(&thatTime);
	int numVoxes = 0;
	float sampleSum = 0;
	vector< bool> voxMap(vol->GetVolume(), false);
	for (int i = 0; i < segs->size(); i++){
		if (segs->at(i) == 0) continue;
		if (segs->at(i)->GetNumVoxes() == 0){
			cout << "empty container.\n";
		}
		numVoxes += segs->at(i)->GetNumVoxes();
		MyVoxContainerf::Iterator itr = segs->at(i)->Begin();
		MyVoxContainerf::Iterator itrEnd = segs->at(i)->End();
		while (itr != itrEnd){
			if (voxMap[itr.GetVoxelIndex()]){
				cout << itr.GetVoxelIndex() << " duplicated.\n";
			}
			if (itr.GetVoxelIndex() >= vol->GetVolume()){
				cout << itr.GetVoxelIndex() << " out of range.\n";
			}
			if (*itr != vol->At(itr.GetVoxelIndex())){
				cout << "wrong voxel index.\n";
			}
			sampleSum += *itr;
			voxMap[itr.GetVoxelIndex()] = true;
			itr++;
		}
	}
	time(&thisTime);
	cout << "Segmentations have in total: " << numVoxes << " voxes." << endl;
	cout << "Segmentations sample sum " << sampleSum << endl;
	cout << "Average sample value " << sampleSum / numVoxes << endl;
	cout << difftime(thisTime, thatTime) << " seconds to check segments\n";

	MyUiPanel a;
	a.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
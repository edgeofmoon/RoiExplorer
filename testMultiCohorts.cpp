
// disable warnings
// disable size_t to in
// disable double to GLfloat
#pragma warning( disable : 4267 4244 )

#define DSR_X 2
#define DSR_Y 2

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
#include "MyJoinTreeROI.h"
#include "MyVolSegVoxContourTree.h"
#include "MyJoinTreeLayout.h"
#include "MyJoinTreeDrawer.h"
#include "MySegmentNodeMatchCounter.h"
#include "MyBloomDrawingHelper.h"
#include "MyMarchingSquares.h"
#include "OSCB.h"
#include "MyCanvas.h"
#include "MySegNodeInfoLayout2D.h"
#include "MySegsPlanarDrawer.h"
#include "MyLineConnectorDrawer.h"

int ROI_Index = 0;
MyUiPanel UI;
MyJoinTreeDrawer joinTreeDrawer;
MyJoinTreeLayoutSPtr layout;
MyArray<const MySegmentNode*> ROIPtr;
MyBloomDrawingHelper helper;
MySegmentNodeMatchCounterSPtr match;
MyCanvas canvas;
MySegNodeInfoLayout2DSPtr seglayout;
MySegsPlanarDrawer segsDrawer;
MyLineConnectorDrawer connectDrawer;

MyVec4i GlobalViewport(0, 0, 1920, 1000);

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
	canvas.On();
	canvas.Clear();

	// draw other stuff
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-0.05, 1.05, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	helper.Render(GlobalViewport[2], GlobalViewport[3]);
	joinTreeDrawer.Render(GlobalViewport[2], GlobalViewport[3]);

	connectDrawer.Render(GlobalViewport[2], GlobalViewport[3]);

	glTranslatef(0, -1, 0);
	segsDrawer.SetLinkDrawThreshold(UI.UI_linkDrawThreshold);
	segsDrawer.Update();
	segsDrawer.Render(GlobalViewport[2], GlobalViewport[3]);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	canvas.Off();
	canvas.Show(GlobalViewport);
	glutSwapBuffers();
}

void myGlutReshape(int w, int h){
	MyUiPanel::GetViewport(GlobalViewport[0],
		GlobalViewport[1], GlobalViewport[2], GlobalViewport[3]);
	GlobalViewport[2] *= DSR_X;
	GlobalViewport[3] *= DSR_Y;
	MyGraphicsTool::SetViewport(GlobalViewport);

	/***************** Resize Buffers **************/
	canvas.Resize(GlobalViewport[2] * DSR_X, GlobalViewport[3] * DSR_Y);
	segsDrawer.Resize(GlobalViewport[2] * DSR_X, GlobalViewport[3] * DSR_Y);

	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
}

void myGlutMouse(int button, int state, int x, int y){
	MyVec4i name = canvas.GetName(x*DSR_X, GlobalViewport[3] - y*DSR_Y);
	cout << "Name: " << name[0] << ", " << name[1]
		<< ", " << name[2] << ", " << name[3] << endl;
	if (name[0] >= 0){
		ROI_Index = name[1];
	}
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


	My3dArrayfSPtr labelVolRaw = MyDataReader::LoadVolumeFromFile("JHU-WhiteMatter-labels-1mm.nii");
	My3dArrayfSPtr labelVol;
	labelVol = labelVolRaw;
	//int sizeReduce = 4;
	int sizeReduce = 1;
	if (sizeReduce != 1){
		MyVec3i volSize = labelVolRaw->GetDimSizes();
		volSize /= 4;
		volSize += MyVec3i(1, 1, 1);
		labelVol = std::make_shared<My3dArrayf>(volSize);

		for (int i = 0; i < volSize[0]; i++){
			for (int j = 0; j < volSize[1]; j++){
				for (int k = 0; k < volSize[2]; k++){
					MyVec3f uniformPos = MyVec3f(i / (float)(volSize[0] - 1),
						j / (float)(volSize[1] - 1), k / (float)(volSize[2] - 1));
					float value = labelVolRaw->NearestUniform(uniformPos);
					labelVol->operator[](MyVec3i(i, j, k)) = value;
				}
			}
		}
	}

	MyMapSPtr<const MySegmentNode*, MyVec4f> ROI_color
		= make_shared<MyMap<const MySegmentNode*, MyVec4f>>();
	MyVolumeSegmenterTemplate segmenter;
	segmenter.SetVolumeData(labelVol);
	MyArraySPtr<MySegmentNodeSPtr> ROIs = segmenter.MakeSegments();
	for (int i = 0; i < ROIs->size(); i++){
		ROIPtr.push_back(ROIs->at(i).get());
		float rgba[4];
		ColorScaleTable::SequentialColor(i, 0, ROIs->size() - 1, rgba);
		//ColorScaleTable::CategoricalColor(i, 0, ROIs->size() - 1, rgba);
		ROI_color->operator[](ROIs->at(i).get()) = MyVec4f(rgba);
	}
	cout << timer.GetElapsed() << " seconds to read ROIs.\n";
	timer.Restart();

	// add old other stuff
	MyMapSPtr<int, MyString> labels = MyDataReader::LoadRegionLabel("GOBS_look_up_table.txt");

	// control cohort
	//string folderStr = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\control\\";
	string folderStr = "..\\..\\tmpdata\\skeletons\\control\\";
	MySegNodeInfoAssembleSPtr segAsmb_control 
		= MyDataReader::ConstructAssembleFromDirectory(folderStr.c_str(), ROIs.get());
	cout << timer.GetElapsed() << " seconds to construct " 
		<< segAsmb_control->GetSegmentNodeInfos()->front()->GetVolumes()->size() << " control cohort\n";
	timer.Restart();


	//string folderStr1 = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\scz\\";
	string folderStr1 = "..\\..\\tmpdata\\skeletons\\scz\\";
	MySegNodeInfoAssembleSPtr segAsmb_scz
		= MyDataReader::ConstructAssembleFromDirectory(folderStr1.c_str(), ROIs.get());
	cout << timer.GetElapsed() << " seconds to construct "
		<< segAsmb_scz->GetSegmentNodeInfos()->front()->GetVolumes()->size() << " scz cohort\n";
	timer.Restart();

	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ctr_10.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ACR.trk");
	MyTracksSPtr tracks = std::make_shared<MyTracks>("..\\..\\tmpdata\\ACR.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\dti.trk");
	cout << timer.GetElapsed() << " seconds to read tracks.\n";
	timer.Restart();
	MySegTrkNetworkSPtr network = std::make_shared<MySegTrkNetwork>();
	network->SetTracks(tracks);
	network->SetSegmentNodeInfos(segAsmb_control->GetSegmentNodeInfos());
	network->Update();
	cout << timer.GetElapsed() << " seconds to compute connections.\n";
	timer.Restart();

	MySegmentAssembleGroupSPtr asmGroup = make_shared<MySegmentAssembleGroup>();
	asmGroup->PushBack(segAsmb_control);
	asmGroup->PushBack(segAsmb_scz);
	asmGroup->Update();
	seglayout = std::make_shared<MySegNodeInfoLayout2D>();
	seglayout->SetBoxesIn(segAsmb_control->GetSegment2DBoxes());
	seglayout->SetSegmentAssembleGroup(asmGroup);
	seglayout->Update();
	segsDrawer.SetLayout(seglayout);
	segsDrawer.SetSegmentAssembleGroup(asmGroup);
	segsDrawer.SetSegTrkNetwork(network);
	segsDrawer.SetLabels(labels);
	segsDrawer.CompileShader();
	segsDrawer.SetSegNodeColor(ROI_color);
	segsDrawer.Update();

	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_sub2_sub2_div10000.nii.gz");
	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("controlAverage.nii.gz");
	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_div10000.nii");

	MyJoinTreeSPtr joinTree = make_shared<MyJoinTree>();
	MyComponentFilterSPtr filter = make_shared<MyComponentFilter>();
	filter->SetSizeThreshold(20);
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

	match = make_shared<MySegmentNodeMatchCounter>();
	match->AddGroupByArray(&ROIPtr);
	match->AddGroupByTree(joinTree->GetRoot().get());
	match->Update();
	cout << timer.GetElapsed() << " seconds to compute match.\n";

	helper.SetLayout(layout);
	helper.SetMatchCounter(match);
	helper.SetSegmentColor(ROI_color);

	connectDrawer.SetJoinTreeLayout(layout);
	connectDrawer.SetRoiLayout(seglayout);
	connectDrawer.SetMatchCounter(match);
	connectDrawer.SetJoinTreeLayoutOffset(MyVec2f(0, 0));
	connectDrawer.SetRoiLayoutOffset(MyVec2f(0, -1));
	connectDrawer.SetRoiColors(ROI_color);

	//glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);

	UI.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
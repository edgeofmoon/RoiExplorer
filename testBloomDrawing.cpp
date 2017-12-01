
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
#include "MySegmentNodeMatchCounter.h"
#include "MyBloomDrawingHelper.h"
#include "MyMarchingSquares.h"
#include "OSCB.h"
#include "MyCanvas.h"
#include "MySegsPlanarDrawer.h"

int ROI_Index = 0;
MyUiPanel UI;
MyJoinTreeDrawer joinTreeDrawer;
MyJoinTreeLayoutSPtr layout;
MyArray<const MySegmentNode*> ROIPtr;
MyBloomDrawingHelper helper;

MyCanvas canvas;
MySegsPlanarDrawer segsDrawer;

MyVec4i GlobalViewport(0, 0, 1920, 1000);

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
	canvas.On();
	canvas.Clear();

	/*
	glViewport(GlobalViewport[0], GlobalViewport[1] + GlobalViewport[3] / 2,
		GlobalViewport[2], GlobalViewport[3] / 2);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	MyBox2f box = layout->GetTreeBox();
	float widthBoarder = box.GetSize(0)*0.05;
	float heightBoarder = box.GetSize(1)*0.05;
	glOrtho(box.GetLowPos()[0] - widthBoarder, box.GetHighPos()[0] + widthBoarder,
		box.GetLowPos()[1] - heightBoarder, box.GetHighPos()[1] + heightBoarder, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	helper.Render(GlobalViewport[2], GlobalViewport[3] / 2);
	joinTreeDrawer.Render(GlobalViewport[2], GlobalViewport[3] / 2);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	*/



	glViewport(GlobalViewport[0], GlobalViewport[1],
		GlobalViewport[2], GlobalViewport[3] / 2);
	glPushMatrix(); {
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, -1, 1);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); {
			glLoadIdentity();
			segsDrawer.SetLinkDrawThreshold(UI.UI_linkDrawThreshold);
			segsDrawer.Update();
			segsDrawer.Render(GlobalViewport[2] / 2, GlobalViewport[3] / 2);
		}
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}glPopMatrix();

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
	segsDrawer.Resize(GlobalViewport[2], GlobalViewport[3] / 2);

	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
}

void myGlutMouse(int button, int state, int x, int y){
	MyVec4i name = canvas.GetName(x, GlobalViewport[3] - y);
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
	timer.Restart();
	MySegNodeInfoAssembleSPtr segAsmb = std::make_shared<MySegNodeInfoAssemble>();
	MyMapSPtr<int, MyString> labels = MyDataReader::LoadRegionLabel("GOBS_look_up_table.txt");
	MyArraySPtr<My3dArrayfScPtr> vols = std::make_shared<MyArray<My3dArrayfScPtr>>();
	//string folderStr = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\";
	string folderStr = "..\\..\\tmpdata\\skeletons\\";
	vector<string> skeletonfiles = get_all_files_names_within_folder(folderStr);
	for (int i = 0; i < skeletonfiles.size(); i++){
		My3dArrayfSPtr ske = MyDataReader::LoadVolumeFromFile((folderStr + skeletonfiles[i]).c_str());
		vols->PushBack(ske);
	}
	cout << timer.GetElapsed() << " seconds to read " << vols->size() << " subjects volumes.\n";

	for (int i = 0; i < ROIs->size(); i++){
		MySegmentNodeInfoSPtr segInfo = std::make_shared<MySegmentNodeInfo>();
		segInfo->SetSegmentNode(ROIs->at(i));
		segInfo->SetVolumes(vols);
		segInfo->Update();
		segAsmb->AddSegmentNodeInfo(segInfo);
	}
	segAsmb->Update();
	cout << timer.GetElapsed() << " seconds to encode " << ROIs->size() << " segments\n";
	timer.Restart();

	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ctr_10.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ACR.trk");
	MyTracksSPtr tracks = std::make_shared<MyTracks>("..\\..\\tmpdata\\ACR.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\dti.trk");
	cout << timer.GetElapsed() << " seconds to read tracks.\n";
	timer.Restart();
	MySegTrkNetworkSPtr network = std::make_shared<MySegTrkNetwork>();
	network->SetTracks(tracks);
	network->SetSegmentNodeInfos(segAsmb->GetSegmentNodeInfos());
	network->Update();
	cout << timer.GetElapsed() << " seconds to compute connections.\n";
	timer.Restart();
	//segsRenderer.SetSegmentNodeInfoAssemble(segAsmb);
	//segsRenderer.CompileShader();
	//segsRenderer.Update();
	segsDrawer.AddSegmentNodeInfoAssemble(segAsmb);
	segsDrawer.SetSegTrkNetwork(network);
	segsDrawer.SetLabels(labels);
	segsDrawer.CompileShader();
	segsDrawer.SetSegNodeColor(ROI_color);
	segsDrawer.Update();
	/*
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

	MySegmentNodeMatchCounterSPtr match = make_shared<MySegmentNodeMatchCounter>();
	match->AddGroupByArray(&ROIPtr);
	match->AddGroupByTree(joinTree->GetRoot().get());
	match->Update();
	cout << timer.GetElapsed() << " seconds to compute match.\n";

	helper.SetLayout(layout);
	helper.SetMatchCounter(match);
	helper.SetSegmentColor(ROI_color);
	*/

	UI.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
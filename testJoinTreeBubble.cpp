
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
#include "MySegmentNodeMatch.h"
#include "MyBubbleSetsDrawingHelper.h"
#include "MyMarchingSquares.h"

#include "MyCanvas.h"
#include "MyCohortPlot.h"
#include "OSCB.h"
#include "MySegsVolRenderer.h"
#include "MySegsPlanarDrawer.h"
#include "MySegTrkNetwork.h"

int ROI_Index = 0;
MyUiPanel UI;
MySegmentNodeMatch match;
MyJoinTreeDrawer joinTreeDrawer;
MyJoinTreeLayoutSPtr layout;
MyArray<const MySegmentNode*> ROIPtr;

MyCanvas canvas;
MySegTrkNetworkSPtr network;
MyTracksSPtr tracks;
MySegsVolRenderer segsRenderer;
MySegsPlanarDrawer segsDrawer;
MyCohortPlot cohortPlot;

MyVec4i GlobalViewport(0, 0, 1920, 1000);

void myGlutDisplay(){
	glClearColor(1, 1, 1, 0);
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
	MyBubbleSetsDrawingHelper helper;
	helper.SetLayout(layout);
	helper.SetMaxEnergyRadius(UI.UI_bubbleMaxRadius);
	helper.SetCoreEnergyRadius(UI.UI_bubbleCoreRadius);
	cout << "Max Radius: " << UI.UI_bubbleMaxRadius << endl;
	cout << "Core Radius: " << UI.UI_bubbleCoreRadius << endl;
	helper.Update();

	// try the grid
	// tmp sol
	int roiidx = ROI_Index;
	if (roiidx < 0) roiidx = 0;
	if (roiidx >= ROIPtr.size() - 1) roiidx = ROIPtr.size() - 1;
	/*
	MyBox2f bbox = helper.ComputeBoundingBox(&(match.GetNeighbors(ROIPtr[roiidx], MyVec2i(0, 1))));
	glColor4f(1, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(bbox.GetLowPos()[0], bbox.GetLowPos()[1], 0.5);
	glVertex3f(bbox.GetHighPos()[0], bbox.GetLowPos()[1], 0.5);
	glVertex3f(bbox.GetHighPos()[0], bbox.GetHighPos()[1], 0.5);
	glVertex3f(bbox.GetLowPos()[0], bbox.GetHighPos()[1], 0.5);
	glEnd();
	glRasterPos3f(bbox.GetHighPos()[0] + bbox.GetSize(0)*0.01, bbox.GetHighPos()[1], 0.7);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)MyString(roiidx).c_str());
	*/
	MyBox1f range(MyVec1f(FLT_MAX), MyVec1f(-FLT_MAX));
	My2dfGridfSPtr field = helper.ComputeEnergyField(&(match.GetNeighbors(ROIPtr[roiidx], MyVec2i(0, 1))));
	for (int i = 0; i < field->GetDimSize(0); i++){
		for (int j = 0; j < field->GetDimSize(1); j++){
			MyVec2i index(i, j);
			float value = field->At(index);
			range.Engulf(value);
		}
	}
	float threshold = (UI.UI_bubbleThreshold
		*range.GetHighPos()[0] - range.GetLowPos()[0]) + range.GetLowPos()[0];
	cout << "Threshold: " << threshold << endl;
	for (int i = 0; i < field->GetDimSize(0); i++){
		for (int j = 0; j < field->GetDimSize(1); j++){
			MyVec2i index(i, j);
			float value = field->At(index);
			MyBox2f box = field->ComputeCell(index);
			float color[4] = { 0.8, 0.3, 0.3, 1 };
			ColorScaleTable::DiffValueToColor(value, range.GetLowPos()[0], range.GetHighPos()[0], color);
			glColor4fv(color);
			//glBegin(GL_LINE_LOOP);
			glBegin(GL_QUADS);
			glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], 0.5);
			glVertex3f(box.GetHighPos()[0], box.GetLowPos()[1], 0.5);
			glVertex3f(box.GetHighPos()[0], box.GetHighPos()[1], 0.5);
			glVertex3f(box.GetLowPos()[0], box.GetHighPos()[1], 0.5);
			glEnd();
		}
	}

	// test the marching squares
	glColor4f(0.9, 0.1, 0.1, 1);
	glLineWidth(3);
	MyMarchingSquares mq;
	mq.SetField(field);
	mq.SetThreshold(threshold);
	mq.Update();
	const MyArray<MyLine2f>& lines = mq.GetLines();
	for (int iLine = 0; iLine < lines.size(); iLine++){
		MyPrimitiveDrawer::DrawLine(lines[iLine]);
	}
	glLineWidth(1);

	joinTreeDrawer.Render(GlobalViewport[2], GlobalViewport[3]);


	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

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
	segsRenderer.Resize(GlobalViewport[2] / 2, GlobalViewport[3]);
	segsDrawer.Resize(GlobalViewport[2] / 2, GlobalViewport[3] / 2);
	canvas.Resize(GlobalViewport[2], GlobalViewport[3]);

	glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y){
}

void myGlutMouse(int button, int state, int x, int y){
	MyVec4i name = canvas.GetName(x, GlobalViewport[3] - y);
	cout << "Name: " << name[0] << ", " << name[1]
		<< ", " << name[2] << ", " << name[3] << endl;
	if (name[0] >= 0){
		MyArrayiScPtr indices0 = network->GetSegmentTrackIndices()->at(name[0]);
		if (name[1] == name[0]){
			tracks->SetFiberToDraw(indices0.get());
		}
		else{
			MyArrayiScPtr indices1 = network->GetSegmentTrackIndices()->at(name[1]);
			MyArrayi* common = MyArrayi::MakeCommonElementArray(*(indices0.get()), *(indices1.get()));
			tracks->SetFiberToDraw(common);
			delete common;
		}
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
	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_sub2_sub2_div10000.nii.gz");
	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("controlAverage.nii.gz");
	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_div10000.nii");

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

	MyVolumeSegmenterTemplate segmenter;
	segmenter.SetVolumeData(labelVol);
	MyArraySPtr<MySegmentNodeSPtr> ROIs = segmenter.MakeSegments();
	for (int i = 0; i < ROIs->size(); i++){
		ROIPtr.push_back(ROIs->at(i).get());
	}
	cout << timer.GetElapsed() << " seconds to read ROIs.\n";
	timer.Restart();

	/*
	MyVolSegVoxContourTree segmenter;
	//MyVolumeSegmenterTemplate segmenter;
	segmenter.SetVolumeData(vol);
	timer.Restart();
	MyArraySPtr<MySegmentNodeSPtr> segs = segmenter.MakeSegments();
	cout << timer.GetElapsed() << " seconds to make segments\n";
	*/

	timer.Restart();
	MyJoinTreeSPtr joinTree = make_shared<MyJoinTree>();
	MyComponentFilterSPtr filter = make_shared<MyComponentFilter>();
	filter->SetSizeThreshold(30);
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

	match.AddGroupByArray(&ROIPtr);
	match.AddGroupByTree(joinTree->GetRoot().get());
	match.Update();
	cout << timer.GetElapsed() << " seconds to compute match.\n";
	timer.Restart();

	// ROI plot
	MySegNodeInfoAssembleSPtr segAsmb = std::make_shared<MySegNodeInfoAssemble>();	MyMapSPtr<int, MyString> labels = MyDataReader::LoadRegionLabel("GOBS_look_up_table.txt");
	MyArraySPtr<My3dArrayfScPtr> vols = std::make_shared<MyArray<My3dArrayfScPtr>>();
	//string folderStr = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\";
	string folderStr = "..\\..\\tmpdata\\skeletons\\";
	vector<string> skeletonfiles = get_all_files_names_within_folder(folderStr);
	for (int i = 0; i < skeletonfiles.size(); i++){
		My3dArrayfSPtr ske = MyDataReader::LoadVolumeFromFile((folderStr + skeletonfiles[i]).c_str());
		vols->PushBack(ske);
	}
	cout << timer.GetElapsed() << " seconds to read " << vols->size() << " subjects volumes.\n";
	timer.Restart();
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
	//tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ACR.trk");
	tracks = std::make_shared<MyTracks>("..\\..\\tmpdata\\ACR.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\dti.trk");
	cout << timer.GetElapsed() << " seconds to read tracks.\n";
	timer.Restart();
	network = std::make_shared<MySegTrkNetwork>();
	network->SetTracks(tracks);
	network->SetSegmentNodeInfos(segAsmb->GetSegmentNodeInfos());
	network->Update();
	cout << timer.GetElapsed() << " seconds to compute connections.\n";
	timer.Restart();
	//segsRenderer.SetSegmentNodeInfoAssemble(segAsmb);
	//segsRenderer.CompileShader();
	//segsRenderer.Update();
	segsDrawer.SetSegmentNodeInfoAssemble(segAsmb);
	segsDrawer.SetSegTrkNetwork(network);
	segsDrawer.SetLabels(labels);
	segsDrawer.CompileShader();
	segsDrawer.Update();
	cohortPlot.SetSegmentNodeInfoAssemble(segAsmb);
	cohortPlot.Update();
	//tracks->ComputeGeometry();
	//tracks->LoadShader();
	//tracks->LoadGeometry();

	UI.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
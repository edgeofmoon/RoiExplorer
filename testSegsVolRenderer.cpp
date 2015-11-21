
// disable warnings
// disable size_t to in
// disable double to GLfloat
#pragma warning( disable : 4267 4244 )

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
using namespace std;

#include "MyUiPanel.h"
#include "MyVoxContainer.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"
#include "MyVolSegVoxContourTree.h"
#include "MyVolumeSegmenterTemplate.h"
#include "MyDataReader.h"
#include "MyTrackBall.h"
#include "MySegsVolRenderer.h"
#include "MySegsPlanarDrawer.h"
#include "MySegTrkNetwork.h"
#include "MyTimer.h"
#include "MyCanvas.h"
#include "MyMathHelper.h"
#include "OSCB.h"
#include "MyCohortPlot.h"

MyTimer timer;
MyTrackBall trackBall;
MyVec4i GlobalViewport(0, 0, 1920, 1000);
MyUiPanel uiPanel;
MyCanvas canvas;
MySegTrkNetworkSPtr network;
MyTracksSPtr tracks;
MySegsVolRenderer segsRenderer;
MySegsPlanarDrawer segsDrawer;
MyCohortPlot cohortPlot;

void myGlutDisplay(){
	canvas.On();
	canvas.Clear();
	
	glViewport(GlobalViewport[0], GlobalViewport[1], 
		GlobalViewport[2]/2, GlobalViewport[3]);
	glPushMatrix(); {
		MyGraphicsTool::LoadTrackBall(&trackBall);
		segsRenderer.SetDensityThreshold(uiPanel.UI_trackDensityFilter);
		segsRenderer.SetSampleRate(uiPanel.UI_sampleRate);
		segsRenderer.SetDecayFactor(uiPanel.UI_decayFactor);
		segsRenderer.Render(GlobalViewport[2]/2, GlobalViewport[3]);

		glTranslatef(-182 / 2, -218 / 2, -182 / 2);
		tracks->Show();
	}glPopMatrix();
	
	glViewport(GlobalViewport[2]/2, GlobalViewport[1],
		GlobalViewport[2] / 2, GlobalViewport[3] / 2);
	glPushMatrix(); {
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, -1, 1);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); {
			glLoadIdentity();
			segsDrawer.SetLinkDrawThreshold(uiPanel.UI_linkDrawThreshold);
			segsDrawer.Update();
			segsDrawer.Render(GlobalViewport[2] / 2, GlobalViewport[3] / 2);
		}
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}glPopMatrix();

	glViewport(GlobalViewport[2] / 2, GlobalViewport[3] / 2,
		GlobalViewport[2] / 2, GlobalViewport[3] / 2);
	glPushMatrix(); {
		glLoadIdentity();
		glOrtho(-0.02, 1.02, -0.02, 1.02, -1, 1);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); {
			glLoadIdentity();
			cohortPlot.Render(GlobalViewport[2] / 2, GlobalViewport[3] / 2);
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
	trackBall.Reshape(GlobalViewport[2], GlobalViewport[3]);
	//MyMatrixf projectionMatrix = MyMatrixf::PerspectiveMatrix(
	//	45, GlobalViewport[2] / (float)GlobalViewport[3], 0.01f, 1000);
	MyMatrixf projectionMatrix = MyMatrixf::PerspectiveMatrix(
		45, 1, 0.01f, 1000);
	MyGraphicsTool::LoadProjectionMatrix(&projectionMatrix);
	MyGraphicsTool::LoadModelViewMatrix(&MyMatrixf::IdentityMatrix());
	gluLookAt(0, 0, 400, 0, 0, 0, 0, 1, 0);

	/***************** Resize Buffers **************/
	segsRenderer.Resize(GlobalViewport[2] / 2, GlobalViewport[3]);
	segsDrawer.Resize(GlobalViewport[2] / 2, GlobalViewport[3] / 2);
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
		segsRenderer.CompileShader(shaderMode = 1 - shaderMode);
		segsDrawer.CompileShader(shaderMode = 1 - shaderMode);
		glutPostRedisplay();
		break;
	}
}

void myGlutMouse(int button, int state, int x, int y){
	if (x < GlobalViewport[2] / 2){
		if (state == GLUT_DOWN){
			trackBall.StartMotion(x, y);
		}
		else{
			trackBall.EndMotion(x, y);
		}
	}
	else{
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
		}
	}
}

void myGlutMouseWheel(int button, int dir, int x, int y){
	if (dir > 0){
		trackBall.ScaleMultiply(1.05f);
	}
	else{
		trackBall.ScaleMultiply(1/1.05f);
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

	My3dArrayfSPtr tempVol = MyDataReader::LoadVolumeFromFile("JHU-WhiteMatter-labels-1mm.nii");
	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_div10000.nii.gz");

	MyMapSPtr<int, MyString> labels = MyDataReader::LoadRegionLabel("GOBS_look_up_table.txt");
	MyArraySPtr<My3dArrayfScPtr> vols = std::make_shared<MyArray<My3dArrayfScPtr>>();
	string folderStr = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\";
	vector<string> skeletonfiles = get_all_files_names_within_folder(folderStr);
	for (int i = 0; i < skeletonfiles.size(); i++){
		My3dArrayfSPtr ske = MyDataReader::LoadVolumeFromFile((folderStr+skeletonfiles[i]).c_str());
		vols->PushBack(ske);
	}
	//vols->PushBack(vol);
	MyVolSegVoxContourTree segmenter;
	//MyVolumeSegmenterTemplate segmenter;
	segmenter.SetVolumeData(tempVol);
	timer.Restart();
	MyArraySPtr<MySegmentNodeSPtr> segs = segmenter.MakeSegments();
	cout << timer.GetElapsed() << " seconds to make segments\n";

	/*
	// projection
	for (int i = 0; i < segs->size(); i++){
		if (segs->at(i)->GetIndex() == 5){
			int numVox = segs->at(i)->GetNumTotalVoxes();
			MySegmentNode::VoxelIterator itr = segs->at(i)->VoxelBegin();
			MySegmentNode::VoxelIterator itrEnd = segs->at(i)->VoxelEnd();
			MyVec3i size = segs->at(i)->GetVolumeSize();
			MyMatrixf dataMat(numVox, 3);
			int iVox = 0;
			while (itr != itrEnd){
				MyVec3i pos = My3dArrayf::ComputePosition(itr.GetVoxelIndex(), size);
				dataMat.At(iVox, 0) = pos[0];
				dataMat.At(iVox, 1) = pos[1];
				dataMat.At(iVox, 2) = pos[2];
				itr++;
				iVox++;
			}
			MyMatrixf* rstMat = MyMathHelper::PCA_Projection(&dataMat, 2);

			ofstream output("rstProj.txt");
			for (int ir = 0; ir < rstMat->GetNumRows(); ir++){
				for (int ic = 0; ic < rstMat->GetNumCols(); ic++){
					output << rstMat->At(ir, ic) << ' ';
				}
				output << endl;
			}
			return 1;
		}
	}
	// end projection
	*/

	MySegNodeInfoAssembleSPtr segAsmb = std::make_shared<MySegNodeInfoAssemble>();
	timer.Restart();
	for (int i = 0; i < segs->size(); i++){
		MySegmentNodeInfoSPtr segInfo = std::make_shared<MySegmentNodeInfo>();
		segInfo->SetSegmentNode(segs->at(i));
		segInfo->SetVolumes(vols);
		segInfo->Update();
		segAsmb->AddSegmentNodeInfo(segInfo);
	}
	segAsmb->Update();
	cout << timer.GetElapsed() << " seconds to encode " << segs->size() << " segments\n";
	timer.Restart();
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ctr_10.trk");
	tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ACR.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\dti.trk");
	network = std::make_shared<MySegTrkNetwork>();
	network->SetTracks(tracks);
	network->SetSegmentNodeInfos(segAsmb->GetSegmentNodeInfos());
	network->Update();
	cout << timer.GetElapsed() << " seconds to compute connections.\n";
	timer.Restart();
	segsRenderer.SetSegmentNodeInfoAssemble(segAsmb);
	segsRenderer.CompileShader();
	segsRenderer.Update();
	segsDrawer.SetSegmentNodeInfoAssemble(segAsmb);
	segsDrawer.SetSegTrkNetwork(network);
	segsDrawer.SetLabels(labels);
	segsDrawer.CompileShader();
	segsDrawer.Update();
	cohortPlot.SetSegmentNodeInfoAssemble(segAsmb);
	cohortPlot.Update();
	tracks->ComputeGeometry();
	tracks->LoadShader();
	tracks->LoadGeometry();

	cout << timer.GetElapsed() << " seconds to generate " << segs->size() << " renderables\n";
	uiPanel.AddUIs();
	glutMainLoop();
	return EXIT_SUCCESS;
}
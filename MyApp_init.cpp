#include "MyApp.h"

// always include
#include "MyGLHeader.h"
#include "MyDataReader.h"
#include "MyCanvas.h"
#include "MyTrackBall.h"
#include "MyUiPanel.h"
#include "MyGraphicsTool.h"
#include "MyArrayMD.h"
#include "MyTimer.h"

// general
#include "OSCB.h"
#include "MyFont.h"

// 0. spatial view
#include "MyIsosurfaceTracker.h"
#include "MySurfaceRenderer.h"
#include "MySpatialView.h"
#include "MyMesh.h"
#include "Shader.h"

// 1. joinTree view
#include "MyJoinTree.h"
#include "MyJoinTreeLayout.h"
#include "MyJoinTreeDrawer.h"
#include "MyJoinTreeView.h"

// 2. roi view
#include "MyVolumeSegmenterTemplate.h"
#include "MySegmentNodeMatchCounter.h"
#include "MyBloomDrawingHelper.h"
#include "MySegsPlanarDrawer.h"
#include "MyRoiView.h"

// 3. connector view
#include "MyLineConnectorDrawer.h"
#include "MyConnectorView.h"

// STL
#include <iostream>
using namespace std;

void MyApp::Init(){
	mAsmbVolLimit = INT_MAX;
	//mAsmbVolLimit = 3;
	MyVec4i GlobalViewport(0, 0, 1920, 1000);
	float isoValue = 0.5;
	MyTimer timer;

	MyUiPanel::InitGL(GlobalViewport[2], GlobalViewport[3]);
	// font
	MyFontSPtr font = std::make_shared<MyFont>();
	//font->Load("fonts\\Vera.ttf", 16);
	font->Load("fonts\\Sansita-Regular.otf", 16);
	// Roi
	My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("target_div10000.nii.gz");
	//My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile("controlAverage.nii.gz");

	My3dArrayfSPtr labelVol = MyDataReader::LoadVolumeFromFile("JHU-WhiteMatter-labels-1mm.nii");
	MyVolumeSegmenterTemplate segmenter;
	segmenter.SetVolumeData(labelVol);
	MyArraySPtr<MySegmentNodeSPtr> ROIs = segmenter.MakeSegments();
	//ROIs->resize(5);
	cout << timer.GetElapsed() << " seconds to read ROIs.\n";
	timer.Restart();
	// control cohort
	string folderStr = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\control\\";
	MySegNodeInfoAssembleSPtr segAsmb_control
		= MyDataReader::ConstructAssembleFromDirectory(folderStr.c_str(), ROIs.get(), mAsmbVolLimit);
	cout << timer.GetElapsed() << " seconds to construct "
		<< segAsmb_control->GetSegmentNodeInfos()->front()->GetVolumes()->size() << " control cohort\n";
	timer.Restart();
	// scz cohort
	string folderStr1 = "C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\skeletons\\scz\\";
	MySegNodeInfoAssembleSPtr segAsmb_scz
		= MyDataReader::ConstructAssembleFromDirectory(folderStr1.c_str(), ROIs.get(), mAsmbVolLimit);
	cout << timer.GetElapsed() << " seconds to construct "
		<< segAsmb_scz->GetSegmentNodeInfos()->front()->GetVolumes()->size() << " scz cohort\n";
	timer.Restart();
	// group
	MySegmentAssembleGroupSPtr asmbGroup = make_shared<MySegmentAssembleGroup>();
	asmbGroup->PushBack(segAsmb_control);
	asmbGroup->PushBack(segAsmb_scz);
	asmbGroup->Update();
	MyMapScPtr<const MySegmentNode*, MyVec4f> ROI_color = asmbGroup->GetRoiColors();
	cout << timer.GetElapsed() << " seconds to construct assemble group.\n";
	timer.Restart();

	// 0. spatial view
	cout << "constructing mesh...\r";
	MyMesh mesh0, mesh1;
	mesh0.Read("lh.pial.obj");
	mesh1.Read("rh.pial.obj");
	mesh0.Merge(mesh1);
	mesh0.GenPerVertexNormal();
	cout << "Mesh Center: " << mesh0.GetBoundingBox().GetCenter()[0] << ", "
		<< mesh0.GetBoundingBox().GetCenter()[1] << ", "
		<< mesh0.GetBoundingBox().GetCenter()[2] << endl;
	cout << "Mesh Size: " << mesh0.GetBoundingBox().GetRange(0) << ", "
		<< mesh0.GetBoundingBox().GetRange(1) << ", "
		<< mesh0.GetBoundingBox().GetRange(2) << endl;
	MySurfaceRendererSPtr meshRenderer = std::make_shared<MySurfaceRenderer>();
	unsigned int meshShader = InitShader(
		"shaders\\mesh.vert", "shaders\\mesh.frag", "fragColour", "name");
	meshRenderer->SetShaderProgram(meshShader);
	cout << "Mesh Shader " << meshShader << " compiled.\n";
	meshRenderer->SetGeometry(&mesh0.GetVertices(), &mesh0.GetNormals(), &mesh0.GetTriangles());
	meshRenderer->SetName(MyVec4i(99, 99, 99, 0));
	meshRenderer->SetTransparency(5);
	meshRenderer->Update();
	mSpatialView = std::make_shared<MySpatialView>();
	mSpatialView->SetMeshRenderer(meshRenderer);
	mSpatialView->SetIndex(0);
	cout << timer.GetElapsed() << " seconds to load mesh to spatial view.\n";
	timer.Restart();

	// 1. joinTree view
	MyJoinTreeSPtr joinTree = make_shared<MyJoinTree>();
	MyComponentFilterSPtr filter = make_shared<MyComponentFilter>();
	filter->SetSizeThreshold(20);
	joinTree->SetVolumn(vol);
	joinTree->SetComponentFilter(filter);
	joinTree->Update();
	cout << timer.GetElapsed() << " seconds to make join tree.\n";
	timer.Restart();
	MyJoinTreeLayoutSPtr layout = make_shared<MyJoinTreeLayout>();
	layout->SetJoinTreeRoot(joinTree->GetRoot());
	layout->Update();
	cout << timer.GetElapsed() << " seconds to layout join tree.\n";
	timer.Restart();
	MyJoinTreeDrawerSPtr joinTreeDrawer = std::make_shared<MyJoinTreeDrawer>();
	joinTreeDrawer->SetJoinTree(joinTree);
	joinTreeDrawer->SetJoinTreeLayout(layout);

	MySegmentNodeMatchCounterSPtr match = make_shared<MySegmentNodeMatchCounter>();
	match->SetGroupByArray(ROIs.get(), 0);
	match->SetGroupByTree(joinTree->GetRoot().get(), 1);
	match->Update();
	cout << timer.GetElapsed() << " seconds to compute match.\n";
	timer.Restart();

	MyBloomDrawingHelperSPtr bloomDrawer = make_shared<MyBloomDrawingHelper>();
	bloomDrawer->SetLayout(layout);
	bloomDrawer->SetMatchCounter(match);
	bloomDrawer->SetSegmentColor(ROI_color);

	MyRoiStatisticsDrawerSPtr statDrawer = make_shared<MyRoiStatisticsDrawer>();
	statDrawer->SetLayout(layout);
	statDrawer->SetGroupVolume(
		segAsmb_control->GetSegmentNodeInfos()->front()->GetVolumes(), 0);
	statDrawer->SetGroupVolume(
		segAsmb_scz->GetSegmentNodeInfos()->front()->GetVolumes(), 1);
	statDrawer->SetGroupColor(MyVec4f(0, 0, 1, 0.5), 0);
	statDrawer->SetGroupColor(MyVec4f(1, 0, 0, 0.5), 1);
	statDrawer->AddRoiByTree(joinTree->GetRoot().get());
	statDrawer->Update();

	mJoinTreeView = std::make_shared<MyJoinTreeView>();
	mJoinTreeView->SetJoinTreeDrawer(joinTreeDrawer);
	mJoinTreeView->SetMyBloomDrawingHelper(bloomDrawer);
	mJoinTreeView->SetStatisticsDrawer(statDrawer);
	mJoinTreeView->SetIndex(1);
	cout << timer.GetElapsed() << " seconds to construct joinTree view.\n";
	timer.Restart();

	// 2. roi view
	MyMapSPtr<int, MyString> labels = MyDataReader::LoadRegionLabel("GOBS_look_up_table.txt");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ctr_10.trk");
	//MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\ACR.trk");
	MyTracksSPtr tracks = std::make_shared<MyTracks>("C:\\Users\\GuohaoZhang\\Desktop\\tmpdata\\dti.trk");
	cout << timer.GetElapsed() << " seconds to read tracks.\n";
	timer.Restart();
	MySegTrkNetworkSPtr network = std::make_shared<MySegTrkNetwork>();
	network->SetTracks(tracks);
	network->SetSegmentNodeInfos(segAsmb_control->GetSegmentNodeInfos());
	network->Update();
	cout << timer.GetElapsed() << " seconds to compute connections.\n";
	timer.Restart();
	MySegNodeInfoLayout2DSPtr seglayout = std::make_shared<MySegNodeInfoLayout2D>();
	seglayout->SetBoxesIn(segAsmb_control->GetSegment2DBoxes());
	seglayout->SetSegmentAssembleGroup(asmbGroup);
	seglayout->Update();
	MySegsPlanarDrawerSPtr segsDrawer = make_shared<MySegsPlanarDrawer>();
	segsDrawer->SetLayout(seglayout);
	segsDrawer->SetSegmentAssembleGroup(asmbGroup);
	segsDrawer->SetSegTrkNetwork(network);
	segsDrawer->GetLabelManager()->SetFont(font);
	segsDrawer->SetLabels(labels);
	//segsDrawer->CompileShader();
	segsDrawer->SetSegNodeColor(ROI_color);
	segsDrawer->Update();
	mRoiView = make_shared<MyRoiView>();
	mRoiView->SetMySegsPlanarDrawer(segsDrawer);
	mRoiView->SetIndex(2);
	seglayout->SetBoxStatus(mRoiView->GetStatus());
	cout << timer.GetElapsed() << " seconds to construct ROI view.\n";
	timer.Restart();

	MyLineConnectorDrawerSPtr connectDrawer = make_shared<MyLineConnectorDrawer>();
	connectDrawer->SetJoinTreeLayout(layout);
	connectDrawer->SetRoiLayout(seglayout);
	connectDrawer->SetMatchCounter(match);
	connectDrawer->SetJoinTreeView(mJoinTreeView);
	connectDrawer->SetRoiView(mRoiView);
	connectDrawer->SetRoiColors(ROI_color);
	connectDrawer->SetFont(font);
	mConnectorView = std::make_shared<MyConnectorView>();
	mConnectorView->SetConnectorDrawer(connectDrawer);
	mConnectorView->SetIndex(3);
	cout << timer.GetElapsed() << " seconds to construct connector view.\n";
	timer.Restart();

	// connect all slots
	this->ConnectAll();

	//glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	mUiPanel.AddUIs();
}
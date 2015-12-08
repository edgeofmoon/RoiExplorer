#include "MyApp.h"
#include "MyIsosurfaceTracker.h"
#include "OSCB.h"
#include "MyDataReader.h"

//debug
#include <iostream>
using namespace std;

int MyApp::UpdateIsosurface(int x, int y){
	MyVec2i pixelPos(x, y);
	const MySegmentNode* seg = mJoinTreeView->ComputeSegmentNodeAt(pixelPos);
	if (seg){
		MyVec4i name = mJoinTreeView->GetSegmentNodeName(seg);
		// find seeds, which must be a value larger than isovalue
		My3dArrayfScPtr vol 
			= mJoinTreeView->GetJoinTreeDrawer()->GetJoinTree()->GetVolumn();
		MyVec2f pos = mJoinTreeView->ComputeGeometryPosition(pixelPos);
		int startIdx = this->FindLowestIndex(seg->GetUniqueVoxes().get(), vol.get());
		MyIsosurfaceTracker tracker;
		tracker.SetStartIndex(startIdx);
		tracker.SetVolumn(vol);
		tracker.SetIsovalue(pos[1]);
		tracker.Update();
		MySurfaceRendererSPtr renderer = std::make_shared<MySurfaceRenderer>();
		renderer->SetGeometry(tracker.GetVertices().get(),
			tracker.GetNormals().get(), tracker.GetTriangles().get());
		renderer->SetName(name);
		renderer->Update();
		// update spatial view
		MySurfaceRendererSPtr oldRenderer 
			= mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(oldRenderer);
		mSpatialView->AddSurfaceRenderer(renderer);
		// give it a mark
		cout << "Updating Marks: " << seg << " at isoValue: " << pos[1] << endl;
		mJoinTreeView->UpdateMarks(seg, pos[1]);
		return 1;
	}
	return 0;
}

int MyApp::RemoveIsosurface(int x, int y){
	MyVec2i pixelPos(x, y);
	const MySegmentNode* seg = mJoinTreeView->ComputeSegmentNodeAt(pixelPos);
	if (seg){
		MyVec4i name = mJoinTreeView->GetSegmentNodeName(seg);
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		// remove the mark
		mJoinTreeView->RemoveMarks(seg);
		return 1;
	}
	return 0;
}

int MyApp::UpdateRoiSurface(int x, int y){
	MyVec2i pixelPos(x, y);
	const MySegmentNode* roi = mRoiView->ComputeSegmentNodeAt(pixelPos);
	if (roi){
		this->AddRoiSurface(roi);
		return 1;
	}
	return 0;
}

int MyApp::RemoveRoisurface(int x, int y){
	MyVec2i pixelPos(x, y);
	if (!mRoiView->IsIntersected(pixelPos)){
		return 0;
	}
	const MySegmentNode* roi = mRoiView->ComputeSegmentNodeAt(pixelPos);
	if (roi){
		this->RemoveRoiSurface(roi);
		return 1;
	}
	return 0;
}

int MyApp::RemoveSurface(int x, int y){
	MyVec2i pixelPos(x, y);
	if (!mSpatialView->IsIntersected(pixelPos)){
		return 0;
	}
	MyVec4i name = mSpatialView->GetName(pixelPos);
	cout << "spatial name: " << name[0] << ", " << name[1] << ", "
		<< name[2] << ", " << name[3] << endl;
	if (name[0] >= 0){
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		if (name[0] == 1){
			const MySegmentNode* node = mJoinTreeView->GetSegmentNodeByName(name);
			mJoinTreeView->RemoveMarks(node);
		}
		else if (name[0] == 2){
			const MySegmentNode* node = mRoiView->GetSegmentNodeByName(name);
			mRoiView->UnsetSelect(node);
		}
		return 1;
	}
	return 0;
}

int MyApp::CreateRoiFromJoinTree(){
	MyMapSPtr<const MySegmentNode*, float> marks 
		= mJoinTreeView->ComputeSeparableMarks();
	cout << "Clear Marks#: " << marks->size() << endl;
	MyMap<const MySegmentNode*, float>::const_iterator itr;
	for (itr = marks->begin(); itr != marks->end(); itr++){
		cout << "Mark: " << itr->first << endl;
		// make all voxels
		MySegmentNode::const_VoxelIterator voxItr = itr->first->VoxelBegin();
		MySegmentNode::const_VoxelIterator voxEnd = itr->first->VoxelEnd();
		MyArrayi voxIndices;
		MyArrayf voxValues;
		int numVoxels = 0;
		//voxIndices.reserve(this->GetNumTotalVoxes());
		while (voxItr != voxEnd){
			if (*voxItr > itr->second){
				voxIndices << voxItr.GetVoxelIndex();
				voxValues << *voxItr;
				numVoxels++;
			}
			voxItr++;
		}
		cout << "newRoi numVoxels: " << numVoxels << endl;
		MyVoxContainerfSPtr voxels = MyVoxContainerf::MakeVoxContainer(&voxIndices, 
			&voxValues, itr->first->GetVolumeSize());
		MySegmentNodeSPtr newRoi = std::make_shared<MySegmentNode>();
		newRoi->SetAutoIndex();
		// add voxels to this roi
		newRoi->SetUniqueVoxes(voxels);
		// add to roi list
		mAsmbGroup->AddSegmentNode(newRoi);
		// update everything related
		mAsmbGroup->Update();
		mRoiView->GetRoiDrawer()->GetNetwork()->Update();
		mRoiView->GetRoiDrawer()->GetLayoutManager()->
			SetBoxesIn(mAsmbGroup->front()->GetSegment2DBoxes());
		mRoiView->GetRoiDrawer()->GetLayoutManager()->Update();
		mRoiView->GetRoiDrawer()->Update();
		mConnectorView->GetConnectorDrawer()->GetMatchCounter()
			->AddToGroupAndUpdate(newRoi.get(), 0);
		return 1;
	}
	return 1;
}

int MyApp::RemoveRoi(){
	if (mRoiView->GetRoiDrawer()->GetLayoutManager()->GetBoxesOut()->size() < 2){
		// lets at least keep 1 Roi
		cout << "At least 1 ROI must be there!" << endl;
		return 0;
	}
	MyMapScPtr<const MySegmentNode*, MyObjectStatus> marks = mRoiView->GetStatus();
	MyMap<const MySegmentNode*, MyObjectStatus>::const_iterator itr;
	bool hasUpdate = false;
	for (itr = marks->begin(); itr != marks->end(); ){
		// remove from match
		mConnectorView->GetConnectorDrawer()->GetMatchCounter()
			->RemoveSegment(itr->first, 0);
		// remove from spatial view
		MyVec4i name = mRoiView->GetSegmentNodeName(itr->first);
		cout << "NameToRm: " << name[0] << ", " << name[1] << ", "
			<< name[2] << ", " << name[3] << " ("
			<< itr->first->GetIndex() << ") " << endl;
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		// remove the instance
		mAsmbGroup->RemoveSegmentNode(itr->first);
		// must use inplace increment to avoid iterator invalidness
		mRoiView->UnsetSelect(itr++->first);
		hasUpdate = true;
	}
	if (hasUpdate){
		// update everything related
		mAsmbGroup->Update();
		mRoiView->GetRoiDrawer()->GetNetwork()->Update();
		mRoiView->GetRoiDrawer()->GetLayoutManager()->
			SetBoxesIn(mAsmbGroup->front()->GetSegment2DBoxes());
		mRoiView->GetRoiDrawer()->GetLayoutManager()->Update();
		mRoiView->GetRoiDrawer()->Update();
		return 1;
	}
	return 0;
}

int MyApp::DisableRoi(){
	MyMapScPtr<const MySegmentNode*, MyObjectStatus> marks = mRoiView->GetStatus();
	MyMap<const MySegmentNode*, MyObjectStatus>::const_iterator itr;
	bool hasUpdate = false;
	for (itr = marks->begin(); itr != marks->end();){
		// remove from match
		mConnectorView->GetConnectorDrawer()->GetMatchCounter()
			->RemoveSegment(itr->first, 0);
		// remove from spatial view
		MyVec4i name = mRoiView->GetSegmentNodeName(itr->first);
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		// remove the instance
		mRoiView->SetDisable(itr->first);
		mRoiView->UnsetSelect(itr++->first);
		hasUpdate = true;
	}
	if (hasUpdate){
		// update everything related
		mRoiView->GetRoiDrawer()->GetLayoutManager()->
			SetBoxesIn(mAsmbGroup->front()->GetSegment2DBoxes());
		mRoiView->GetRoiDrawer()->GetLayoutManager()->Update();
		mRoiView->GetRoiDrawer()->Update();
		return 1;
	}
	return 0;
}

int MyApp::EnableRoi(){
	MyMapScPtr<const MySegmentNode*, MyObjectStatus> marks = mRoiView->GetStatus();
	MyMap<const MySegmentNode*, MyObjectStatus>::const_iterator itr;
	bool hasUpdate = false;
	for (itr = marks->begin(); itr != marks->end();){
		// first check if it is already enabled
		if (!itr->second.IsBitSet(MyObjectStatus::STATUS_DISABLE_BIT)){
			itr++;
			continue;
		}
		// next check if it is selected
		if (!itr->second.IsBitSet(MyObjectStatus::STATUS_SELECT_BIT)){
			itr++;
			continue;
		}
		// add to match
		mConnectorView->GetConnectorDrawer()->GetMatchCounter()
			->AddToGroupAndUpdate(itr->first, 0);

		// add to spatial view
		MyVec4i name = mRoiView->GetSegmentNodeName(itr->first);
		MySurfaceRendererSPtr oldRenderer = mSpatialView->GetSurfaceRendererByName(name);
		if (!oldRenderer){
			My3dArrayfSPtr vol = itr->first->GetUniqueVoxes()->MakeVolume();
			MyIsosurfaceTracker tracker;
			// as always, use min
			int currentLowestIndex = this->FindLowestIndex(itr->first->GetUniqueVoxes().get());
			tracker.SetStartIndex(currentLowestIndex);
			tracker.SetVolumn(vol);
			// any number slightly higher than 0 
			// and lower than min will do
			tracker.SetIsovalue(0.0001f);
			tracker.Update();
			MyVec4f color = mAsmbGroup->GetRoiColors()->at(itr->first);
			MySurfaceRendererSPtr renderer = std::make_shared<MySurfaceRenderer>();
			renderer->SetGeometry(tracker.GetVertices().get(),
				tracker.GetNormals().get(), tracker.GetTriangles().get());
			renderer->SetName(name);
			renderer->SetColor(color);
			renderer->Update();
			// update spatial view
			mSpatialView->AddSurfaceRenderer(renderer);
		}

		// enable the instance
		mRoiView->UnsetDisable(itr++->first);
		hasUpdate = true;
	}
	if (hasUpdate){
		// update everything related
		mRoiView->GetRoiDrawer()->GetLayoutManager()->
			SetBoxesIn(mAsmbGroup->front()->GetSegment2DBoxes());
		mRoiView->GetRoiDrawer()->GetLayoutManager()->Update();
		mRoiView->GetRoiDrawer()->Update();
		return 1;
	}
	return 0;
}

int MyApp::UpdateJoinTree(){
	OSCB::OpenFileDialog* openFileDialog = new OSCB::OpenFileDialog();
	openFileDialog->Flags |= OFN_NOCHANGEDIR;
	if (openFileDialog->ShowDialog()){
		// first remove isosurface in spatial view
		const MyMap<const MySegmentNode*, float>& marks 
			= mJoinTreeView->GetMarks();;
		MyMap<const MySegmentNode*, float>::const_iterator itr;
		for (itr = marks.begin(); itr != marks.end(); ){
			MyVec4i name = mJoinTreeView->GetSegmentNodeName(itr->first);
			MySurfaceRendererSPtr renderer 
				= mSpatialView->GetSurfaceRendererByName(name);
			mSpatialView->RemoveSurfaceRenderer(renderer);
			mJoinTreeView->RemoveMarks(itr++->first);
		}
		// build new join tree view
		char DefChar = ' ';
		char ch[260];
		// convert wchar to char
		WideCharToMultiByte(CP_ACP, 0, openFileDialog->FileName, 
			-1, ch, 260, &DefChar, NULL);
		My3dArrayfSPtr vol = MyDataReader::LoadVolumeFromFile(ch);
		MyJoinTreeSPtr joinTree = make_shared<MyJoinTree>();
		MyComponentFilterSPtr filter = make_shared<MyComponentFilter>();
		filter->SetSizeThreshold(20);
		joinTree->SetVolumn(vol);
		joinTree->SetComponentFilter(filter);
		joinTree->Update();
		MyJoinTreeDrawerSPtr joinTreeDrawer =
			mJoinTreeView->GetJoinTreeDrawer();
		joinTreeDrawer->SetJoinTree(joinTree);
		joinTreeDrawer->Update();
		MySegmentNodeMatchCounterSPtr match 
			= mConnectorView->GetConnectorDrawer()->GetMatchCounter();
		match->SetGroupByTree(joinTree->GetRoot().get(), 1);
		match->Update();
		// update distribution drawers
		MyRoiStatisticsDrawerSPtr statDrawer
			= mJoinTreeView->GetStatisticsDrawer();
		statDrawer->ClearRois();
		statDrawer->AddRoiByTree(joinTree->GetRoot().get());
		return 1;
	}
	delete openFileDialog;
	return 0;
}

int MyApp::UpdateAssembleGroup(){
	static string lastPath("C:\\");
	string path = OSCB::BrowseFolder(lastPath);
	lastPath = path;
	string path_ctr = path + "\\control\\";
	string path_scz = path + "\\scz\\";
	cout << "Update control group to " << path_ctr << endl;
	cout << "Update scz group to " << path_scz << endl;
	if (!path.empty()){

		// retrive ROIs
		MyArray<MySegmentNodeSPtr> ROIs;
		MyArrayScPtr<MySegmentNodeInfoScPtr> segInfos
			= mAsmbGroup->front()->GetSegmentNodeInfos();
		for (int i = 0; i < segInfos->size(); i++){
			ROIs << segInfos->at(i)->GetSegmentNode();
		}
		MySegNodeInfoAssembleSPtr segAsmb_control
			= MyDataReader::ConstructAssembleFromDirectory(path_ctr.c_str(), &ROIs);
		MySegNodeInfoAssembleSPtr segAsmb_scz
			= MyDataReader::ConstructAssembleFromDirectory(path_scz.c_str(), &ROIs);
		mAsmbGroup->clear();
		mAsmbGroup->PushBack(segAsmb_control);
		mAsmbGroup->PushBack(segAsmb_scz);
		mAsmbGroup->Update();
		MySegNodeInfoLayout2DSPtr seglayout 
			= mRoiView->GetRoiDrawer()->GetLayoutManager();
		seglayout->SetBoxesIn(segAsmb_control->GetSegment2DBoxes());
		seglayout->Update();
		// update distribution drawers
		MyRoiStatisticsDrawerSPtr statDrawer
			= mJoinTreeView->GetStatisticsDrawer();
		statDrawer->SetGroupVolume(segAsmb_control->GetSegmentNodeInfos()->front()->GetVolumes(), 0);
		statDrawer->SetGroupVolume(segAsmb_scz->GetSegmentNodeInfos()->front()->GetVolumes(), 1);
		return 1;
	}
	return 0;
}
#include "MyApp.h"
#include "MyIsosurfaceTracker.h"

void MyApp::ConnectAll(){
	mRoiView->Signal_SegmentSelected.Connect(this, &MyApp::AddRoiSurface);
	mRoiView->Signal_SegmentUnselected.Connect(this, &MyApp::RemoveRoiSurface);
}

void MyApp::AddRoiSurface(const MySegmentNode* roi){
	if (roi){
		MyVec4i name = mRoiView->GetSegmentNodeName(roi);
		MySurfaceRendererSPtr oldRenderer = mSpatialView->GetSurfaceRendererByName(name);
		if (oldRenderer) return;
		My3dArrayfScPtr joinTreeVol
			= mJoinTreeView->GetJoinTreeDrawer()->GetJoinTree()->GetVolumn();
		My3dArrayfSPtr vol = roi->GetUniqueVoxes()->MakeVolume();
		MyIsosurfaceTracker tracker;
		// as always, use min
		int currentLowestIndex = this->FindLowestIndex(roi->GetUniqueVoxes().get());
		tracker.SetStartIndex(currentLowestIndex);
		tracker.SetVolumn(vol);
		// any number slightly higher than 0 
		// and lower than min will do
		tracker.SetIsovalue(0.0001f);
		tracker.Update();
		MyVec4f color = mAsmbGroup->GetRoiColors()->at(roi);
		MySurfaceRendererSPtr renderer = std::make_shared<MySurfaceRenderer>();
		renderer->SetGeometry(tracker.GetVertices().get(),
			tracker.GetNormals().get(), tracker.GetTriangles().get());
		renderer->SetName(name);
		renderer->SetColor(color);
		renderer->Update();
		// update spatial view
		mSpatialView->AddSurfaceRenderer(renderer);
		// give it a mark
		mRoiView->SetSelect(roi);
	}
}

void MyApp::RemoveRoiSurface(const MySegmentNode* roi){
	if (roi){
		MyVec4i name = mRoiView->GetSegmentNodeName(roi);
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		// remove the mark
		mRoiView->UnsetSelect(roi);
	}
}
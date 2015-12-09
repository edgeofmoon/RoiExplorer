#include "MyApp.h"
#include "MyIsosurfaceTracker.h"

void MyApp::ConnectAll(){
	mRoiView->Signal_SegmentSelected.Connect(this, &MyApp::AddRoiSurface);
	mRoiView->Signal_SegmentUnselected.Connect(this, &MyApp::RemoveRoiSurface);
}

void MyApp::AddRoiSurface(MyVec4i name){
	const MySegmentNode* roi = this->GetSegmentByName(name);
	if (roi){
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
		MyVec4f color = this->GetRoiViewByName(name)->GetRoiDrawer()
			->GetSegmentAssembleGroup()->GetRoiColors()->at(roi);
		MySurfaceRendererSPtr renderer = std::make_shared<MySurfaceRenderer>();
		renderer->SetGeometry(tracker.GetVertices().get(),
			tracker.GetNormals().get(), tracker.GetTriangles().get());
		renderer->SetName(name);
		renderer->SetColor(color);
		renderer->Update();
		// update spatial view
		mSpatialView->AddSurfaceRenderer(renderer);
		// give it a mark
		if (name[0] == mRoiView->GetIndex()) mRoiView->SetSelect(roi);
		if (mSecondRoiView){
			if (name[0] == mSecondRoiView->GetIndex()) {
				mSecondRoiView->SetSelect(roi);
			}
		}
	}
}

void MyApp::RemoveRoiSurface(MyVec4i name){
	const MySegmentNode* roi = this->GetSegmentByName(name);
	if (roi){
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		// remove the mark
		if (name[0] == mRoiView->GetIndex()) mRoiView->UnsetSelect(roi);
		if (mSecondRoiView){
			if (name[0] == mSecondRoiView->GetIndex()){
				mSecondRoiView->UnsetSelect(roi);
			}
		}
	}
}


const MySegmentNode* MyApp::GetSegmentByName(const MyVec4i& name) const{
	int index = name[0];
	switch (index)
	{
	case 1:
		return mJoinTreeView->GetSegmentNodeByName(name);
		break;
	case 2:
		return mRoiView->GetSegmentNodeByName(name);
		break;
	case 3:
		return mSecondRoiView->GetSegmentNodeByName(name);
		break;
	default:
		return 0;
		break;
	}
}

MyRoiViewSPtr MyApp::GetRoiViewByName(const MyVec4i& name) const{
	int index = name[0];
	switch (index)
	{
	case 2:
		return mRoiView;
		break;
	case 3:
		return mSecondRoiView;
		break;
	default:
		return 0;
		break;
	}
}